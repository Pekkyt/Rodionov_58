#pragma once

/**
 * @file Header.h
 * @brief Заголовочный файл программы расчета математического выражения.
 *
 * Файл содержит перечисления, структуры, класс узла дерева выражения
 * и объявления функций, используемых для чтения входного файла,
 * обработки присваиваний, разбора арифметического выражения,
 * построения дерева, вычисления результата и вывода дерева в формате DOT.
 */

 /**
  * @mainpage Документация программы расчета математического выражения
  *
  * @section intro_sec Назначение программы
  * Программа предназначена для вычисления арифметических выражений,
  * записанных в инфиксной форме, с поддержкой переменных, операций
  * присваивания и вывода дерева выражения в формате Graphviz DOT.
  *
  * @section input_sec Входные данные
  * Программа получает два аргумента командной строки:
  * - имя входного текстового файла;
  * - имя выходного текстового файла.
  *
  * Все строки входного файла, кроме последней, являются присваиваниями
  * переменных. Последняя строка содержит итоговое арифметическое выражение.
  *
  * @section output_sec Выходные данные
  * В выходной файл записывается дерево выражения в формате DOT.
  */

  /**
   * @defgroup functions Functions
   * @brief Functions of the mathematical expression calculation program.
   */

#include <string>
#include <vector>
#include <map>
#include <fstream>

using std::string;
using std::vector;
using std::map;
using std::ofstream;

/**
 * @enum ExprNodeType
 * @brief Тип узла дерева арифметического выражения.
 */
enum class ExprNodeType
{
    NUMBER,       /**< Числовой литерал. */
    VARIABLE,     /**< Переменная. */
    ADD,          /**< Операция сложения. */
    SUB,          /**< Операция вычитания. */
    MUL,          /**< Операция умножения. */
    DIV,          /**< Операция деления. */
    POW,          /**< Операция возведения в степень. */
    UNARY_MINUS   /**< Операция унарного минуса. */
};

/**
 * @enum ErrorType
 * @brief Тип ошибки, возникающей во время работы программы.
 */
enum class ErrorType
{
    NO_ERROR,                  /**< Ошибка отсутствует. */
    INVALID_INPUT_FILE,         /**< Неверно указан входной файл. */
    INVALID_OUTPUT_FILE,        /**< Невозможно создать или открыть выходной файл. */
    EMPTY_INPUT_FILE,           /**< Входной файл пуст. */
    TOO_MANY_LINES,             /**< Количество строк во входном файле превышает допустимое значение. */
    LINE_TOO_LONG,              /**< Длина строки превышает допустимое значение. */
    INVALID_SYMBOL,             /**< Обнаружен недопустимый символ. */
    INVALID_NUMBER_FORMAT,      /**< Некорректный формат записи числа. */
    NUMBER_OUT_OF_RANGE,        /**< Число выходит за допустимый диапазон. */
    MISMATCHED_BRACKETS,        /**< Некорректная расстановка скобок. */
    MISSING_OPERAND,            /**< Отсутствует операнд. */
    MISSING_OPERATOR,           /**< Отсутствует оператор между операндами. */
    TOO_MANY_OPERATIONS,        /**< Количество операций превышает допустимое значение. */
    DIVISION_BY_ZERO,           /**< Деление на ноль. */
    UNKNOWN_VARIABLE,           /**< Используется необъявленная переменная. */
    INVALID_ASSIGNMENT,         /**< Некорректный формат присваивания переменной. */
    INVALID_VARIABLE_NAME,      /**< Некорректное имя переменной. */
    MISSING_FINAL_EXPRESSION    /**< Отсутствует итоговое арифметическое выражение. */
};

/**
 * @struct Error
 * @brief Хранит информацию об ошибке.
 */
struct Error
{
    ErrorType type;  /**< Тип ошибки. */
    int position;    /**< Позиция ошибки в исходной строке. Если позиция не определена, содержит -1. */
    string token;    /**< Проблемный элемент, из-за которого возникла ошибка. */
};

/**
 * @enum TokenType
 * @brief Тип токена арифметического выражения.
 */
enum class TokenType
{
    NUMBER,         /**< Число. */
    VARIABLE,       /**< Переменная. */
    OPERATOR,       /**< Оператор. */
    LEFT_BRACKET,   /**< Открывающая скобка. */
    RIGHT_BRACKET   /**< Закрывающая скобка. */
};

/**
 * @struct Token
 * @brief Хранит информацию о токене арифметического выражения.
 */
struct Token
{
    string text;      /**< Текстовое представление токена. */
    int position;     /**< Позиция токена в исходной строке. */
    TokenType type;   /**< Тип токена. */
};

/**
 * @class ExprNode
 * @brief Узел дерева арифметического выражения.
 *
 * Узел может представлять число, переменную, бинарную операцию
 * или унарный минус. Каждый узел хранит вычисленное значение,
 * тип, строковое представление и указатели на дочерние узлы.
 */
class ExprNode
{
public:
    double value;        /**< Значение узла. */
    ExprNodeType type;   /**< Тип узла. */
    ExprNode* left;      /**< Левый потомок. */
    ExprNode* right;     /**< Правый потомок. */
    int nodeId;          /**< Уникальный номер узла. */
    string token;        /**< Строковое представление узла. */

