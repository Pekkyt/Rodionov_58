#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <cmath>
#include <stack>
#include <sstream>
#include <cctype>
#include <clocale>
#include <cstdlib>
#include "Header.h"
using namespace std;
static int NEXT_NODE_ID = 0;
static int NEXT_ACTION_ID = 1;

void readInvalidNumberToken(const string& expression, int& currentPosition, int tokenStartPosition, vector<Error>& errors)
{
    // Считать некорректное число целиком
    while (currentPosition < (int)expression.size() && (isdigit((unsigned char)expression[currentPosition]) || expression[currentPosition] == '.' || expression[currentPosition] == ','))
    {
        currentPosition++;
    }
    // Добавить ошибку INVALID_NUMBER_FORMAT
    errors.push_back({ ErrorType::INVALID_NUMBER_FORMAT, tokenStartPosition, expression.substr(tokenStartPosition, currentPosition - tokenStartPosition) });
}

bool readNumberToken(const string& expression, int& currentPosition, vector<Token>& tokens, vector<Error>& errors)
{
    // Если текущий символ не является цифрой, число не считывается
    if (!isdigit((unsigned char)expression[currentPosition]))
    {
        return false;
    }
    // Запомнить позицию начала числа
    int tokenStartPosition = currentPosition;
    // Считать целую часть числа
    while (currentPosition < (int)expression.size() && isdigit((unsigned char)expression[currentPosition]))
    {
        currentPosition++;
    }
    // Если после целой части встречена точка
    if (currentPosition < (int)expression.size() && expression[currentPosition] == '.')
    {
        // Добавить точку в число
        currentPosition++;
        // Проверить, что после точки следует хотя бы одна цифра
        if (currentPosition >= (int)expression.size() || !isdigit((unsigned char)expression[currentPosition]))
        {
            readInvalidNumberToken(expression, currentPosition, tokenStartPosition, errors);
            return true;
        }
        // Считать дробную часть числа
        while (currentPosition < (int)expression.size() && isdigit((unsigned char)expression[currentPosition]))
        {
            currentPosition++;
        }
    }
    // Если после числа снова встретилась точка или запятая, формат числа некорректен
    if (currentPosition < (int)expression.size() && (expression[currentPosition] == '.' || expression[currentPosition] == ','))
    {
        readInvalidNumberToken(expression, currentPosition, tokenStartPosition, errors);
        return true;
    }
    // Получить строковое представление числа
    string numberText = expression.substr(tokenStartPosition, currentPosition - tokenStartPosition);
    // Проверить, что модуль числа не превышает 2147483647
    double numberValue = stod(numberText);
    if (fabs(numberValue) > 2147483647.0)
    {
        errors.push_back({ ErrorType::NUMBER_OUT_OF_RANGE, tokenStartPosition, numberText });
        return true;
    }
    // Добавить токен числа в список
    tokens.push_back({ numberText, tokenStartPosition, TokenType::NUMBER });
    return true;
}

bool readVariableToken(const string& expression, int& currentPosition, vector<Token>& tokens)
{
    // Если текущий символ не является латинской буквой, переменная не считывается
    if ((unsigned char)expression[currentPosition] >= 128 ||
        !isalpha((unsigned char)expression[currentPosition]))
    {
        return false;
    }
    // Запомнить позицию начала имени переменной
    int tokenStartPosition = currentPosition;
    // Считать имя переменной целиком
    while (currentPosition < (int)expression.size() && (unsigned char)expression[currentPosition] < 128 && isalnum((unsigned char)expression[currentPosition]))
    {
        currentPosition++;
    }
    // Добавить токен переменной в список
    tokens.push_back({ expression.substr(tokenStartPosition, currentPosition - tokenStartPosition), tokenStartPosition, TokenType::VARIABLE });
    return true;
}

bool readOperatorToken(const string& expression, int& currentPosition, vector<Token>& tokens)
{
    // Если текущий символ не является оператором или скобкой
    if (string("+-*/^()").find(expression[currentPosition]) == string::npos)
    {
        return false;
    }
    // Сохранить текст текущего символа
    string tokenText(1, expression[currentPosition]);
    // По умолчанию считать токен оператором
    TokenType tokenType = TokenType::OPERATOR;
    // Если символ является открывающей скобкой
    if (expression[currentPosition] == '(')
    {
        tokenType = TokenType::LEFT_BRACKET;
    }
    // Если символ является закрывающей скобкой
    else if (expression[currentPosition] == ')')
    {
        tokenType = TokenType::RIGHT_BRACKET;
    }
    // Если символ является минусом, определить: унарный он или бинарный
    else if (expression[currentPosition] == '-')
    {
        bool isUnary = tokens.empty() || tokens.back().type == TokenType::LEFT_BRACKET || tokens.back().type == TokenType::OPERATOR;
        tokenText = isUnary ? "~" : "-";
    }
    // Добавить соответствующий токен в список
    tokens.push_back({ tokenText, currentPosition, tokenType });
    // Перейти к следующему символу
    currentPosition++;
    return true;
}

