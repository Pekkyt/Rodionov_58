#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <stack>
#include <cctype>
#include "Header.h"
using namespace std;
static int NEXT_NODE_ID = 0;

vector<Token> tokenize(const string& s, vector<Error>& errors) 
{
    // Создать пустой список токенов
    vector<Token> t;
    // Установить индекс текущего символа в начало строки
    int i = 0;
    // Пока индекс меньше длины строки
    while (i < (int)s.size()) {

        // Если текущий символ - пробел
        if (isspace((unsigned char)s[i])) {

            // Перейти к следующему символу
            i++;
            continue;
        }
        // Если текущий символ - цифра
        if (isdigit((unsigned char)s[i]))
        {

            // Запомнить позицию начала числа
            int st = i;
            // Считать целую часть числа
            while (i < (int)s.size() && isdigit((unsigned char)s[i])) { i++; }
            // Если после целой части встречена точка
            if (i < (int)s.size() && s[i] == '.')
            {

                // Добавить точку в число
                i++;
                // Проверить, что после точки следует хотя бы одна цифра
                if (i >= (int)s.size() || !isdigit((unsigned char)s[i]))
                {
                    // Считать некорректное число целиком
                    while (i < (int)s.size() && (isdigit((unsigned char)s[i]) || s[i] == '.' || s[i] == ',')) { i++; }
                    // Если формат числа некорректен, добавить ошибку INVALID_NUMBER_FORMAT
                    errors.push_back({ ErrorType::INVALID_NUMBER_FORMAT, st, s.substr(st, i - st) });
                    continue;
                }
                // Считать дробную часть
                while (i < (int)s.size() && isdigit((unsigned char)s[i])) { i++; }
            }
            // Если после числа снова встретилась точка или запятая, формат числа некорректен
            if (i < (int)s.size() && (s[i] == '.' || s[i] == ','))
            {
                // Считать некорректное число целиком
                while (i < (int)s.size() && (isdigit((unsigned char)s[i]) || s[i] == '.' || s[i] == ',')) { i++; }
                // Если формат числа некорректен, добавить ошибку INVALID_NUMBER_FORMAT
                errors.push_back({ ErrorType::INVALID_NUMBER_FORMAT, st, s.substr(st, i - st) });
                continue;
            }
            // Получить строковое представление числа
            string num = s.substr(st, i - st);
            // Проверить, что модуль числа не превышает 2147483647
            double value = stod(num);
            // Если число выходит за диапазон, добавить ошибку NUMBER_OUT_OF_RANGE
            if (fabs(value) > 2147483647.0)
            {
                errors.push_back({ ErrorType::NUMBER_OUT_OF_RANGE, st, num });
                continue;
            }
            // Добавить токен числа в список.
            t.push_back({ num, st, TokenType::NUMBER });
            continue;
        }
        // Если число начинается с точки или запятой, формат числа некорректен
        if (s[i] == '.' || s[i] == ',')
        {
            // Запомнить позицию начала некорректного числа
            int st = i;
            // Считать некорректное число целиком
            while (i < (int)s.size() && (isdigit((unsigned char)s[i]) || s[i] == '.' || s[i] == ',')) { i++; }
            // Добавить ошибку INVALID_NUMBER_FORMAT
            errors.push_back({ ErrorType::INVALID_NUMBER_FORMAT, st, s.substr(st, i - st) });
            continue;
        }
        // Если текущий символ - латинская буква
        if ((unsigned char)s[i] < 128 && isalpha((unsigned char)s[i]))
        {
            // Запомнить позицию начала имени переменной
            int st = i;
            // Считать имя переменной целиком
            while (i < (int)s.size() && (unsigned char)s[i] < 128 && isalnum((unsigned char)s[i])) { i++; }
            // Добавить токен переменной в список
            t.push_back({ s.substr(st, i - st), st, TokenType::VARIABLE });
            continue;
        }
        // Если текущий символ - один из символов +, -, *, /, ^, (, )
        if (string("+-*/^()").find(s[i]) != string::npos)
        {
            // Сохранить текст текущего символа
            string cur(1, s[i]);
            // По умолчанию считать токен оператором
            TokenType type = TokenType::OPERATOR;
            // Если символ является открывающей скобкой, задать тип LEFT_BRACKET
            if (s[i] == '(') { type = TokenType::LEFT_BRACKET; }
            // Если символ является закрывающей скобкой, задать тип RIGHT_BRACKET
            else if (s[i] == ')') { type = TokenType::RIGHT_BRACKET; }
            // Если символ является минусом, определить: унарный он или бинарный
            else if (s[i] == '-')
            {
                // Если "-" находится в начале выражения, после "(" или после другого оператора, считать его унарным минусом
                bool unary = t.empty() || t.back().type == TokenType::LEFT_BRACKET || t.back().type == TokenType::OPERATOR;
                // Иначе считать символ "-" бинарной операцией вычитания
                cur = unary ? "~" : "-";
            }
            // Добавить соответствующий токен в список.
            t.push_back({ cur, i, type });
            // Перейти к следующему символу.
            i++;
            continue;
        }
        // Если текущий символ не относится ни к одному допустимому случаю
        // Добавить ошибку INVALID_SYMBOL
        errors.push_back({ ErrorType::INVALID_SYMBOL, i, string(1, s[i]) });
        // Перейти к следующему символу
        i++;
    }
    // Если при разборе символов уже были ошибки, дополнительные проверки не выполнять
    if (!errors.empty()) { return t; }
    // После завершения прохода проверить токены
    for (int j = 0; j < (int)t.size(); j++)
    {
        // Определить, является ли текущий токен бинарным оператором
        bool currentIsBinaryOperator = t[j].type == TokenType::OPERATOR && t[j].text != "~";
        // Если бинарный оператор стоит в начале выражения
        if (j == 0 && currentIsBinaryOperator)
        {
            errors.push_back({ ErrorType::MISSING_OPERAND, t[j].position, t[j].text });
        }
        // Если бинарный оператор стоит в конце выражения
        if (j == (int)t.size() - 1 && currentIsBinaryOperator)
        {
            errors.push_back({ ErrorType::MISSING_OPERAND, t[j].position, t[j].text });
        }
        // Для проверки соседних токенов нужен предыдущий токен
        if (j == 0) {
            continue;
        }
        // Определить, является ли предыдущий токен операндом или закрывающей скобкой
        bool previousIsOperand = t[j - 1].type == TokenType::NUMBER || t[j - 1].type == TokenType::VARIABLE || t[j - 1].type == TokenType::RIGHT_BRACKET;
        // Определить, является ли текущий токен операндом или открывающей скобкой
        bool currentIsOperandOrLeftBracket = t[j].type == TokenType::NUMBER || t[j].type == TokenType::VARIABLE || t[j].type == TokenType::LEFT_BRACKET;
        // Если подряд идут два операнда, добавить ошибку MISSING_OPERATOR
        // Если после операнда сразу идёт открывающая скобка, добавить ошибку MISSING_OPERATOR
        // Если после закрывающей скобки сразу идёт операнд, добавить ошибку MISSING_OPERATOR
        if (previousIsOperand && currentIsOperandOrLeftBracket)
        {
            errors.push_back({ ErrorType::MISSING_OPERATOR, t[j].position, t[j].text });
        }
        // Определить, является ли предыдущий токен бинарным оператором
        bool previousIsBinaryOperator = t[j - 1].type == TokenType::OPERATOR && t[j - 1].text != "~";
        // Если подряд идут два бинарных оператора, добавить ошибку MISSING_OPERAND
        if (previousIsBinaryOperator && currentIsBinaryOperator)
        {
            errors.push_back({ ErrorType::MISSING_OPERAND, t[j].position, t[j].text });
        }
        // Если после открывающей скобки идёт бинарный оператор
        if (t[j - 1].type == TokenType::LEFT_BRACKET && currentIsBinaryOperator)
        {
            errors.push_back({ ErrorType::MISSING_OPERAND, t[j].position, t[j].text });
        }
    }
    // Вернуть список токенов
    return t;
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
            current->type == ExprNodeType::POW)
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

