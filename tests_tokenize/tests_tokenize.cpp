#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_tokenize
{
    TEST_CLASS(tests_tokenize)
    {
    private:
        void AssertToken(const Token& token,
                         const string& expectedText,
                         int expectedPosition,
                         TokenType expectedType)
        {
            Assert::AreEqual(expectedText, token.text);
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
            Assert::AreEqual(expectedToken, error.token);
        }

    public:

        TEST_METHOD(EmptyString)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("", errors);

            Assert::AreEqual(size_t(0), tokens.size());
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(OnlySpaces)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("     ", errors);

            Assert::AreEqual(size_t(0), tokens.size());
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(OneIntegerNumber)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("123", errors);

            Assert::AreEqual(size_t(1), tokens.size());
            AssertToken(tokens[0], "123", 0, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(OneRealNumber)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("2.5", errors);

            Assert::AreEqual(size_t(1), tokens.size());
            AssertToken(tokens[0], "2.5", 0, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(NumberOnUpperBoundary)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("2147483647", errors);

            Assert::AreEqual(size_t(1), tokens.size());
            AssertToken(tokens[0], "2147483647", 0, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(VariableLatinLetters)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("a", errors);

            Assert::AreEqual(size_t(1), tokens.size());
            AssertToken(tokens[0], "a", 0, TokenType::VARIABLE);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(VariableLatinLettersAndDigits)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("a1b", errors);

            Assert::AreEqual(size_t(1), tokens.size());
            AssertToken(tokens[0], "a1b", 0, TokenType::VARIABLE);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(AllBinaryOperations)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("2+3-4*5/6^7", errors);

            Assert::AreEqual(size_t(11), tokens.size());
            AssertToken(tokens[0], "2", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "+", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "3", 2, TokenType::NUMBER);
            AssertToken(tokens[3], "-", 3, TokenType::OPERATOR);
            AssertToken(tokens[4], "4", 4, TokenType::NUMBER);
            AssertToken(tokens[5], "*", 5, TokenType::OPERATOR);
            AssertToken(tokens[6], "5", 6, TokenType::NUMBER);
            AssertToken(tokens[7], "/", 7, TokenType::OPERATOR);
            AssertToken(tokens[8], "6", 8, TokenType::NUMBER);
            AssertToken(tokens[9], "^", 9, TokenType::OPERATOR);
            AssertToken(tokens[10], "7", 10, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(ExpressionWithBrackets)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("(2+3)", errors);

            Assert::AreEqual(size_t(5), tokens.size());
            AssertToken(tokens[0], "(", 0, TokenType::LEFT_BRACKET);
            AssertToken(tokens[1], "2", 1, TokenType::NUMBER);
            AssertToken(tokens[2], "+", 2, TokenType::OPERATOR);
            AssertToken(tokens[3], "3", 3, TokenType::NUMBER);
            AssertToken(tokens[4], ")", 4, TokenType::RIGHT_BRACKET);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(ExpressionWithSpaces)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize(" 12 + abc ", errors);

            Assert::AreEqual(size_t(3), tokens.size());
            AssertToken(tokens[0], "12", 1, TokenType::NUMBER);
            AssertToken(tokens[1], "+", 4, TokenType::OPERATOR);
            AssertToken(tokens[2], "abc", 6, TokenType::VARIABLE);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(UnaryMinusAtBeginning)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("-1", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "~", 0, TokenType::OPERATOR);
            AssertToken(tokens[1], "1", 1, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(UnaryMinusAfterLeftBracket)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("(-a)", errors);

            Assert::AreEqual(size_t(4), tokens.size());
            AssertToken(tokens[0], "(", 0, TokenType::LEFT_BRACKET);
            AssertToken(tokens[1], "~", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "a", 2, TokenType::VARIABLE);
            AssertToken(tokens[3], ")", 3, TokenType::RIGHT_BRACKET);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(UnaryMinusAfterBinaryOperator)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("2--3", errors);

            Assert::AreEqual(size_t(4), tokens.size());
            AssertToken(tokens[0], "2", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "-", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "~", 2, TokenType::OPERATOR);
            AssertToken(tokens[3], "3", 3, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(SeveralUnaryMinuses)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("--5", errors);

            Assert::AreEqual(size_t(3), tokens.size());
            AssertToken(tokens[0], "~", 0, TokenType::OPERATOR);
            AssertToken(tokens[1], "~", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "5", 2, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(UnaryMinusBeforeBracket)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("-(2+3)", errors);

            Assert::AreEqual(size_t(6), tokens.size());
            AssertToken(tokens[0], "~", 0, TokenType::OPERATOR);
            AssertToken(tokens[1], "(", 1, TokenType::LEFT_BRACKET);
            AssertToken(tokens[2], "2", 2, TokenType::NUMBER);
            AssertToken(tokens[3], "+", 3, TokenType::OPERATOR);
            AssertToken(tokens[4], "3", 4, TokenType::NUMBER);
            AssertToken(tokens[5], ")", 5, TokenType::RIGHT_BRACKET);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(BinaryMinusBetweenOperands)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("5-2", errors);

            Assert::AreEqual(size_t(3), tokens.size());
            AssertToken(tokens[0], "5", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "-", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "2", 2, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }

        TEST_METHOD(InvalidSymbol)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("1%8", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "1", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "8", 2, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_SYMBOL, 1, "%");
        }

        TEST_METHOD(RealNumberWithoutIntegerPart)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize(".5+2", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "+", 2, TokenType::OPERATOR);
            AssertToken(tokens[1], "2", 3, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, ".5");
        }

        TEST_METHOD(RealNumberWithoutFractionalPart)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("5.+2", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "+", 2, TokenType::OPERATOR);
            AssertToken(tokens[1], "2", 3, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, "5.");
        }

        TEST_METHOD(RealNumberWithTwoDots)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("1..2+3", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "+", 4, TokenType::OPERATOR);
            AssertToken(tokens[1], "3", 5, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, "1..2");
        }

        TEST_METHOD(RealNumberWithComma)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("1,5+2", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "+", 3, TokenType::OPERATOR);
            AssertToken(tokens[1], "2", 4, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_NUMBER_FORMAT, 0, "1,5");
        }

        TEST_METHOD(NumberOutOfRange)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("2147483648", errors);

            Assert::AreEqual(size_t(0), tokens.size());
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::NUMBER_OUT_OF_RANGE, 0, "2147483648");
        }

        TEST_METHOD(NegativeNumberOutOfRange)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("-2147483648", errors);

            Assert::AreEqual(size_t(1), tokens.size());
            AssertToken(tokens[0], "~", 0, TokenType::OPERATOR);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::NUMBER_OUT_OF_RANGE, 1, "2147483648");
        }

        TEST_METHOD(TwoNumbersInRow)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("1 2", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "1", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "2", 2, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 2, "2");
        }

        TEST_METHOD(VariableAndNumberWithoutOperator)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("a 2", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "a", 0, TokenType::VARIABLE);
            AssertToken(tokens[1], "2", 2, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 2, "2");
        }

        TEST_METHOD(OperandBeforeLeftBracket)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("2(3+4)", errors);

            Assert::AreEqual(size_t(6), tokens.size());
            AssertToken(tokens[0], "2", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "(", 1, TokenType::LEFT_BRACKET);
            AssertToken(tokens[2], "3", 2, TokenType::NUMBER);
            AssertToken(tokens[3], "+", 3, TokenType::OPERATOR);
            AssertToken(tokens[4], "4", 4, TokenType::NUMBER);
            AssertToken(tokens[5], ")", 5, TokenType::RIGHT_BRACKET);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 1, "(");
        }

        TEST_METHOD(RightBracketBeforeOperand)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("(2+3)4", errors);

            Assert::AreEqual(size_t(6), tokens.size());
            AssertToken(tokens[0], "(", 0, TokenType::LEFT_BRACKET);
            AssertToken(tokens[1], "2", 1, TokenType::NUMBER);
            AssertToken(tokens[2], "+", 2, TokenType::OPERATOR);
            AssertToken(tokens[3], "3", 3, TokenType::NUMBER);
            AssertToken(tokens[4], ")", 4, TokenType::RIGHT_BRACKET);
            AssertToken(tokens[5], "4", 5, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 5, "4");
        }

        TEST_METHOD(TwoBinaryOperatorsInRow)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("1++2", errors);

            Assert::AreEqual(size_t(4), tokens.size());
            AssertToken(tokens[0], "1", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "+", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "+", 2, TokenType::OPERATOR);
            AssertToken(tokens[3], "2", 3, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 2, "+");
        }

        TEST_METHOD(BinaryOperatorAfterLeftBracket)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("(+5)", errors);

            Assert::AreEqual(size_t(4), tokens.size());
            AssertToken(tokens[0], "(", 0, TokenType::LEFT_BRACKET);
            AssertToken(tokens[1], "+", 1, TokenType::OPERATOR);
            AssertToken(tokens[2], "5", 2, TokenType::NUMBER);
            AssertToken(tokens[3], ")", 3, TokenType::RIGHT_BRACKET);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 1, "+");
        }

        TEST_METHOD(BinaryOperatorAtBeginning)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("+10", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "+", 0, TokenType::OPERATOR);
            AssertToken(tokens[1], "10", 1, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 0, "+");
        }

        TEST_METHOD(BinaryOperatorAtEnd)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("10-", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "10", 0, TokenType::NUMBER);
            AssertToken(tokens[1], "-", 2, TokenType::OPERATOR);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 2, "-");
        }

        TEST_METHOD(CyrillicLetterInsteadOfVariable)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("ã+5", errors);

            Assert::AreEqual(size_t(2), tokens.size());
            AssertToken(tokens[0], "+", 1, TokenType::OPERATOR);
            AssertToken(tokens[1], "5", 2, TokenType::NUMBER);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::INVALID_SYMBOL, 0, "ã");
        }

        TEST_METHOD(ComplexCorrectExpression)
        {
            vector<Error> errors;

            vector<Token> tokens = tokenize("a1*(-2.5+b)^3", errors);

            Assert::AreEqual(size_t(10), tokens.size());
            AssertToken(tokens[0], "a1", 0, TokenType::VARIABLE);
            AssertToken(tokens[1], "*", 2, TokenType::OPERATOR);
            AssertToken(tokens[2], "(", 3, TokenType::LEFT_BRACKET);
            AssertToken(tokens[3], "~", 4, TokenType::OPERATOR);
            AssertToken(tokens[4], "2.5", 5, TokenType::NUMBER);
            AssertToken(tokens[5], "+", 8, TokenType::OPERATOR);
            AssertToken(tokens[6], "b", 9, TokenType::VARIABLE);
            AssertToken(tokens[7], ")", 10, TokenType::RIGHT_BRACKET);
            AssertToken(tokens[8], "^", 11, TokenType::OPERATOR);
            AssertToken(tokens[9], "3", 12, TokenType::NUMBER);
            Assert::AreEqual(size_t(0), errors.size());
        }
    };
}