void validateTokenSequence(const vector<Token>& tokens, vector<Error>& errors)
{
    // После завершения прохода проверить токены
    for (int tokenIndex = 0; tokenIndex < (int)tokens.size(); tokenIndex++)
    {
        // Определить, является ли текущий токен бинарным оператором
        bool currentIsBinaryOperator = tokens[tokenIndex].type == TokenType::OPERATOR && tokens[tokenIndex].text != "~";
        // Если бинарный оператор стоит в начале выражения
        if (tokenIndex == 0 && currentIsBinaryOperator)
        {
            errors.push_back({ ErrorType::MISSING_OPERAND, tokens[tokenIndex].position, tokens[tokenIndex].text });
        }
        // Если бинарный оператор стоит в конце выражения
        if (tokenIndex == (int)tokens.size() - 1 && currentIsBinaryOperator)
        {
            errors.push_back({ ErrorType::MISSING_OPERAND, tokens[tokenIndex].position, tokens[tokenIndex].text });
        }
        // Проверка соседних токенов выполняется только если предыдущий токен существует
        if (tokenIndex > 0)
        {
            // Определить, является ли предыдущий токен операндом или закрывающей скобкой
            bool previousIsOperand = tokens[tokenIndex - 1].type == TokenType::NUMBER || tokens[tokenIndex - 1].type == TokenType::VARIABLE || tokens[tokenIndex - 1].type == TokenType::RIGHT_BRACKET;
            // Определить, является ли текущий токен операндом или открывающей скобкой
            bool currentIsOperandOrLeftBracket = tokens[tokenIndex].type == TokenType::NUMBER || tokens[tokenIndex].type == TokenType::VARIABLE || tokens[tokenIndex].type == TokenType::LEFT_BRACKET;
            // Если подряд идут два операнда или после операнда сразу идет открывающая скобка
            if (previousIsOperand && currentIsOperandOrLeftBracket)
            {
                errors.push_back({ ErrorType::MISSING_OPERATOR, tokens[tokenIndex].position, tokens[tokenIndex].text });
            }
            // Определить, является ли предыдущий токен бинарным оператором
            bool previousIsBinaryOperator = tokens[tokenIndex - 1].type == TokenType::OPERATOR && tokens[tokenIndex - 1].text != "~";
            // Если подряд идут два бинарных оператора
            if (previousIsBinaryOperator && currentIsBinaryOperator)
            {
                errors.push_back({ ErrorType::MISSING_OPERAND, tokens[tokenIndex].position, tokens[tokenIndex].text });
            }
            // Если после открывающей скобки идет бинарный оператор
            if (tokens[tokenIndex - 1].type == TokenType::LEFT_BRACKET && currentIsBinaryOperator)
            {
                errors.push_back({ ErrorType::MISSING_OPERAND, tokens[tokenIndex].position, tokens[tokenIndex].text });
            }
        }
    }
}

vector<Token> tokenize(const string& expression, vector<Error>& errors)
{
    // Создать пустой список токенов
    vector<Token> tokens;
    // Установить индекс текущего символа в начало строки
    int currentPosition = 0;
    // Пока индекс меньше длины строки
    while (currentPosition < (int)expression.size())
    {
        // Если текущий символ - пробел, перейти к следующему символу
        if (isspace((unsigned char)expression[currentPosition]))
        {
            currentPosition++;
        }
        // Если текущий символ - цифра, считать числовой токен
        else if (isdigit((unsigned char)expression[currentPosition]))
        {
            readNumberToken(expression, currentPosition, tokens, errors);
        }
        // Если число начинается с точки или запятой, формат числа некорректен
        else if (expression[currentPosition] == '.' || expression[currentPosition] == ',')
        {
            int tokenStartPosition = currentPosition;
            readInvalidNumberToken(expression, currentPosition, tokenStartPosition, errors);
        }
        // Если текущий символ - латинская буква, считать имя переменной
        else if ((unsigned char)expression[currentPosition] < 128 && isalpha((unsigned char)expression[currentPosition]))
        {
            readVariableToken(expression, currentPosition, tokens);
        }
        // Если текущий символ - оператор или скобка, считать соответствующий токен
        else if (string("+-*/^()").find(expression[currentPosition]) != string::npos)
        {
            readOperatorToken(expression, currentPosition, tokens);
        }
        // Если текущий символ не относится ни к одному допустимому случаю
        else
        {
            errors.push_back({ ErrorType::INVALID_SYMBOL, currentPosition, string(1, expression[currentPosition]) });
            currentPosition++;
        }
    }
    // Если при разборе символов не было ошибок, проверить последовательность токенов
    if (errors.empty())
    {
        validateTokenSequence(tokens, errors);
    }
    // Вернуть список токенов
    return tokens;
}

