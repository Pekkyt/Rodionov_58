#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_buildTree
{
    TEST_CLASS(tests_buildTree)
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

        vector<Token> MakePowerPostfix(int operationsCount)
        {
            vector<Token> postfix;

            postfix.push_back({ "1", 0, TokenType::NUMBER });

            for (int i = 1; i <= operationsCount; i++)
            {
                postfix.push_back({ "1", 4 * i - 2, TokenType::NUMBER });
                postfix.push_back({ "^", 4 * i, TokenType::OPERATOR });
            }

            return postfix;
        }

    public:

        // 1. Вектор токенов пуст
        TEST_METHOD(EmptyTokenVector)
        {
            vector<Token> postfix = {};
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 2. Выражение из одного целого числа
        TEST_METHOD(OneIntegerNumber)
        {
            vector<Token> postfix = {
                {"123", 0, TokenType::NUMBER}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertNumber(root, 123, "123");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 3. Выражение из одного вещественного числа
        TEST_METHOD(OneRealNumber)
        {
            vector<Token> postfix = {
                {"12.34", 0, TokenType::NUMBER}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertNumber(root, 12.34, "12.34");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 4. Выражение из одной переменной
        TEST_METHOD(OneVariable)
        {
            vector<Token> postfix = {
                {"x", 0, TokenType::VARIABLE}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertVariable(root, "x");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 5. Одиночная операция сложения
        TEST_METHOD(SingleAdditionOperation)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"+", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertNumber(root->left, 2, "2");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 6. Одиночная операция вычитания
        TEST_METHOD(SingleSubtractionOperation)
        {
            vector<Token> postfix = {
                {"5", 0, TokenType::NUMBER},
                {"2", 2, TokenType::NUMBER},
                {"-", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::SUB, "-");
            AssertNumber(root->left, 5, "5");
            AssertNumber(root->right, 2, "2");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 7. Одиночная операция умножения
        TEST_METHOD(SingleMultiplicationOperation)
        {
            vector<Token> postfix = {
                {"4", 0, TokenType::NUMBER},
                {"6", 2, TokenType::NUMBER},
                {"*", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertNumber(root->left, 4, "4");
            AssertNumber(root->right, 6, "6");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 8. Одиночная операция деления
        TEST_METHOD(SingleDivisionOperation)
        {
            vector<Token> postfix = {
                {"8", 0, TokenType::NUMBER},
                {"2", 2, TokenType::NUMBER},
                {"/", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::DIV, "/");
            AssertNumber(root->left, 8, "8");
            AssertNumber(root->right, 2, "2");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 9. Одиночная операция возведения в степень
        TEST_METHOD(SinglePowerOperation)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"^", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::POW, "^");
            AssertNumber(root->left, 2, "2");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 10. Одиночная унарная операция
        TEST_METHOD(SingleUnaryOperation)
        {
            vector<Token> postfix = {
                {"5", 1, TokenType::NUMBER},
                {"~", 0, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertUnary(root, "~");
            AssertNumber(root->left, 5, "5");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 11. Унарный минус перед подвыражением
        TEST_METHOD(UnaryMinusBeforeSubexpression)
        {
            vector<Token> postfix = {
                {"2", 2, TokenType::NUMBER},
                {"3", 4, TokenType::NUMBER},
                {"+", 3, TokenType::OPERATOR},
                {"~", 0, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertUnary(root, "~");
            AssertBinary(root->left, ExprNodeType::ADD, "+");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->left->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 12. Несколько унарных минусов подряд
        TEST_METHOD(SeveralUnaryMinuses)
        {
            vector<Token> postfix = {
                {"5", 2, TokenType::NUMBER},
                {"~", 1, TokenType::OPERATOR},
                {"~", 0, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertUnary(root, "~");
            AssertUnary(root->left, "~");
            AssertNumber(root->left->left, 5, "5");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 13. Бинарная операция с унарным правым операндом
        TEST_METHOD(BinaryOperationWithUnaryRightOperand)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 3, TokenType::NUMBER},
                {"~", 2, TokenType::OPERATOR},
                {"*", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertNumber(root->left, 2, "2");
            AssertUnary(root->right, "~");
            AssertNumber(root->right->left, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 14. Бинарная операция с унарным левым операндом
        TEST_METHOD(BinaryOperationWithUnaryLeftOperand)
        {
            vector<Token> postfix = {
                {"2", 1, TokenType::NUMBER},
                {"~", 0, TokenType::OPERATOR},
                {"3", 3, TokenType::NUMBER},
                {"+", 2, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertUnary(root->left, "~");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 15. Левое подвыражение внутри бинарной операции
        TEST_METHOD(LeftSubexpressionInsideBinaryOperation)
        {
            vector<Token> postfix = {
                {"2", 1, TokenType::NUMBER},
                {"3", 3, TokenType::NUMBER},
                {"+", 2, TokenType::OPERATOR},
                {"4", 6, TokenType::NUMBER},
                {"*", 5, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::MUL, "*");
            AssertBinary(root->left, ExprNodeType::ADD, "+");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->left->right, 3, "3");
            AssertNumber(root->right, 4, "4");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 16. Правое подвыражение внутри бинарной операции
        TEST_METHOD(RightSubexpressionInsideBinaryOperation)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"4", 4, TokenType::NUMBER},
                {"*", 3, TokenType::OPERATOR},
                {"+", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertNumber(root->left, 2, "2");
            AssertBinary(root->right, ExprNodeType::MUL, "*");
            AssertNumber(root->right->left, 3, "3");
            AssertNumber(root->right->right, 4, "4");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 17. Выражение с несколькими переменными
        TEST_METHOD(ExpressionWithSeveralVariables)
        {
            vector<Token> postfix = {
                {"a", 0, TokenType::VARIABLE},
                {"b", 2, TokenType::VARIABLE},
                {"c", 4, TokenType::VARIABLE},
                {"*", 3, TokenType::OPERATOR},
                {"+", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertVariable(root->left, "a");
            AssertBinary(root->right, ExprNodeType::MUL, "*");
            AssertVariable(root->right->left, "b");
            AssertVariable(root->right->right, "c");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 18. Цепочка делений с сохранением порядка операндов
        TEST_METHOD(DivisionChainOperandOrder)
        {
            vector<Token> postfix = {
                {"9", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"/", 1, TokenType::OPERATOR},
                {"3", 4, TokenType::NUMBER},
                {"/", 3, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::DIV, "/");
            AssertBinary(root->left, ExprNodeType::DIV, "/");
            AssertNumber(root->left->left, 9, "9");
            AssertNumber(root->left->right, 3, "3");
            AssertNumber(root->right, 3, "3");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 19. Цепочка возведения в степень с сохранением порядка операндов
        TEST_METHOD(PowerChainOperandOrder)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"^", 1, TokenType::OPERATOR},
                {"2", 4, TokenType::NUMBER},
                {"^", 3, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::POW, "^");
            AssertBinary(root->left, ExprNodeType::POW, "^");
            AssertNumber(root->left->left, 2, "2");
            AssertNumber(root->left->right, 3, "3");
            AssertNumber(root->right, 2, "2");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 20. Сложное выражение со всеми бинарными операциями
        TEST_METHOD(ComplexExpressionWithAllBinaryOperations)
        {
            vector<Token> postfix = {
                {"2", 1, TokenType::NUMBER},
                {"3", 3, TokenType::NUMBER},
                {"^", 2, TokenType::OPERATOR},
                {"8", 5, TokenType::NUMBER},
                {"4", 7, TokenType::NUMBER},
                {"/", 6, TokenType::OPERATOR},
                {"+", 4, TokenType::OPERATOR},
                {"5", 11, TokenType::NUMBER},
                {"1", 13, TokenType::NUMBER},
                {"-", 12, TokenType::OPERATOR},
                {"*", 9, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::MUL, "*");

            AssertBinary(root->left, ExprNodeType::ADD, "+");
            AssertBinary(root->left->left, ExprNodeType::POW, "^");
            AssertNumber(root->left->left->left, 2, "2");
            AssertNumber(root->left->left->right, 3, "3");

            AssertBinary(root->left->right, ExprNodeType::DIV, "/");
            AssertNumber(root->left->right->left, 8, "8");
            AssertNumber(root->left->right->right, 4, "4");

            AssertBinary(root->right, ExprNodeType::SUB, "-");
            AssertNumber(root->right->left, 5, "5");
            AssertNumber(root->right->right, 1, "1");

            Assert::AreEqual(size_t(0), errors.size());
        }

        // 21. Деление на ноль не проверяется в buildTree
        TEST_METHOD(DivisionByZeroIsNotCheckedInBuildTree)
        {
            vector<Token> postfix = {
                {"5", 0, TokenType::NUMBER},
                {"0", 2, TokenType::NUMBER},
                {"/", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::DIV, "/");
            AssertNumber(root->left, 5, "5");
            AssertNumber(root->right, 0, "0");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 22. Необъявленная переменная не проверяется в buildTree
        TEST_METHOD(UnknownVariableIsNotCheckedInBuildTree)
        {
            vector<Token> postfix = {
                {"unknown", 0, TokenType::VARIABLE},
                {"2", 8, TokenType::NUMBER},
                {"+", 7, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            AssertBinary(root, ExprNodeType::ADD, "+");
            AssertVariable(root->left, "unknown");
            AssertNumber(root->right, 2, "2");
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 23. Бинарная операция без обоих операндов
        TEST_METHOD(BinaryOperationWithoutBothOperands)
        {
            vector<Token> postfix = {
                {"+", 0, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 0, "+");
        }

        // 24. Бинарная операция без одного операнда
        TEST_METHOD(BinaryOperationWithoutOneOperand)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"*", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 1, "*");
        }

        // 25. Унарная операция без операнда
        TEST_METHOD(UnaryOperationWithoutOperand)
        {
            vector<Token> postfix = {
                {"~", 0, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 0, "~");
        }

        // 26. Лишний операнд после корректного подвыражения
        TEST_METHOD(ExtraOperandAfterCorrectSubexpression)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"+", 1, TokenType::OPERATOR},
                {"4", 4, TokenType::NUMBER}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 4, "4");
        }

        // 27. Выражение состоит только из операндов
        TEST_METHOD(ExpressionOnlyOperands)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"4", 4, TokenType::NUMBER}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 4, "4");
        }

        // 28. Два готовых поддерева не соединены оператором
        TEST_METHOD(TwoReadySubtreesWithoutOperator)
        {
            vector<Token> postfix = {
                {"2", 0, TokenType::NUMBER},
                {"3", 2, TokenType::NUMBER},
                {"+", 1, TokenType::OPERATOR},
                {"4", 4, TokenType::NUMBER},
                {"5", 6, TokenType::NUMBER},
                {"*", 5, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERATOR, 5, "*");
        }

        // 29. Выражение, состоящее из 100 операций
        TEST_METHOD(ExpressionWith100Operations)
        {
            vector<Token> postfix = MakePowerPostfix(100);
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNotNull(root);
            Assert::IsTrue(ExprNodeType::POW == root->type);
            Assert::AreEqual(201, CountNodes(root));
            Assert::AreEqual(100, CountOperationNodes(root));
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 30. Выражение, состоящее из 101 операции, количество операций не проверяется в buildTree
        TEST_METHOD(ExpressionWith101Operations)
        {
            vector<Token> postfix = MakePowerPostfix(101);
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNotNull(root);
            Assert::IsTrue(ExprNodeType::POW == root->type);
            Assert::AreEqual(203, CountNodes(root));
            Assert::AreEqual(101, CountOperationNodes(root));
            Assert::AreEqual(size_t(0), errors.size());
        }
    };
}