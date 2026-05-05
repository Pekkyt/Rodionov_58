#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

using std::string;
using std::vector;
using std::map;
using std::ofstream;

/*
 * Тип узла дерева выражения.
 */
enum class ExprNodeType
{
    NUMBER,
    VARIABLE,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    UNARY_MINUS
};

/*
 * Тип ошибки, возникающей при чтении файла, разборе выражения,
 * построении дерева или вычислении результата.
 */
enum class ErrorType
{
    NO_ERROR,
    INVALID_INPUT_FILE,
    INVALID_OUTPUT_FILE,
    EMPTY_INPUT_FILE,
    TOO_MANY_LINES,
    LINE_TOO_LONG,
    INVALID_SYMBOL,
    INVALID_NUMBER_FORMAT,
    NUMBER_OUT_OF_RANGE,
    MISMATCHED_BRACKETS,
    MISSING_OPERAND,
    MISSING_OPERATOR,
    TOO_MANY_OPERATIONS,
    DIVISION_BY_ZERO,
    UNKNOWN_VARIABLE,
    INVALID_ASSIGNMENT,
    INVALID_VARIABLE_NAME,
    MISSING_FINAL_EXPRESSION
};

/*
 * Структура для хранения информации об ошибке.
 *
 * Поля:
 * type     - тип ошибки.
 * position - позиция ошибки в исходной строке.
 * token    - проблемный элемент.
 */
struct Error
{
    ErrorType type;
    int position;
    string token;
};

/*
 * Тип токена арифметического выражения.
 */
enum class TokenType
{
    NUMBER,
    VARIABLE,
    OPERATOR,
    LEFT_BRACKET,
    RIGHT_BRACKET
};

/*
 * Структура для хранения токена выражения.
 *
 * Поля:
 * text     - текст токена.
 * position - позиция токена в исходной строке.
 * type     - тип токена.
 */
struct Token
{
    string text;
    int position;
    TokenType type;
};

/*
 * Класс узла дерева арифметического выражения.
 *
 * Поля:
 * value  - значение узла.
 * type   - тип узла.
 * left   - левый потомок.
 * right  - правый потомок.
 * nodeId - уникальный номер узла.
 * token  - строковое представление узла.
 */
class ExprNode
{
public:
    double value;
    ExprNodeType type;
    ExprNode* left;
    ExprNode* right;
    int nodeId;
    string token;

    /*
     * @brief Вычисляет значение поддерева, корнем которого является текущий узел.
     * @return double - результат вычисления поддерева.
     */
    double calculate();

    /*
     * @brief Создает узел числового значения.
     * @param[in] val - значение числа.
     */
    ExprNode(double val);

    /*
     * @brief Создает узел операции.
     * @param[in] type - тип операции.
     * @param[in] left - левый потомок.
     * @param[in] right - правый потомок.
     */
    ExprNode(ExprNodeType type, ExprNode* left, ExprNode* right);

    /*
     * @brief Удаляет узел и рекурсивно освобождает память дочерних узлов.
     */
    ~ExprNode();
};

/*
 * @brief Главная функция программы.
 * @param[in] argc - количество аргументов командной строки.
 * @param[in] argv - массив аргументов командной строки.
 * @return int - код завершения программы: 0 при успехе, 1 при ошибке.
 */
int main(int argc, char* argv[]);

/*
 * @brief Считывает строки из входного файла.
 * @param[in] filename - путь к входному файлу.
 * @param[out] errors - вектор для хранения ошибок.
 * @return vector<string> - список строк входного файла.
 */
vector<string> readFile(const string& filename, vector<Error>& errors);

/*
 * @brief Обрабатывает строки присваивания переменных.
 * @param[in] lines - строки входного файла.
 * @param[out] variables - таблица переменных.
 * @param[out] errors - вектор для хранения ошибок.
 * @return bool - true, если присваивания обработаны успешно; false, если возникли ошибки.
 */
bool processAssignments(const vector<string>& lines,
    map<string, double>& variables,
    vector<Error>& errors);

/*
 * @brief Парсит арифметическое выражение и строит дерево выражения.
 * @param[in] expression - строка арифметического выражения.
 * @param[out] errors - вектор для хранения ошибок.
 * @return ExprNode* - корень дерева выражения или nullptr при ошибке.
 */
ExprNode* parseExpression(const string& expression, vector<Error>& errors);

/*
 * @brief Разбивает арифметическое выражение на токены с сохранением их позиций.
 * @param[in] expression - строка выражения.
 * @param[out] errors - вектор для хранения ошибок.
 * @return vector<Token> - список токенов.
 */
vector<Token> tokenize(const string& expression, vector<Error>& errors);

/*
 * @brief Преобразует список токенов из инфиксной формы в постфиксную форму.
 * @param[in] tokens - список токенов инфиксного выражения.
 * @param[out] errors - вектор для хранения ошибок.
 * @return vector<Token> - список токенов в постфиксной форме.
 */
vector<Token> toPostfix(const vector<Token>& tokens, vector<Error>& errors);

/*
 * @brief Вычисляет значение дерева арифметического выражения.
 * @param[in] node - корень дерева выражения.
 * @param[in] variables - таблица значений переменных.
 * @param[out] errors - вектор для хранения ошибок.
 * @return double - результат вычисления выражения.
 */
double calculate(ExprNode* node,
    const map<string, double>& variables,
    vector<Error>& errors);

/*
 * @brief Строит дерево арифметического выражения по постфиксной записи.
 * @param[in] postfix - выражение в постфиксной форме.
 * @param[out] errors - вектор для хранения ошибок.
 * @return ExprNode* - корень дерева выражения или nullptr при ошибке.
 */
ExprNode* buildTree(const vector<Token>& postfix, vector<Error>& errors);

/*
 * @brief Записывает дерево выражения в файл в формате DOT.
 * @param[in] root - корень дерева выражения.
 * @param[out] out - выходной файловый поток.
 */
void writeGraph(ExprNode* root, ofstream& out);

/*
 * @brief Генерирует описание узлов и связей дерева в формате DOT.
 * @param[in] node - текущий узел дерева.
 * @param[out] out - выходной файловый поток.
 */
void generateGraph(ExprNode* node, ofstream& out);

/*
 * @brief Проверяет корректность имени переменной.
 * @param[in] name - имя переменной.
 * @return bool - true, если имя корректно; false, если имя некорректно.
 */
bool isValidVariableName(const string& name);

/*
 * @brief Возвращает приоритет операции.
 * @param[in] token - строковое представление операции.
 * @return int - целочисленный приоритет операции.
 */
int getPriority(const string& token);

/*
 * @brief Форматирует число для вывода.
 * @param[in] value - число для вывода.
 * @return string - строковое представление числа.
 */
string formatNumber(double value);

/*
 * @brief Выводит сообщение об ошибке.
 * @param[in] error - структура ошибки.
 */
void printError(const Error& error);