vector<Token> toPostfix(const vector<Token>& tokens, vector<Error>& errors) {
    // Создать пустой выходной список
    vector<Token> postfix;
    // Создать пустой стек операторов
    stack<Token> operators;
    // Для каждого токена
    for (Token token : tokens)
    {
        // Если токен является числом или переменной
        if (token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE)
        {
            // Добавить его в выходной список
            postfix.push_back(token);
        }
        // Если токен является открывающей скобкой
        else if (token.type == TokenType::LEFT_BRACKET)
        {
            // Поместить его в стек
            operators.push(token);
        }
        // Если токен является закрывающей скобкой
        else if (token.type == TokenType::RIGHT_BRACKET)
        {
            // Извлекать операторы из стека в выходной список до открывающей скобки
            while (!operators.empty() && operators.top().type != TokenType::LEFT_BRACKET)
            {
                postfix.push_back(operators.top());
                operators.pop();
            }
            // Если открывающая скобка не найдена, добавить ошибку MISMATCHED_BRACKETS
            if (operators.empty())
            {
                errors.push_back({ ErrorType::MISMATCHED_BRACKETS, token.position, token.text });
                return {};
            }
            // Удалить открывающую скобку из стека
            operators.pop();
        }
        // Если токен является оператором
        else if (token.type == TokenType::OPERATOR)
        {

            // Пока стек не пуст, на вершине стека находится оператор, сравнить приоритет текущего оператора и оператора на вершине стека getPriority
            // Если оператор со стека должен быть выполнен раньше или одновременно, перенести его в выходной список
            while (!operators.empty() && operators.top().type == TokenType::OPERATOR && token.text != "~" && getPriority(operators.top().text) >= getPriority(token.text))
            {
                // Перенести оператор из стека в выходной список
                postfix.push_back(operators.top());
                operators.pop();
            }
            // Поместить текущий оператор в стек
            operators.push(token);
        }
    }
    // После обработки всех токенов перенести оставшиеся операторы из стека в выходной список
    while (!operators.empty())
    {
        // Если среди оставшихся элементов стека обнаружены скобки
        if (operators.top().type == TokenType::LEFT_BRACKET || operators.top().type == TokenType::RIGHT_BRACKET)
        {
            // Добавить ошибку MISMATCHED_BRACKETS
            errors.push_back({ ErrorType::MISMATCHED_BRACKETS,operators.top().position,operators.top().text });
            return {};
        }
        // Перенести оставшийся оператор в выходной список
        postfix.push_back(operators.top());
        operators.pop();
    }
    // Вернуть постфиксную форму выражения
    return postfix;
}

