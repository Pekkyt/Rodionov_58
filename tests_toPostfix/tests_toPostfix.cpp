#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_toPostfix
{
    TEST_CLASS(tests_toPostfix)
    {
    private:
        void AssertToken(const Token& token,
            const string& expectedText,
            int expectedPosition,
            TokenType expectedType)
        {
            Assert::AreEqual(expectedText.c_str(), token.text.c_str());
            Assert::AreEqual(expectedPosition, token.position);
            Assert::IsTrue(expectedType == token.type);
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

    public:

        // 1. Вектор токенов пуст
        TEST_METHOD(EmptyTokenVector)
        {
            vector<Token> tokens = {};
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(0), postfix.size());
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 2. Выражение из одного числа
        TEST_METHOD(OneNumber)
        {
            vector<Token> tokens = {
                {"123", 0, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(1), postfix.size());
            AssertToken(postfix[0], "123", 0, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 3. Выражение из одной переменной
        TEST_METHOD(OneVariable)
        {
            vector<Token> tokens = {
                {"x", 0, TokenType::VARIABLE}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(1), postfix.size());
            AssertToken(postfix[0], "x", 0, TokenType::VARIABLE);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 4. Одиночная бинарная операция
        TEST_METHOD(SingleBinaryOperation)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"+", 1, TokenType::OPERATOR},
                {"3", 2, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(3), postfix.size());
            AssertToken(postfix[0], "2", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 2, TokenType::NUMBER);
            AssertToken(postfix[2], "+", 1, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 5. Учет приоритета умножения перед сложением
        TEST_METHOD(MultiplicationPriorityBeforeAddition)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"+", 1, TokenType::OPERATOR},
                {"3", 2, TokenType::NUMBER},
                {"*", 3, TokenType::OPERATOR},
                {"4", 4, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(5), postfix.size());
            AssertToken(postfix[0], "2", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 2, TokenType::NUMBER);
            AssertToken(postfix[2], "4", 4, TokenType::NUMBER);
            AssertToken(postfix[3], "*", 3, TokenType::OPERATOR);
            AssertToken(postfix[4], "+", 1, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 6. Изменение приоритета с помощью скобок
        TEST_METHOD(BracketsChangePriority)
        {
            vector<Token> tokens = {
                {"(", 0, TokenType::LEFT_BRACKET},
                {"2", 1, TokenType::NUMBER},
                {"+", 2, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER},
                {")", 4, TokenType::RIGHT_BRACKET},
                {"*", 5, TokenType::OPERATOR},
                {"4", 6, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(5), postfix.size());
            AssertToken(postfix[0], "2", 1, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 3, TokenType::NUMBER);
            AssertToken(postfix[2], "+", 2, TokenType::OPERATOR);
            AssertToken(postfix[3], "4", 6, TokenType::NUMBER);
            AssertToken(postfix[4], "*", 5, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 7. Вложенные скобки
        TEST_METHOD(NestedBrackets)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"*", 1, TokenType::OPERATOR},
                {"(", 2, TokenType::LEFT_BRACKET},
                {"3", 3, TokenType::NUMBER},
                {"+", 4, TokenType::OPERATOR},
                {"(", 5, TokenType::LEFT_BRACKET},
                {"4", 6, TokenType::NUMBER},
                {"-", 7, TokenType::OPERATOR},
                {"1", 8, TokenType::NUMBER},
                {")", 9, TokenType::RIGHT_BRACKET},
                {")", 10, TokenType::RIGHT_BRACKET}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(7), postfix.size());
            AssertToken(postfix[0], "2", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 3, TokenType::NUMBER);
            AssertToken(postfix[2], "4", 6, TokenType::NUMBER);
            AssertToken(postfix[3], "1", 8, TokenType::NUMBER);
            AssertToken(postfix[4], "-", 7, TokenType::OPERATOR);
            AssertToken(postfix[5], "+", 4, TokenType::OPERATOR);
            AssertToken(postfix[6], "*", 1, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 8. Левоассоциативность сложения и вычитания
        TEST_METHOD(LeftAssociativityAdditionAndSubtraction)
        {
            vector<Token> tokens = {
                {"5", 0, TokenType::NUMBER},
                {"-", 1, TokenType::OPERATOR},
                {"2", 2, TokenType::NUMBER},
                {"+", 3, TokenType::OPERATOR},
                {"1", 4, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(5), postfix.size());
            AssertToken(postfix[0], "5", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "2", 2, TokenType::NUMBER);
            AssertToken(postfix[2], "-", 1, TokenType::OPERATOR);
            AssertToken(postfix[3], "1", 4, TokenType::NUMBER);
            AssertToken(postfix[4], "+", 3, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 9. Левоассоциативность умножения и деления
        TEST_METHOD(LeftAssociativityMultiplicationAndDivision)
        {
            vector<Token> tokens = {
                {"8", 0, TokenType::NUMBER},
                {"/", 1, TokenType::OPERATOR},
                {"2", 2, TokenType::NUMBER},
                {"*", 3, TokenType::OPERATOR},
                {"3", 4, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(5), postfix.size());
            AssertToken(postfix[0], "8", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "2", 2, TokenType::NUMBER);
            AssertToken(postfix[2], "/", 1, TokenType::OPERATOR);
            AssertToken(postfix[3], "3", 4, TokenType::NUMBER);
            AssertToken(postfix[4], "*", 3, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 10. Левоассоциативность возведения в степень
        TEST_METHOD(LeftAssociativityPower)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"^", 1, TokenType::OPERATOR},
                {"3", 2, TokenType::NUMBER},
                {"^", 3, TokenType::OPERATOR},
                {"2", 4, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(5), postfix.size());
            AssertToken(postfix[0], "2", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 2, TokenType::NUMBER);
            AssertToken(postfix[2], "^", 1, TokenType::OPERATOR);
            AssertToken(postfix[3], "2", 4, TokenType::NUMBER);
            AssertToken(postfix[4], "^", 3, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 11. Унарный минус перед числом
        TEST_METHOD(UnaryMinusBeforeNumber)
        {
            vector<Token> tokens = {
                {"~", 0, TokenType::OPERATOR},
                {"5", 1, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(2), postfix.size());
            AssertToken(postfix[0], "5", 1, TokenType::NUMBER);
            AssertToken(postfix[1], "~", 0, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 12. Унарный минус перед выражением в скобках
        TEST_METHOD(UnaryMinusBeforeBracketExpression)
        {
            vector<Token> tokens = {
                {"~", 0, TokenType::OPERATOR},
                {"(", 1, TokenType::LEFT_BRACKET},
                {"2", 2, TokenType::NUMBER},
                {"+", 3, TokenType::OPERATOR},
                {"3", 4, TokenType::NUMBER},
                {")", 5, TokenType::RIGHT_BRACKET}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(4), postfix.size());
            AssertToken(postfix[0], "2", 2, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 4, TokenType::NUMBER);
            AssertToken(postfix[2], "+", 3, TokenType::OPERATOR);
            AssertToken(postfix[3], "~", 0, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 13. Унарный минус после бинарного оператора
        TEST_METHOD(UnaryMinusAfterBinaryOperator)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"*", 1, TokenType::OPERATOR},
                {"~", 2, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(4), postfix.size());
            AssertToken(postfix[0], "2", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 3, TokenType::NUMBER);
            AssertToken(postfix[2], "~", 2, TokenType::OPERATOR);
            AssertToken(postfix[3], "*", 1, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 14. Несколько унарных минусов подряд
        TEST_METHOD(SeveralUnaryMinuses)
        {
            vector<Token> tokens = {
                {"~", 0, TokenType::OPERATOR},
                {"~", 1, TokenType::OPERATOR},
                {"5", 2, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(3), postfix.size());
            AssertToken(postfix[0], "5", 2, TokenType::NUMBER);
            AssertToken(postfix[1], "~", 1, TokenType::OPERATOR);
            AssertToken(postfix[2], "~", 0, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 15. Унарный минус имеет больший приоритет, чем степень
        TEST_METHOD(UnaryMinusPriorityHigherThanPower)
        {
            vector<Token> tokens = {
                {"~", 0, TokenType::OPERATOR},
                {"2", 1, TokenType::NUMBER},
                {"^", 2, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(4), postfix.size());
            AssertToken(postfix[0], "2", 1, TokenType::NUMBER);
            AssertToken(postfix[1], "~", 0, TokenType::OPERATOR);
            AssertToken(postfix[2], "3", 3, TokenType::NUMBER);
            AssertToken(postfix[3], "^", 2, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 16. Унарный минус в правом операнде степени
        TEST_METHOD(UnaryMinusInRightOperandOfPower)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"^", 1, TokenType::OPERATOR},
                {"~", 2, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(4), postfix.size());
            AssertToken(postfix[0], "2", 0, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 3, TokenType::NUMBER);
            AssertToken(postfix[2], "~", 2, TokenType::OPERATOR);
            AssertToken(postfix[3], "^", 1, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 17. Две независимые группы скобок
        TEST_METHOD(TwoIndependentBracketGroups)
        {
            vector<Token> tokens = {
                {"(", 0, TokenType::LEFT_BRACKET},
                {"2", 1, TokenType::NUMBER},
                {"+", 2, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER},
                {")", 4, TokenType::RIGHT_BRACKET},
                {"/", 5, TokenType::OPERATOR},
                {"(", 6, TokenType::LEFT_BRACKET},
                {"4", 7, TokenType::NUMBER},
                {"-", 8, TokenType::OPERATOR},
                {"1", 9, TokenType::NUMBER},
                {")", 10, TokenType::RIGHT_BRACKET}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(7), postfix.size());
            AssertToken(postfix[0], "2", 1, TokenType::NUMBER);
            AssertToken(postfix[1], "3", 3, TokenType::NUMBER);
            AssertToken(postfix[2], "+", 2, TokenType::OPERATOR);
            AssertToken(postfix[3], "4", 7, TokenType::NUMBER);
            AssertToken(postfix[4], "1", 9, TokenType::NUMBER);
            AssertToken(postfix[5], "-", 8, TokenType::OPERATOR);
            AssertToken(postfix[6], "/", 5, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 18. Сложное выражение с переменной, вещественным числом, унарным минусом и степенью
        TEST_METHOD(ComplexExpression)
        {
            vector<Token> tokens = {
                {"a1", 0, TokenType::VARIABLE},
                {"*", 2, TokenType::OPERATOR},
                {"(", 3, TokenType::LEFT_BRACKET},
                {"~", 4, TokenType::OPERATOR},
                {"2.5", 5, TokenType::NUMBER},
                {"+", 8, TokenType::OPERATOR},
                {"b", 9, TokenType::VARIABLE},
                {")", 10, TokenType::RIGHT_BRACKET},
                {"^", 11, TokenType::OPERATOR},
                {"3", 12, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(8), postfix.size());
            AssertToken(postfix[0], "a1", 0, TokenType::VARIABLE);
            AssertToken(postfix[1], "2.5", 5, TokenType::NUMBER);
            AssertToken(postfix[2], "~", 4, TokenType::OPERATOR);
            AssertToken(postfix[3], "b", 9, TokenType::VARIABLE);
            AssertToken(postfix[4], "+", 8, TokenType::OPERATOR);
            AssertToken(postfix[5], "3", 12, TokenType::NUMBER);
            AssertToken(postfix[6], "^", 11, TokenType::OPERATOR);
            AssertToken(postfix[7], "*", 2, TokenType::OPERATOR);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 19. Пустые скобки
        TEST_METHOD(EmptyBrackets)
        {
            vector<Token> tokens = {
                {"(", 0, TokenType::LEFT_BRACKET},
                {")", 1, TokenType::RIGHT_BRACKET}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(0), postfix.size());
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 20. Закрывающая скобка без открывающей
        TEST_METHOD(ClosingBracketWithoutOpeningBracket)
        {
            vector<Token> tokens = {
                {"2", 0, TokenType::NUMBER},
                {"+", 1, TokenType::OPERATOR},
                {"3", 2, TokenType::NUMBER},
                {")", 3, TokenType::RIGHT_BRACKET}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(0), postfix.size());
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISMATCHED_BRACKETS, 3, ")");
        }

        // 21. Открывающая скобка без закрывающей
        TEST_METHOD(OpeningBracketWithoutClosingBracket)
        {
            vector<Token> tokens = {
                {"(", 0, TokenType::LEFT_BRACKET},
                {"2", 1, TokenType::NUMBER},
                {"+", 2, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER}
            };
            vector<Error> errors;

            vector<Token> postfix = toPostfix(tokens, errors);

            Assert::AreEqual(size_t(0), postfix.size());
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISMATCHED_BRACKETS, 0, "(");
        }
    };
}