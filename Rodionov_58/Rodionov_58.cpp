#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <cctype>
#include "Header.h"
using namespace std;

vector<Token> tokenize(const string& s, vector<Error>& errors) {
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

vector<Token> toPostfix(const vector<Token>& tokens, vector<Error>& errors)
{
    return {};
}

ExprNode* buildTree(const vector<Token>& postfix, vector<Error>& errors)
{
    return nullptr;
}

ExprNode* parseExpression(const string& expression, vector<Error>& errors)
{
    bool onlySpaces = true;
    for (char ch : expression)
    {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r')
        {
            onlySpaces = false;
            break;
        }
    }
    if (expression.empty() || onlySpaces)
    {
        errors.push_back(Error{ ErrorType::MISSING_OPERATOR, -1, "" });
        return nullptr;
    }
    return nullptr;
}

ExprNode::ExprNode(double val)
{
    value = val;
    type = ExprNodeType::NUMBER;
    left = nullptr;
    right = nullptr;
    nodeId = 0;
    token = "";
}

ExprNode::ExprNode(ExprNodeType type, ExprNode* left, ExprNode* right)
{
    value = 0;
    this->type = type;
    this->left = left;
    this->right = right;
    nodeId = 0;
    token = "";
}

ExprNode::~ExprNode()
{
    delete left;
    delete right;
}

double calculate(ExprNode* node, const map<string, double>& variables, vector<Error>& errors)
{
    if (node == nullptr)
    {
        return 0;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    return 0;
}