ExprNode* buildTree(const vector<Token>& postfix, vector<Error>& errors)
{
    // Создать пустой стек узлов дерева.
    stack<ExprNode*> nodes;
    // Для каждого токена постфиксной записи
    for (Token token : postfix)
    {
        // Если токен является числом
        if (token.type == TokenType::NUMBER)
        {
            // Создать узел типа NUMBER
            ExprNode* node = new ExprNode(stod(token.text));
            node->token = token.text;
            // Поместить узел в стек
            nodes.push(node);
        }
        // Если токен является переменной
        else if (token.type == TokenType::VARIABLE)
        {
            // Создать узел типа VARIABLE
            ExprNode* node = new ExprNode(0);
            node->type = ExprNodeType::VARIABLE;
            node->token = token.text;
            // Поместить узел в стек
            nodes.push(node);
        }
        // Если токен является унарным минусом
        else if (token.type == TokenType::OPERATOR && token.text == "~")
        {
            // Если стек пуст
            if (nodes.empty())
            {
                // Добавить ошибку MISSING_OPERAND
                errors.push_back({ ErrorType::MISSING_OPERAND, token.position, token.text });
                // Освободить память уже созданных узлов
                while (!nodes.empty())
                {
                    delete nodes.top();
                    nodes.pop();
                }
                // Вернуть nullptr
                return nullptr;
            }
            // Извлечь один узел из стека
            ExprNode* operand = nodes.top();
            nodes.pop();
            // Создать новый узел типа UNARY_MINUS
            // Сделать извлечённый узел потомком нового узла
            ExprNode* node = new ExprNode(ExprNodeType::UNARY_MINUS, operand, nullptr);
            node->token = token.text;
            // Поместить новый узел в стек
            nodes.push(node);
        }
        // Если токен является бинарной операцией
        else if (token.type == TokenType::OPERATOR)
        {
            // Если в стеке меньше двух узлов
            if (nodes.size() < 2)
            {
                // Добавить ошибку MISSING_OPERAND
                errors.push_back({ ErrorType::MISSING_OPERAND, token.position, token.text });
                // Освободить память уже созданных узлов
                while (!nodes.empty())
                {
                    delete nodes.top();
                    nodes.pop();
                }
                // Вернуть nullptr
                return nullptr;
            }
            // Извлечь правый операнд
            ExprNode* right = nodes.top();
            nodes.pop();
            // Извлечь левый операнд
            ExprNode* left = nodes.top();
            nodes.pop();
            // Создать новый узел соответствующего типа операции
            ExprNodeType type = ExprNodeType::ADD;
            if (token.text == "+") { type = ExprNodeType::ADD; }
            else if (token.text == "-") { type = ExprNodeType::SUB; }
            else if (token.text == "*") { type = ExprNodeType::MUL; }
            else if (token.text == "/") { type = ExprNodeType::DIV; }
            else if (token.text == "^") { type = ExprNodeType::POW; }
            // Связать новый узел с левым и правым потомками
            ExprNode* node = new ExprNode(type, left, right);
            node->token = token.text;
            // Поместить новый узел в стек
            nodes.push(node);
        }
    }
    // После обработки всех токенов
    // Если стек пуст, вернуть nullptr
    if (nodes.empty())
    {
        return nullptr;
    }
    // Если в стеке больше одного элемента
    if (nodes.size() > 1)
    {
        // Добавить ошибку MISSING_OPERATOR
        Error error = { ErrorType::MISSING_OPERATOR, -1, "" };
        if (!postfix.empty())
        {
            error.position = postfix.back().position;
            error.token = postfix.back().text;
        }
        errors.push_back(error);
        // Освободить память всех узлов в стеке
        while (!nodes.empty())
        {
            delete nodes.top();
            nodes.pop();
        }
        // Вернуть nullptr
        return nullptr;
    }
    // Вернуть верхний элемент стека как корень дерева
    return nodes.top();
}

ExprNode* parseExpression(const string& expression, vector<Error>& errors)
{
    // Разбить входную строку на токены функцией tokenize
    vector<Token> tokens = tokenize(expression, errors);
    // Если в процессе токенизации возникли ошибки парсинга
    if (!errors.empty())
    {
        // Вернуть nullptr
        return nullptr;
    }
    // Преобразовать список токенов в постфиксную форму функцией toPostfix
    vector<Token> postfix = toPostfix(tokens, errors);
    // Если при преобразовании в постфиксную форму возникли ошибки парсинга
    if (!errors.empty())
    {
        // Вернуть nullptr
        return nullptr;
    }
    // Построить дерево выражения функцией buildTree
    ExprNode* root = buildTree(postfix, errors);
    // Если при построении дерева возникли ошибки парсинга
    if (!errors.empty())
    {
        // Вернуть nullptr
        return nullptr;
    }
    // Если дерево не построено, добавить ошибку MISSING_OPERATOR
    if (root == nullptr)
    {
        errors.push_back({ ErrorType::MISSING_OPERATOR, -1, "" });
        return nullptr;
    }
    // Посчитать кол-во операций в дереве
    int operationsCount = 0;
    stack<ExprNode*> nodes;
    nodes.push(root);
    while (!nodes.empty())
    {
        ExprNode* current = nodes.top();
        nodes.pop();
        if (current->type == ExprNodeType::ADD ||
            current->type == ExprNodeType::SUB ||
            current->type == ExprNodeType::MUL ||
            current->type == ExprNodeType::DIV ||
            current->type == ExprNodeType::POW ||
            current->type == ExprNodeType::UNARY_MINUS)
        {
            operationsCount++;
        }
        if (current->left != nullptr)
        {
            nodes.push(current->left);
        }
        if (current->right != nullptr)
        {
            nodes.push(current->right);
        }
    }
    // Если количество операций превышает 100.
    if (operationsCount > 100)
    {
        // Добавить ошибку TOO_MANY_OPERATIONS
        errors.push_back({ ErrorType::TOO_MANY_OPERATIONS, -1, "" });
        // Удалить корень дерева выражения
        delete root;
        // Вернуть nullptr
        return nullptr;
    }
    // Вернуть корень дерева выражения
    return root;
}

ExprNode::ExprNode(double val)
{
    value = val;
    type = ExprNodeType::NUMBER;
    left = nullptr;
    right = nullptr;
    nodeId = NEXT_NODE_ID++;
    token = "";
}

