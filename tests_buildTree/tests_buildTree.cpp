#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_buildTree
{
    TEST_CLASS(tests_buildTree)
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
            vector<Token> postfix = { {"123", 0, TokenType::NUMBER} };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = CreateNumber(123, "123");

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 3. Выражение из одного вещественного числа
        TEST_METHOD(OneRealNumber)
        {
            vector<Token> postfix = { {"12.34", 0, TokenType::NUMBER} };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = CreateNumber(12.34, "12.34");

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 4. Выражение из одной переменной
        TEST_METHOD(OneVariable)
        {
            vector<Token> postfix = { {"x", 0, TokenType::VARIABLE} };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = CreateVariable("x");

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::SUB, "-", CreateNumber(5, "5"), CreateNumber(2, "2"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::MUL, "*", CreateNumber(4, "4"), CreateNumber(6, "6"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(8, "8"), CreateNumber(2, "2"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateUnary(CreateNumber(5, "5"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateUnary(CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3")));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateUnary(CreateUnary(CreateNumber(5, "5")));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::MUL, "*", CreateNumber(2, "2"), CreateUnary(CreateNumber(3, "3")));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateUnary(CreateNumber(2, "2")), CreateNumber(3, "3"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
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

        // 18. Выражение с вещественными числами
        TEST_METHOD(ExpressionWithRealNumbers)
        {
            vector<Token> postfix = {
                {"1.5", 0, TokenType::NUMBER},
                {"2.25", 4, TokenType::NUMBER},
                {"+", 3, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(1.5, "1.5"), CreateNumber(2.25, "2.25"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 19. Цепочка делений с сохранением порядка операндов
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

        // 20. Цепочка возведения в степень с сохранением порядка операндов
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

        // 21. Сложное выражение со всеми бинарными операциями
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
            ExprNode* expected = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateBinary(
                    ExprNodeType::ADD, "+",
                    CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3")),
                    CreateBinary(ExprNodeType::DIV, "/", CreateNumber(8, "8"), CreateNumber(4, "4"))
                ),
                CreateBinary(ExprNodeType::SUB, "-", CreateNumber(5, "5"), CreateNumber(1, "1"))
            );

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 22. Деление на ноль не проверяется в buildTree
        TEST_METHOD(DivisionByZeroIsNotCheckedInBuildTree)
        {
            vector<Token> postfix = {
                {"5", 0, TokenType::NUMBER},
                {"0", 2, TokenType::NUMBER},
                {"/", 1, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(5, "5"), CreateNumber(0, "0"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 23. Необъявленная переменная не проверяется в buildTree
        TEST_METHOD(UnknownVariableIsNotCheckedInBuildTree)
        {
            vector<Token> postfix = {
                {"unknown", 0, TokenType::VARIABLE},
                {"2", 8, TokenType::NUMBER},
                {"+", 7, TokenType::OPERATOR}
            };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateVariable("unknown"), CreateNumber(2, "2"));

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 24. Бинарная операция без обоих операндов
        TEST_METHOD(BinaryOperationWithoutBothOperands)
        {
            vector<Token> postfix = { {"+", 0, TokenType::OPERATOR} };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 0, "+");
        }

        // 25. Бинарная операция без одного операнда
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

        // 26. Унарная операция без операнда
        TEST_METHOD(UnaryOperationWithoutOperand)
        {
            vector<Token> postfix = { {"~", 0, TokenType::OPERATOR} };
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);

            Assert::IsNull(root);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::MISSING_OPERAND, 0, "~");
        }

        // 27. Лишний операнд после корректного подвыражения
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

        // 28. Выражение состоит только из операндов
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

        // 29. Два готовых поддерева не соединены оператором
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

        // 30. Выражение, состоящее из 100 операций
        TEST_METHOD(ExpressionWith100Operations)
        {
            vector<Token> postfix = MakePowerPostfix(100);
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = MakeExpectedPowerTree(100);

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 31. Выражение, состоящее из 101 операции, количество операций не проверяется в buildTree
        TEST_METHOD(ExpressionWith101Operations)
        {
            vector<Token> postfix = MakePowerPostfix(101);
            vector<Error> errors;

            ExprNode* root = buildTree(postfix, errors);
            ExprNode* expected = MakeExpectedPowerTree(101);

            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

    };
}