    /**
     * @brief Создает узел числового значения.
     * @param[in] val Значение числа.
     */
    ExprNode(double val);

    /**
     * @brief Создает узел операции.
     * @param[in] type Тип операции.
     * @param[in] left Левый потомок.
     * @param[in] right Правый потомок.
     */
    ExprNode(ExprNodeType type, ExprNode* left, ExprNode* right);

    /**
     * @brief Удаляет узел и рекурсивно освобождает память дочерних узлов.
     */
    ~ExprNode();
};


/**
 * @ingroup functions
 * @brief Главная функция программы.
 * @param[in] argc Количество аргументов командной строки.
 * @param[in] argv Массив аргументов командной строки.
 * @return Код завершения программы: 0 при успешном выполнении, 1 при ошибке.
 */
int main(int argc, char* argv[]);

/**
 * @ingroup functions
 * @brief Считывает строки из входного файла.
 * @param[in] filename Путь к входному файлу.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return Список строк входного файла.
 */
vector<string> readFile(const string& filename, vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Обрабатывает строки присваивания переменных.
 *
 * Функция обрабатывает все строки входного файла, кроме последней.
 * Каждая такая строка должна иметь формат присваивания переменной.
 *
 * @param[in] lines Строки входного файла.
 * @param[out] variables Таблица значений переменных.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return true, если присваивания обработаны успешно; false, если возникли ошибки.
 */
bool processAssignments(const vector<string>& lines,
    map<string, double>& variables,
    vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Парсит арифметическое выражение и строит дерево выражения.
 *
 * Функция выполняет токенизацию, преобразование выражения в постфиксную форму
 * и построение дерева арифметического выражения.
 *
 * @param[in] expression Строка арифметического выражения.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return Указатель на корень дерева выражения или nullptr при ошибке.
 */
ExprNode* parseExpression(const string& expression, vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Разбивает арифметическое выражение на токены.
 *
 * Функция сохраняет текст каждого токена, его тип и позицию в исходной строке.
 *
 * @param[in] expression Строка арифметического выражения.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return Список токенов.
 */
vector<Token> tokenize(const string& expression, vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Преобразует список токенов из инфиксной формы в постфиксную форму.
 *
 * Преобразование выполняется с учетом приоритетов операций,
 * скобок и унарного минуса.
 *
 * @param[in] tokens Список токенов инфиксного выражения.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return Список токенов в постфиксной форме.
 */
vector<Token> toPostfix(const vector<Token>& tokens, vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Строит дерево арифметического выражения по постфиксной записи.
 * @param[in] postfix Список токенов в постфиксной форме.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return Указатель на корень дерева выражения или nullptr при ошибке.
 */
ExprNode* buildTree(const vector<Token>& postfix, vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Вычисляет значение дерева арифметического выражения.
 *
 * Функция рекурсивно вычисляет значения дочерних узлов,
 * проверяет деление на ноль и наличие переменных в таблице значений.
 *
 * @param[in,out] node Корень дерева выражения.
 * @param[in] variables Таблица значений переменных.
 * @param[out] errors Вектор для хранения найденных ошибок.
 * @return Результат вычисления выражения.
 */
double calculate(ExprNode* node,
    const map<string, double>& variables,
    vector<Error>& errors);

/**
 * @ingroup functions
 * @brief Записывает дерево выражения в файл в формате DOT.
 * @param[in] root Корень дерева выражения.
 * @param[out] out Выходной файловый поток.
 */
void writeGraph(ExprNode* root, ofstream& out);

/**
 * @ingroup functions
 * @brief Генерирует описание узлов и связей дерева в формате DOT.
 *
 * Функция рекурсивно обходит дерево выражения и записывает
 * описание каждого узла и связи между узлами.
 *
 * @param[in] node Текущий узел дерева.
 * @param[out] out Выходной файловый поток.
 */
void generateGraph(ExprNode* node, ofstream& out);

/**
 * @ingroup functions
 * @brief Проверяет корректность имени переменной.
 *
 * Имя переменной должно начинаться с латинской буквы
 * и может содержать только латинские буквы и цифры.
 *
 * @param[in] name Имя переменной.
 * @return true, если имя корректно; false, если имя некорректно.
 */
bool isValidVariableName(const string& name);

/**
 * @ingroup functions
 * @brief Возвращает приоритет операции.
 * @param[in] token Строковое представление операции.
 * @return Целочисленный приоритет операции.
 */
int getPriority(const string& token);

/**
 * @ingroup functions
 * @brief Форматирует число для вывода.
 *
 * Целые числа выводятся без дробной части.
 * Нецелые числа округляются до трех знаков после точки.
 *
 * @param[in] value Число для вывода.
 * @return Строковое представление числа.
 */
string formatNumber(double value);

/**
 * @ingroup functions
 * @brief Удаляет начальные и конечные пробельные символы из строки.
 * @param[in] s Исходная строка.
 * @return Строка без пробелов в начале и конце.
 */
string trim(string s);

/**
 * @ingroup functions
 * @brief Выводит сообщение об ошибке в консоль.
 * @param[in] error Структура с информацией об ошибке.
 */
void printError(const Error& error);