ExprNode::ExprNode(ExprNodeType type, ExprNode* left, ExprNode* right)
{
    value = 0;
    this->type = type;
    this->left = left;
    this->right = right;
    nodeId = NEXT_NODE_ID++;
    token = "";
}

ExprNode::~ExprNode()
{
    // Если левый потомок существует, удалить левый потомок
    delete left;
    // Если правый потомок существует, удалить правый потомок
    delete right;
}

bool calculateVariableNode(ExprNode* currentNode, const map<string, double>& variables, vector<Error>& errors)
{
    // Найти значение переменной в таблице variables
    map<string, double>::const_iterator variableIterator = variables.find(currentNode->token);
    // Если переменная не найдена
    if (variableIterator == variables.end())
    {
        errors.push_back({ ErrorType::UNKNOWN_VARIABLE, -1, currentNode->token });
        return false;
    }
    // Записать найденное значение в поле value
    currentNode->value = variableIterator->second;
    return true;
}

bool calculateBinaryOperation(ExprNode* operationNode, double leftValue, double rightValue, vector<Error>& errors)
{
    // Выполнить сложение
    if (operationNode->type == ExprNodeType::ADD)
    {
        operationNode->value = leftValue + rightValue;
        return true;
    }
    // Выполнить вычитание
    if (operationNode->type == ExprNodeType::SUB)
    {
        operationNode->value = leftValue - rightValue;
        return true;
    }
    // Выполнить умножение
    if (operationNode->type == ExprNodeType::MUL)
    {
        operationNode->value = leftValue * rightValue;
        return true;
    }
    // Выполнить деление
    if (operationNode->type == ExprNodeType::DIV)
    {
        if (fabs(rightValue) < 1e-12)
        {
            errors.push_back({ ErrorType::DIVISION_BY_ZERO, -1, "" });
            return false;
        }
        operationNode->value = leftValue / rightValue;
        return true;
    }
    // Выполнить возведение в степень
    if (operationNode->type == ExprNodeType::POW)
    {
        operationNode->value = pow(leftValue, rightValue);
        return true;
    }
    // Если тип операции неизвестен
    errors.push_back({ ErrorType::INVALID_SYMBOL, -1, operationNode->token });
    return false;
}

double calculate(ExprNode* node, const map<string, double>& variables, vector<Error>& errors)
{
    // Если узел пуст
    if (node == nullptr)
    {
        return 0;
    }
    // Если узел имеет тип NUMBER
    if (node->type == ExprNodeType::NUMBER)
    {
        return node->value;
    }
    // Если узел имеет тип VARIABLE
    if (node->type == ExprNodeType::VARIABLE)
    {
        if (!calculateVariableNode(node, variables, errors))
        {
            return 0;
        }
        return node->value;
    }
    // Если узел имеет тип UNARY_MINUS
    if (node->type == ExprNodeType::UNARY_MINUS)
    {
        double value = calculate(node->left, variables, errors);
        if (!errors.empty())
        {
            return 0;
        }
        node->value = -value;
        return node->value;
    }
    // Рекурсивно вычислить значение левого поддерева
    double leftValue = calculate(node->left, variables, errors);
    if (!errors.empty())
    {
        return 0;
    }
    // Рекурсивно вычислить значение правого поддерева
    double rightValue = calculate(node->right, variables, errors);
    if (!errors.empty())
    {
        return 0;
    }
    // Выполнить бинарную операцию
    if (!calculateBinaryOperation(node, leftValue, rightValue, errors))
    {
        return 0;
    }
    return node->value;
}

int getPriority(const string& token) 
{
    // Если токен - унарный минус, вернуть наивысший приоритет
    if (token == "~") return 4;
    // Если токен - "^", вернуть приоритет степени
    if (token == "^") return 3;
    //  Если токен - "*" или "/", вернуть следующий приоритет
    if (token == "*" || token == "/") return 2;
    // Если токен - "+" или "-" , вернуть следующий приоритет
    if (token == "+" || token == "-") return 1;
    // Иначе вернуть 0
    return 0;
}

bool isValidVariableName(const string& name)
{
    // Если строка пуста, вернуть false
    if (name.empty())
    {
        return false;
    }
    // Проверить, что первый символ - латинская буква
    if ((unsigned char)name[0] >= 128 || !isalpha((unsigned char)name[0]))
    {
        return false;
    }
    // Проверить, что все остальные символы являются латинскими буквами или цифрами
    for (int i = 1; i < (int)name.size(); i++)
    {
        if ((unsigned char)name[i] >= 128 || !isalnum((unsigned char)name[i]))
        {
            // Если условие не выполнено, вернуть false
            return false;
        }
    }
    // Если все условия выполнены, вернуть true
    return true;
}

