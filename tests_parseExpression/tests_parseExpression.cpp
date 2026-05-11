#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_parseExpression
{
    TEST_CLASS(tests_parseExpression)
    {
    private:
        ExprNode* CreateNumber(double value, const string& token)
        {
            ExprNode* node = new ExprNode(value);
            node->value = value;
            node->type = ExprNodeType::NUMBER;
            node->left = nullptr;
            node->right = nullptr;
            node->token = token;
            return node;
        }

        ExprNode* CreateVariable(const string& token)
        {
            ExprNode* node = new ExprNode(0.0);
            node->value = 0;
            node->type = ExprNodeType::VARIABLE;
            node->left = nullptr;
            node->right = nullptr;
            node->token = token;
            return node;
        }

        ExprNode* CreateUnary(ExprNode* child)
        {
            ExprNode* node = new ExprNode(0.0);
            node->value = 0;
            node->type = ExprNodeType::UNARY_MINUS;
            node->left = child;
            node->right = nullptr;
            node->token = "~";
            return node;
        }

        ExprNode* CreateBinary(ExprNodeType type, const string& token, ExprNode* left, ExprNode* right)
        {
            ExprNode* node = new ExprNode(0.0);
            node->value = 0;
            node->type = type;
            node->left = left;
            node->right = right;
            node->token = token;
            return node;
        }

        void AssertDouble(double expected, double actual)
        {
            Assert::AreEqual(expected, actual, 0.000001);
        }

        void AssertError(const Error& error,
            ErrorType expectedType,
            int expectedPosition,
            const string& expectedToken)
        {
            Assert::IsTrue(expectedType == error.type);
            Assert::AreEqual(expectedPosition, error.position);
            Assert::AreEqual(expectedToken.c_str(), error.token.c_str());
        }

        void AssertTreeEqual(ExprNode* actual, ExprNode* expected)
        {
            if (expected == nullptr)
            {
                Assert::IsNull(actual);
                return;
            }

            Assert::IsNotNull(actual);
            Assert::IsTrue(expected->type == actual->type);
            Assert::AreEqual(expected->token.c_str(), actual->token.c_str());

            if (expected->type != ExprNodeType::VARIABLE)
            {
                AssertDouble(expected->value, actual->value);
            }

            AssertTreeEqual(actual->left, expected->left);
            AssertTreeEqual(actual->right, expected->right);
        }

        string MakePowerExpression(int operationsCount)
        {
            string expression = "1";

            for (int i = 0; i < operationsCount; i++)
            {
                expression += "^1";
            }

            return expression;
        }

        ExprNode* MakeExpectedPowerTree(int operationsCount)
        {
            ExprNode* root = CreateNumber(1, "1");

            for (int i = 1; i <= operationsCount; i++)
            {
                root = CreateBinary(ExprNodeType::POW, "^", root, CreateNumber(1, "1"));
            }

            return root;
        }

    public:

        // 1. Пустая строка
        TEST_METHOD(EmptyString)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, -1, "");
        }

        // 2. Строка только из пробелов
        TEST_METHOD(OnlySpaces)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("     ", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, -1, "");
        }

        // 3. Выражение из одного числа
        TEST_METHOD(OneNumber)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("123", errors);
            ExprNode* expected = CreateNumber(123, "123");

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 4. Выражение из одного вещественного числа
        TEST_METHOD(OneRealNumber)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("12.34", errors);
            ExprNode* expected = CreateNumber(12.34, "12.34");

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 5. Выражение из одной переменной
        TEST_METHOD(OneVariable)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("x", errors);
            ExprNode* expected = CreateVariable("x");

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 6. Одиночная бинарная операция
        TEST_METHOD(SingleBinaryOperation)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("5-2", errors);
            ExprNode* expected = CreateBinary(ExprNodeType::SUB, "-", CreateNumber(5, "5"), CreateNumber(2, "2"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 7. Унарный минус перед числом
        TEST_METHOD(UnaryMinusBeforeNumber)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-5", errors);
            ExprNode* expected = CreateUnary(CreateNumber(5, "5"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 8. Несколько унарных минусов подряд
        TEST_METHOD(SeveralUnaryMinuses)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("--5", errors);
            ExprNode* expected = CreateUnary(CreateUnary(CreateNumber(5, "5")));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 9. Унарный минус перед выражением в скобках
        TEST_METHOD(UnaryMinusBeforeBracketExpression)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-(2+3)", errors);
            ExprNode* expected = CreateUnary(
                CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3"))
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 10. Бинарная операция с унарным правым операндом
        TEST_METHOD(BinaryOperationWithUnaryRightOperand)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2*-3", errors);
            ExprNode* expected = CreateBinary(ExprNodeType::MUL, "*", CreateNumber(2, "2"), CreateUnary(CreateNumber(3, "3")));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 11. Бинарная операция с унарным левым операндом
        TEST_METHOD(BinaryOperationWithUnaryLeftOperand)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-2+3", errors);
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateUnary(CreateNumber(2, "2")), CreateNumber(3, "3"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 12. Приоритет умножения выше сложения
        TEST_METHOD(MultiplicationPriorityBeforeAddition)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2+3*4", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateNumber(2, "2"),
                CreateBinary(ExprNodeType::MUL, "*", CreateNumber(3, "3"), CreateNumber(4, "4"))
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 13. Изменение приоритета с помощью скобок
        TEST_METHOD(BracketsChangePriority)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("(2+3)*4", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateNumber(4, "4")
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 14. Вложенные скобки
        TEST_METHOD(NestedBrackets)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2*(3+(4-1))", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateNumber(2, "2"),
                CreateBinary(
                    ExprNodeType::ADD, "+",
                    CreateNumber(3, "3"),
                    CreateBinary(ExprNodeType::SUB, "-", CreateNumber(4, "4"), CreateNumber(1, "1"))
                )
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 15. Левоассоциативность деления
        TEST_METHOD(LeftAssociativityDivision)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("9/3/3", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::DIV, "/",
                CreateBinary(ExprNodeType::DIV, "/", CreateNumber(9, "9"), CreateNumber(3, "3")),
                CreateNumber(3, "3")
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 16. Левоассоциативность возведения в степень
        TEST_METHOD(LeftAssociativityPower)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2^3^2", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::POW, "^",
                CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateNumber(2, "2")
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 17. Приоритет унарного минуса выше степени
        TEST_METHOD(UnaryMinusPriorityHigherThanPower)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-2^3", errors);
            ExprNode* expected = CreateBinary(ExprNodeType::POW, "^", CreateUnary(CreateNumber(2, "2")), CreateNumber(3, "3"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 18. Выражение с переменными
        TEST_METHOD(ExpressionWithVariables)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("a+b*c", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateVariable("a"),
                CreateBinary(ExprNodeType::MUL, "*", CreateVariable("b"), CreateVariable("c"))
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 19. Сложное корректное выражение
        TEST_METHOD(ComplexCorrectExpression)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("a1*(-2.5+b)^3", errors);
            ExprNode* expected = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateVariable("a1"),
                CreateBinary(
                    ExprNodeType::POW, "^",
                    CreateBinary(
                        ExprNodeType::ADD, "+",
                        CreateUnary(CreateNumber(2.5, "2.5")),
                        CreateVariable("b")
                    ),
                    CreateNumber(3, "3")
                )
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 20. Выражение с пробелами между токенами
        TEST_METHOD(ExpressionWithSpaces)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("  12  +   abc ", errors);
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(12, "12"), CreateVariable("abc"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 21. Выражение из 100 операций
        TEST_METHOD(ExpressionWith100Operations)
        {
            vector<Error> errors;
            string expression = MakePowerExpression(100);

            ExprNode* root = parseExpression(expression, errors);
            ExprNode* expected = MakeExpectedPowerTree(100);

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 22. Выражение из 101 операции
        TEST_METHOD(ExpressionWith101Operations)
        {
            vector<Error> errors;
            string expression = MakePowerExpression(101);

            ExprNode* root = parseExpression(expression, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::TOO_MANY_OPERATIONS, -1, "");
        }

        // 23. Неподдерживаемый символ
        TEST_METHOD(InvalidSymbol)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("7%8", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_SYMBOL, 1, "%");
        }

        // 24. Некорректный формат вещественного числа без целой части
        TEST_METHOD(RealNumberWithoutIntegerPart)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression(".5+2", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, ".5");
        }

        // 25. Некорректный формат вещественного числа без дробной части
        TEST_METHOD(RealNumberWithoutFractionalPart)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("5.+2", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, "5.");
        }

        // 26. Некорректный формат вещественного числа с двумя точками
        TEST_METHOD(RealNumberWithTwoDots)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("1..2+3", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, "1..2");
        }

        // 27. Число больше допустимого диапазона
        TEST_METHOD(NumberOutOfRange)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2147483648", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::NUMBER_OUT_OF_RANGE, 0, "2147483648");
        }

        // 28. Отрицательное число больше допустимого диапазона по модулю
        TEST_METHOD(NegativeNumberOutOfRange)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-2147483648", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::NUMBER_OUT_OF_RANGE, 1, "2147483648");
        }

        // 29. Два операнда подряд
        TEST_METHOD(TwoOperandsInRow)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("4 5", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 2, "5");
        }

        // 30. Операнд перед открывающей скобкой
        TEST_METHOD(OperandBeforeLeftBracket)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2(3+4)", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 1, "(");
        }

        // 31. Закрывающая скобка перед операндом
        TEST_METHOD(RightBracketBeforeOperand)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("(2+3)4", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 5, "4");
        }

        // 32. Два бинарных оператора подряд
        TEST_METHOD(TwoBinaryOperatorsInRow)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("3++4", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 2, "+");
        }

        // 33. Бинарный оператор в начале выражения
        TEST_METHOD(BinaryOperatorAtBeginning)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("+10", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 0, "+");
        }

        // 34. Бинарный оператор в конце выражения
        TEST_METHOD(BinaryOperatorAtEnd)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("10-", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 2, "-");
        }

        // 35. Открывающая скобка без закрывающей
        TEST_METHOD(OpeningBracketWithoutClosingBracket)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("(2+3", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISMATCHED_BRACKETS, 0, "(");
        }

        // 36. Закрывающая скобка без открывающей
        TEST_METHOD(ClosingBracketWithoutOpeningBracket)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2+3)", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISMATCHED_BRACKETS, 3, ")");
        }

        // 37. Кириллическая буква вместо латинской переменной
        TEST_METHOD(CyrillicLetterInsteadOfVariable)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("г+5", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_SYMBOL, 0, "г");
        }

        // 38. Пустые скобки
        TEST_METHOD(EmptyBrackets)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("()", errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, -1, "");
        }

    };
}