double calculate(ExprNode* node, const map<string, double>& variables, vector<Error>& errors)
{
    // Если узел пуст
    if (node == nullptr)
    {
        // Вернуть 0
        return 0;
    }
    // Если узел имеет тип NUMBER
    if (node->type == ExprNodeType::NUMBER)
    {
        // Вернуть его значение
        return node->value;
    }
    // Если узел имеет тип VARIABLE
    if (node->type == ExprNodeType::VARIABLE)
    {
        // Найти значение переменной в таблице variables
        auto it = variables.find(node->token);
        // Если переменная не найдена
        if (it == variables.end())
        {
            // Считать вычисление выражения завершившимся с ошибкой UNKNOWN_VARIABLE
            errors.push_back({ ErrorType::UNKNOWN_VARIABLE, -1, node->token });
            //  Немедленно завершить вычисление
            return 0;
        }
        // Записать найденное значение в поле value
        node->value = it->second;
        // Вернуть найденное значение.
        return node->value;
    }
    // Если узел имеет тип UNARY_MINUS
    if (node->type == ExprNodeType::UNARY_MINUS)
    {
        // Рекурсивно вычислить значение дочернего узла
        double value = calculate(node->left, variables, errors);
        // Если при вычислении дочернего узла возникла ошибка, немедленно завершить вычисление
        if (!errors.empty()) return 0;
        // Записать в поле value результат с противоположным знаком
        node->value = -value;
        // Вернуть value
        return node->value;
    }
    // Если узел имеет тип бинарной операции
    // Рекурсивно вычислить значение левого поддерева
    double leftValue = calculate(node->left, variables, errors);
    // Если при вычислении левого поддерева возникла ошибка, немедленно завершить вычисление
    if (!errors.empty()) return 0;
    // Рекурсивно вычислить значение правого поддерева.
    double rightValue = calculate(node->right, variables, errors);
    // Если при вычислении правого поддерева возникла ошибка, немедленно завершить вычисление
    if (!errors.empty()) return 0;
    // В зависимости от типа узла выполнить операцию
    if (node->type == ExprNodeType::ADD)
    {
        // Для ADD выполнить сложение
        node->value = leftValue + rightValue;
    }
    else if (node->type == ExprNodeType::SUB)
    {
        // Для SUB выполнить вычитание
        node->value = leftValue - rightValue;
    }
    else if (node->type == ExprNodeType::MUL)
    {
        // Для MUL выполнить умножение
        node->value = leftValue * rightValue;
    }
    else if (node->type == ExprNodeType::DIV)
    {
        // Перед делением проверить, что правый операнд не равен нулю
        if (fabs(rightValue) < 1e-12)
        {
            // Если происходит деление на ноль
            // Считать вычисление выражения завершившимся с ошибкой DIVISION_BY_ZERO
            errors.push_back({ ErrorType::DIVISION_BY_ZERO, -1, "" });
            // Немедленно завершить вычисление
            return 0;
        }
        // Для DIV выполнить деление
        node->value = leftValue / rightValue;
    }
    else if (node->type == ExprNodeType::POW)
    {
        // Для POW выполнить возведение в степень
        node->value = pow(leftValue, rightValue);
    }
    // Вернуть результат
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

int main(int argc, char* argv[])
{
    return 0;
}