string trim(string s) 
{
    // Пока строка не пуста и первый символ является пробелом
    while (!s.empty() && isspace((unsigned char)s.front())) 
    {
        // Удалить первый символ строки
        s.erase(s.begin());
    }
    // Пока строка не пуста и последний символ является пробелом
    while (!s.empty() && isspace((unsigned char)s.back())) 
    {
        // Удалить последний символ строки
        s.pop_back();
    }
    // Вернуть строку без начальных и конечных пробелов
    return s;
}

string formatNumber(double value)
{
    // Если число является целым
    if (fabs(value - round(value)) < 1e-9)
    {
        // Вернуть его без дробной части
        return to_string((long long)round(value));
    }
    // Иначе округлить число до трёх знаков после точки и подготовить поток для преобразования числа в строку
    ostringstream out;
    // Округлить число до трёх знаков после точки
    out << fixed << setprecision(3) << value;
    // Преобразовать число в строку
    string result = out.str();
    // Удалить лишние нули в конце дробной части
    while (!result.empty() && result.back() == '0')
    {
        result.pop_back();
    }
    // Если после удаления нулей последним символом осталась точка, удалить её
    if (!result.empty() && result.back() == '.')
    {
        result.pop_back();
    }
    // Вернуть строковое представление числа
    return result;
}

bool processAssignments(const vector<string>& lines, map<string, double>& variables, vector<Error>& errors)
{
    // Если количество строк меньше 2
    if (lines.size() < 2)
    {
        // Вернуть true, так как присваивания могут отсутствовать
        return true;
    }
    // Для каждой строки, кроме последней
    for (int i = 0; i + 1 < (int)lines.size(); i++)
    {
        string line = lines[i];
        // Посчитать кол-во символов "="
        int countEquals = 0;
        int equalPosition = -1;
        for (int j = 0; j < (int)line.size(); j++)
        {
            if (line[j] == '=')
            {
                countEquals++;
                equalPosition = j;
            }
        }
        // Если символ "=" отсутствует или встречается более одного раза
        if (countEquals != 1)
        {
            // Добавить ошибку INVALID_ASSIGNMENT
            errors.push_back({ ErrorType::INVALID_ASSIGNMENT, -1, "" });
            // Перейти к следующей строке
            continue;
        }
        // Разделить строку на левую и правую части по символу "="
        string leftPart = line.substr(0, equalPosition);
        string rightPart = line.substr(equalPosition + 1);
        // Удалить лишние пробелы в левой и правой частях функцией trim
        string left = trim(leftPart);
        string right = trim(rightPart);
        // Если левая часть пуста или правая часть пуста
        if (left.empty() || right.empty())
        {
            // Добавить ошибку INVALID_ASSIGNMENT
            errors.push_back({ ErrorType::INVALID_ASSIGNMENT, -1, "" });
            // Перейти к следующей строке
            continue;
        }
        // Проверить корректность имени переменной функцией isValidVariableName
        // Если имя переменной некорректно
        if (!isValidVariableName(left))
        {
            // Добавить ошибку INVALID_VARIABLE_NAME
            errors.push_back({ ErrorType::INVALID_VARIABLE_NAME, -1, left });
            // Перейти к следующей строке
            continue;
        }
        // Создать отдельный вектор ошибок для правой части присваивания
        vector<Error> localErrors;
        // Обнулить счётчик узлов перед построением временного дерева
        NEXT_NODE_ID = 0;
        // Распарсить правую часть присваивания функцией parseExpression
        ExprNode* root = parseExpression(right, localErrors);
        // Если при парсинге возникли ошибки
        if (!localErrors.empty() || root == nullptr)
        {
            // Перенести найденные ошибки в общий список ошибок
            for (Error error : localErrors)
            {
                errors.push_back(error);
            }
            // Если дерево выражения было создано, удалить его корень
            delete root;
            // Перейти к следующей строке
            continue;
        }
        // Вычислить значение выражения функцией calculate
        double value = calculate(root, variables, localErrors);
        // Если при вычислении возникли ошибки
        if (!localErrors.empty())
        {
            // Перенести найденные ошибки в общий список ошибок
            for (Error error : localErrors)
            {
                errors.push_back(error);
            }
            // Удалить дерево
            delete root;
            // Перейти к следующей строке
            continue;
        }
        // Сохранить полученное значение в таблицу variables по имени переменной. Если переменная уже существует, перезаписать её значение
        variables[left] = value;
        // Удалить корень дерева выражения
        delete root;
    }
    // Если вектор ошибок пуст, вернуть true, иначе вернуть false
    return errors.empty();
}

