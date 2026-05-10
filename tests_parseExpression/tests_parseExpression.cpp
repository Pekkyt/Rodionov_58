#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_parseExpression
{
    TEST_CLASS(tests_parseExpression)
    {
    private:
        void AssertError(const Error& error,
            ErrorType expectedType,
            int expectedPosition,
            const string& expectedToken)
        {
            Assert::IsTrue(expectedType == error.type);
            Assert::AreEqual(expectedPosition, error.position);
            Assert::AreEqual(expectedToken.c_str(), error.token.c_str());
        }

        void AssertNumber(ExprNode* node,
            double expectedValue,
            const string& expectedToken)
        {
            Assert::IsNotNull(node);
            Assert::IsTrue(ExprNodeType::NUMBER == node->type);
            Assert::AreEqual(expectedValue, node->value, 0.000001);
            Assert::AreEqual(expectedToken.c_str(), node->token.c_str());
            Assert::IsNull(node->left);
            Assert::IsNull(node->right);
        }

        void AssertVariable(ExprNode* node,
            const string& expectedToken)
        {
            Assert::IsNotNull(node);
            Assert::IsTrue(ExprNodeType::VARIABLE == node->type);
            Assert::AreEqual(expectedToken.c_str(), node->token.c_str());
            Assert::IsNull(node->left);
            Assert::IsNull(node->right);
        }

        void AssertUnary(ExprNode* node,
            const string& expectedToken)
        {
            Assert::IsNotNull(node);
            Assert::IsTrue(ExprNodeType::UNARY_MINUS == node->type);
            Assert::AreEqual(expectedToken.c_str(), node->token.c_str());
            Assert::IsNotNull(node->left);
            Assert::IsNull(node->right);
        }

        void AssertBinary(ExprNode* node,
            ExprNodeType expectedType,
            const string& expectedToken)
        {
            Assert::IsNotNull(node);
            Assert::IsTrue(expectedType == node->type);
            Assert::AreEqual(expectedToken.c_str(), node->token.c_str());
            Assert::IsNotNull(node->left);
            Assert::IsNotNull(node->right);
        }

        int CountNodes(ExprNode* node)
        {
            if (node == nullptr)
            {
                return 0;
            }

            return 1 + CountNodes(node->left) + CountNodes(node->right);
        }

        int CountOperationNodes(ExprNode* node)
        {
            if (node == nullptr)
            {
                return 0;
            }

            if (node->type == ExprNodeType::NUMBER ||
                node->type == ExprNodeType::VARIABLE)
            {
                return CountOperationNodes(node->left) + CountOperationNodes(node->right);
            }

            return 1 + CountOperationNodes(node->left) + CountOperationNodes(node->right);
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

            AssertNumber(root, 123, "123");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 4. Выражение из одного вещественного числа
        TEST_METHOD(OneRealNumber)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("12.34", errors);

            AssertNumber(root, 12.34, "12.34");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 5. Выражение из одной переменной
        TEST_METHOD(OneVariable)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("x", errors);

            AssertVariable(root, "x");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 6. Одиночная бинарная операция
        TEST_METHOD(SingleBinaryOperation)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("5-2", errors);

            AssertBinary(root, ExprNodeType::SUB, "-");
            AssertNumber(root->left, 5, "5");
            AssertNumber(root->right, 2, "2");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 7. Унарный минус перед числом
        TEST_METHOD(UnaryMinusBeforeNumber)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-5", errors);

            AssertUnary(root, "~");
            AssertNumber(root->left, 5, "5");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 8. Несколько унарных минусов подряд
        TEST_METHOD(SeveralUnaryMinuses)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("--5", errors);

            AssertUnary(root, "~");
            AssertUnary(root->left, "~");
            AssertNumber(root->left->left, 5, "5");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 9. Унарный минус перед выражением в скобках
        TEST_METHOD(UnaryMinusBeforeBracketExpression)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-(2+3)", errors);

            AssertUnary(root, "~");
            AssertBinary(root->left, ExprNodeType::ADD, "+");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->left->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 10. Бинарная операция с унарным правым операндом
        TEST_METHOD(BinaryOperationWithUnaryRightOperand)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2*-3", errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertNumber(root->left, 2, "2");
            AssertUnary(root->right, "~");
            AssertNumber(root->right->left, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 11. Бинарная операция с унарным левым операндом
        TEST_METHOD(BinaryOperationWithUnaryLeftOperand)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-2+3", errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertUnary(root->left, "~");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 12. Приоритет умножения выше сложения
        TEST_METHOD(MultiplicationPriorityBeforeAddition)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2+3*4", errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertNumber(root->left, 2, "2");
            AssertBinary(root->right, ExprNodeType::MUL, "*");
            AssertNumber(root->right->left, 3, "3");
            AssertNumber(root->right->right, 4, "4");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 13. Изменение приоритета с помощью скобок
        TEST_METHOD(BracketsChangePriority)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("(2+3)*4", errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertBinary(root->left, ExprNodeType::ADD, "+");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->left->right, 3, "3");
            AssertNumber(root->right, 4, "4");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 14. Вложенные скобки
        TEST_METHOD(NestedBrackets)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2*(3+(4-1))", errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertNumber(root->left, 2, "2");

            AssertBinary(root->right, ExprNodeType::ADD, "+");
            AssertNumber(root->right->left, 3, "3");

            AssertBinary(root->right->right, ExprNodeType::SUB, "-");
            AssertNumber(root->right->right->left, 4, "4");
            AssertNumber(root->right->right->right, 1, "1");

            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 15. Левоассоциативность деления
        TEST_METHOD(LeftAssociativityDivision)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("9/3/3", errors);

            AssertBinary(root, ExprNodeType::DIV, "/");
            AssertBinary(root->left, ExprNodeType::DIV, "/");
            AssertNumber(root->left->left, 9, "9");
            AssertNumber(root->left->right, 3, "3");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 16. Левоассоциативность возведения в степень
        TEST_METHOD(LeftAssociativityPower)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("2^3^2", errors);

            AssertBinary(root, ExprNodeType::POW, "^");
            AssertBinary(root->left, ExprNodeType::POW, "^");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->left->right, 3, "3");
            AssertNumber(root->right, 2, "2");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 17. Приоритет унарного минуса выше степени
        TEST_METHOD(UnaryMinusPriorityHigherThanPower)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("-2^3", errors);

            AssertBinary(root, ExprNodeType::POW, "^");
            AssertUnary(root->left, "~");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 18. Выражение с переменными
        TEST_METHOD(ExpressionWithVariables)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("a+b*c", errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertVariable(root->left, "a");
            AssertBinary(root->right, ExprNodeType::MUL, "*");
            AssertVariable(root->right->left, "b");
            AssertVariable(root->right->right, "c");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 19. Сложное корректное выражение
        TEST_METHOD(ComplexCorrectExpression)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("a1*(-2.5+b)^3", errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertVariable(root->left, "a1");

            AssertBinary(root->right, ExprNodeType::POW, "^");
            AssertBinary(root->right->left, ExprNodeType::ADD, "+");
            AssertUnary(root->right->left->left, "~");
            AssertNumber(root->right->left->left->left, 2.5, "2.5");
            AssertVariable(root->right->left->right, "b");
            AssertNumber(root->right->right, 3, "3");

            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 20. Выражение с пробелами между токенами
        TEST_METHOD(ExpressionWithSpaces)
        {
            vector<Error> errors;

            ExprNode* root = parseExpression("  12  +   abc ", errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertNumber(root->left, 12, "12");
            AssertVariable(root->right, "abc");
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 21. Выражение из 100 операций
        TEST_METHOD(ExpressionWith100Operations)
        {
            vector<Error> errors;
            string expression = MakePowerExpression(100);

            ExprNode* root = parseExpression(expression, errors);

            Assert::IsNotNull(root);
            Assert::IsTrue(ExprNodeType::POW == root->type);
            Assert::AreEqual(201, CountNodes(root));
            Assert::AreEqual(100, CountOperationNodes(root));
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
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