void writeGraph(ExprNode* root, ofstream& out)
{
    // Обнулить номер действия перед генерацией дерева
    NEXT_ACTION_ID = 1;
    // Записать строку digraph {
    out << "digraph {\n";
    // Сгенерировать дерево функцией generateGraph.
    generateGraph(root, out);
    // Записать строку }
    out << "}\n";
}

void generateGraph(ExprNode* node, ofstream& out)
{
    // Если текущий узел пуст:
    if (node == nullptr)
    {
        // Завершить работу функции
        return;
    }
    // Если существует левый потомок:
    if (node->left != nullptr)
    {
        // Рекурсивно вызвать generateGraph для левого потомка
        generateGraph(node->left, out);
    }
    // Если существует правый потомок:
    if (node->right != nullptr)
    {
        // Рекурсивно вызвать generateGraph для правого потомка
        generateGraph(node->right, out);
    }
    // Сформировать строковое представление текущего узла
    string label;
    // Если узел имеет тип NUMBER:
    if (node->type == ExprNodeType::NUMBER)
    {
        // Использовать числовое значение узла как подпись
        label = formatNumber(node->value);
    }
    // Если узел имеет тип VARIABLE:
    else if (node->type == ExprNodeType::VARIABLE)
    {
        // Записать в качестве подписи узла имя переменной
        label = node->token;
    }
    // Если узел имеет тип UNARY_MINUS:
    else if (node->type == ExprNodeType::UNARY_MINUS)
    {
        // Считать унарный минус отдельной операцией
        // Подпись формируется в формате <номер_действия>: <операция>: <результат>.
        label = to_string(NEXT_ACTION_ID) + ": -: " + formatNumber(node->value);
        // Перейти к следующему номеру действия
        NEXT_ACTION_ID++;
    }
    // Если узел является бинарным операторным узлом:
    else
    {
        // Сформировать подпись в формате <номер_действия>: <операция>: <результат>.
        label = to_string(NEXT_ACTION_ID) + ": " + node->token + ": " + formatNumber(node->value);
        // Перейти к следующему номеру действия.
        NEXT_ACTION_ID++;
    }
    // Записать в файл строку описания узла в формате <номер_узла> [label="<строковое_представление_узла>"];
    out << node->nodeId << " [label=\"" << label << "\"];\n";
    // Если существует левый потомок:
    if (node->left != nullptr)
    {
        // Записать связь <номер_левого_потомка> -> <номер_текущего_узла>;
        out << node->left->nodeId << " -> " << node->nodeId << ";\n";
    }
    // Если существует правый потомок:
    if (node->right != nullptr)
    {
        // Записать связь <номер_правого_потомка> -> <номер_текущего_узла>;
        out << node->right->nodeId << " -> " << node->nodeId << ";\n";
    }
}

vector<string> readFile(const string& filename, vector<Error>& errors)
{
    // Попытаться открыть входной файл
    ifstream in(filename);
    // Если файл не удалось открыть
    if (!in)
    {
        // Добавить ошибку INVALID_INPUT_FILE
        errors.push_back({ ErrorType::INVALID_INPUT_FILE, -1, "" });
        // Вернуть пустой вектор
        return {};
    }
    // Создать пустой вектор строк
    vector<string> lines;
    string line;
    // Считывать файл построчно до конца
    while (getline(in, line))
    {
        // Каждую считанную строку добавить в вектор
        lines.push_back(line);
    }
    // Закрыть файл
    in.close();
    // Вернуть вектор строк
    return lines;
}

string getErrorMessage(ErrorType errorType)
{
    // Создать таблицу сообщений об ошибках
    map<ErrorType, string> errorMessages =
    {
        { ErrorType::INVALID_INPUT_FILE, "Invalid input file. The file may not exist." },
        { ErrorType::INVALID_OUTPUT_FILE, "Invalid output file. The path may not exist or access is denied." },
        { ErrorType::EMPTY_INPUT_FILE, "Error: input file is empty." },
        { ErrorType::TOO_MANY_LINES, "Error: too many lines in the input file. Maximum allowed number of lines is 100." },
        { ErrorType::LINE_TOO_LONG, "Error: line length exceeds 1000 characters." },
        { ErrorType::INVALID_SYMBOL, "Error: invalid symbol." },
        { ErrorType::INVALID_NUMBER_FORMAT, "Error: invalid number format." },
        { ErrorType::NUMBER_OUT_OF_RANGE, "Error: number is out of range [0..2147483647]." },
        { ErrorType::MISMATCHED_BRACKETS, "Error: mismatched brackets." },
        { ErrorType::MISSING_OPERAND, "Error: missing operand." },
        { ErrorType::MISSING_OPERATOR, "Error: missing operator between operands." },
        { ErrorType::TOO_MANY_OPERATIONS, "Error: too many operations. Maximum allowed number of operations is 100." },
        { ErrorType::DIVISION_BY_ZERO, "Error: division by zero." },
        { ErrorType::UNKNOWN_VARIABLE, "Error: unknown variable." },
        { ErrorType::INVALID_ASSIGNMENT, "Error: invalid assignment format." },
        { ErrorType::INVALID_VARIABLE_NAME, "Error: invalid variable name." },
        { ErrorType::MISSING_FINAL_EXPRESSION, "Error: missing final expression." }
    };
    // Найти сообщение по типу ошибки
    map<ErrorType, string>::iterator messageIterator = errorMessages.find(errorType);
    // Если сообщение найдено, вернуть его
    if (messageIterator != errorMessages.end())
    {
        return messageIterator->second;
    }
    // Если тип ошибки неизвестен, вернуть сообщение по умолчанию
    return "Unknown error.";
}

void printError(const Error& error)
{
    // Вывести основное сообщение об ошибке
    cout << getErrorMessage(error.type) << "\n";
    // Если позиция ошибки известна, вывести ее
    if (error.position != -1)
    {
        cout << "Error position: " << error.position << "\n";
    }
    // Если есть проблемный токен, вывести его
    if (!error.token.empty())
    {
        cout << "Problem token: " << error.token << "\n";
    }
    cout << "\n";
}

void printAllErrors(const vector<Error>& errors)
{
    // Для каждой ошибки
    for (Error error : errors)
    {
        // вывести сообщение ошибки
        printError(error);
    }
}

bool validateInputLines(const vector<string>& lines, vector<Error>& errors)
{
    // Проверить, что входной файл не пуст
    if (lines.empty())
    {
        errors.push_back({ ErrorType::EMPTY_INPUT_FILE, -1, "" });
    }
    // Проверить, что количество строк не превышает допустимое значение
    if ((int)lines.size() > 100)
    {
        errors.push_back({ ErrorType::TOO_MANY_LINES, -1, "" });
    }
    // Проверить длину каждой строки входного файла
    for (string line : lines)
    {
        if ((int)line.size() > 1000)
        {
            errors.push_back({ ErrorType::LINE_TOO_LONG, -1, "" });
            break;
        }
    }
    // Проверить наличие итогового арифметического выражения
    if (lines.empty() || trim(lines.back()).empty())
    {
        errors.push_back({ ErrorType::MISSING_FINAL_EXPRESSION, -1, "" });
    }
    // вернуть true если нет ошибок
    return errors.empty();
}

bool saveGraphToFile(const string& filename, ExprNode* root, vector<Error>& errors)
{
    // Открыть выходной файл для записи
    ofstream out(filename);
    // Если выходной файл невозможно создать или открыть
    if (!out)
    {
        errors.push_back({ ErrorType::INVALID_OUTPUT_FILE, -1, "" });
        return false;
    }
    // Записать дерево выражения в выходной файл
    writeGraph(root, out);
    // вернуть true при успешной записи
    return true;
}

int main(int argc, char* argv[])
{
    // Проверить количество переданных аргументов командной строки
    if (argc != 3)
    {
        cout << "Использование: program input.txt output.txt\n";
        return 1;
    }
    // Создать пустой список ошибок
    vector<Error> errors;
    // Считать входной файл
    vector<string> lines = readFile(argv[1], errors);
    // Если при чтении файла возникли ошибки
    if (!errors.empty())
    {
        printAllErrors(errors);
        return 1;
    }
    // Проверить корректность входных строк
    if (!validateInputLines(lines, errors))
    {
        printAllErrors(errors);
        return 1;
    }
    // Создать таблицу значений переменных
    map<string, double> variables;
    // Обработать строки присваивания переменных
    if (!processAssignments(lines, variables, errors))
    {
        printAllErrors(errors);
        return 1;
    }
    // Обнулить счётчик номеров узлов перед построением дерева итогового выражения
    NEXT_NODE_ID = 0;
    // Распарсить последнюю строку входного файла как итоговое арифметическое выражение
    ExprNode* root = parseExpression(lines.back(), errors);
    // Если при парсинге возникли ошибки или дерево не было построено
    if (!errors.empty() || root == nullptr)
    {
        printAllErrors(errors);
        delete root;
        return 1;
    }
    // Вычислить значение дерева выражения
    calculate(root, variables, errors);
    // Если при вычислении возникли ошибки
    if (!errors.empty())
    {
        printAllErrors(errors);
        delete root;
        return 1;
    }
    // Сохранить дерево выражения в выходной файл
    if (!saveGraphToFile(argv[2], root, errors))
    {
        printAllErrors(errors);
        delete root;
        return 1;
    }
    // Удалить корень дерева выражения и освободить память
    delete root;
    return 0;
}