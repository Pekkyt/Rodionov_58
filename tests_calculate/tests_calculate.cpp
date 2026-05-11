#include "pch.h"
#include "CppUnitTest.h"
#include "../Rodionov_58/Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests_calculate
{
    TEST_CLASS(tests_calculate)
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

    public:

        // 1. Пустое дерево
        TEST_METHOD(EmptyTree)
        {
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(nullptr, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(0), errors.size());
        }

        // 2. Дерево из одного целого числа
        TEST_METHOD(OneIntegerNumber)
        {
            ExprNode* root = CreateNumber(123, "123");
            ExprNode* expected = CreateNumber(123, "123");
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(123, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 3. Дерево из одного вещественного числа
        TEST_METHOD(OneRealNumber)
        {
            ExprNode* root = CreateNumber(12.34, "12.34");
            ExprNode* expected = CreateNumber(12.34, "12.34");
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(12.34, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 4. Дерево из одной объявленной переменной
        TEST_METHOD(OneDeclaredVariable)
        {
            ExprNode* root = CreateVariable("x");
            ExprNode* expected = CreateVariable("x");
            vector<Error> errors;
            map<string, double> variables = { {"x", 7} };

            double result = calculate(root, variables, errors);

            AssertDouble(7, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 5. Дерево из одной необъявленной переменной
        TEST_METHOD(OneUnknownVariable)
        {
            ExprNode* root = CreateVariable("x");
            ExprNode* expected = CreateVariable("x");
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::UNKNOWN_VARIABLE, -1, "x");
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 6. Операция сложения
        TEST_METHOD(AdditionOperation)
        {
            ExprNode* root = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3"));
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3"));
            expected->value = 5;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(5, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 7. Операция вычитания
        TEST_METHOD(SubtractionOperation)
        {
            ExprNode* root = CreateBinary(ExprNodeType::SUB, "-", CreateNumber(5, "5"), CreateNumber(2, "2"));
            ExprNode* expected = CreateBinary(ExprNodeType::SUB, "-", CreateNumber(5, "5"), CreateNumber(2, "2"));
            expected->value = 3;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(3, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 8. Операция умножения
        TEST_METHOD(MultiplicationOperation)
        {
            ExprNode* root = CreateBinary(ExprNodeType::MUL, "*", CreateNumber(4, "4"), CreateNumber(6, "6"));
            ExprNode* expected = CreateBinary(ExprNodeType::MUL, "*", CreateNumber(4, "4"), CreateNumber(6, "6"));
            expected->value = 24;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(24, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 9. Операция деления
        TEST_METHOD(DivisionOperation)
        {
            ExprNode* root = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(8, "8"), CreateNumber(2, "2"));
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(8, "8"), CreateNumber(2, "2"));
            expected->value = 4;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(4, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 10. Деление с вещественным результатом
        TEST_METHOD(DivisionWithRealResult)
        {
            ExprNode* root = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(10, "10"), CreateNumber(4, "4"));
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(10, "10"), CreateNumber(4, "4"));
            expected->value = 2.5;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(2.5, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 11. Операция возведения в степень
        TEST_METHOD(PowerOperation)
        {
            ExprNode* root = CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3"));
            ExprNode* expected = CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3"));
            expected->value = 8;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(8, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 12. Унарный минус перед числом
        TEST_METHOD(UnaryMinusBeforeNumber)
        {
            ExprNode* root = CreateUnary(CreateNumber(5, "5"));
            ExprNode* expected = CreateUnary(CreateNumber(5, "5"));
            expected->value = -5;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(-5, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 13. Несколько унарных минусов подряд
        TEST_METHOD(SeveralUnaryMinuses)
        {
            ExprNode* root = CreateUnary(CreateUnary(CreateNumber(5, "5")));
            ExprNode* expected = CreateUnary(CreateUnary(CreateNumber(5, "5")));
            expected->left->value = -5;
            expected->value = 5;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(5, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 14. Деление на ноль
        TEST_METHOD(DivisionByZero)
        {
            ExprNode* root = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(5, "5"), CreateNumber(0, "0"));
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(5, "5"), CreateNumber(0, "0"));
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::DIVISION_BY_ZERO, -1, "");
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 15. Деление на выражение, равное нулю
        TEST_METHOD(DivisionByExpressionEqualZero)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV, "/",
                CreateNumber(5, "5"),
                CreateBinary(ExprNodeType::SUB, "-", CreateNumber(2, "2"), CreateNumber(2, "2"))
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::DIV, "/",
                CreateNumber(5, "5"),
                CreateBinary(ExprNodeType::SUB, "-", CreateNumber(2, "2"), CreateNumber(2, "2"))
            );
            expected->right->value = 0;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::DIVISION_BY_ZERO, -1, "");
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 16. Сложное выражение со скобками
        TEST_METHOD(ComplexExpressionWithBrackets)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateNumber(4, "4")
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateNumber(4, "4")
            );
            expected->left->value = 5;
            expected->value = 20;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(20, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 17. Правое подвыражение внутри бинарной операции
        TEST_METHOD(RightSubexpressionInsideBinaryOperation)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateNumber(2, "2"),
                CreateBinary(ExprNodeType::MUL, "*", CreateNumber(3, "3"), CreateNumber(4, "4"))
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateNumber(2, "2"),
                CreateBinary(ExprNodeType::MUL, "*", CreateNumber(3, "3"), CreateNumber(4, "4"))
            );
            expected->right->value = 12;
            expected->value = 14;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(14, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 18. Цепочка делений с сохранением порядка
        TEST_METHOD(DivisionChainOrder)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV, "/",
                CreateBinary(ExprNodeType::DIV, "/", CreateNumber(9, "9"), CreateNumber(3, "3")),
                CreateNumber(3, "3")
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::DIV, "/",
                CreateBinary(ExprNodeType::DIV, "/", CreateNumber(9, "9"), CreateNumber(3, "3")),
                CreateNumber(3, "3")
            );
            expected->left->value = 3;
            expected->value = 1;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(1, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 19. Цепочка возведения в степень с сохранением порядка
        TEST_METHOD(PowerChainOrder)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::POW, "^",
                CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateNumber(2, "2")
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::POW, "^",
                CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateNumber(2, "2")
            );
            expected->left->value = 8;
            expected->value = 64;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(64, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 20. Приоритет унарного минуса выше степени уже отражён в дереве
        TEST_METHOD(UnaryMinusPriorityReflectedInTree)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::POW, "^",
                CreateUnary(CreateNumber(2, "2")),
                CreateNumber(3, "3")
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::POW, "^",
                CreateUnary(CreateNumber(2, "2")),
                CreateNumber(3, "3")
            );
            expected->left->value = -2;
            expected->value = -8;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(-8, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 21. Выражение с вещественными числами
        TEST_METHOD(ExpressionWithRealNumbers)
        {
            ExprNode* root = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(1.5, "1.5"), CreateNumber(2.25, "2.25"));
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(1.5, "1.5"), CreateNumber(2.25, "2.25"));
            expected->value = 3.75;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(3.75, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 22. Выражение с несколькими переменными
        TEST_METHOD(ExpressionWithSeveralVariables)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateVariable("a"),
                CreateBinary(ExprNodeType::MUL, "*", CreateVariable("b"), CreateVariable("c"))
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateVariable("a"),
                CreateBinary(ExprNodeType::MUL, "*", CreateVariable("b"), CreateVariable("c"))
            );
            expected->right->value = 12;
            expected->value = 14;

            vector<Error> errors;
            map<string, double> variables = {
                {"a", 2},
                {"b", 3},
                {"c", 4}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(14, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 23. Повторное использование одной переменной
        TEST_METHOD(ReusingSameVariable)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateVariable("x"),
                CreateBinary(ExprNodeType::MUL, "*", CreateVariable("x"), CreateNumber(2, "2"))
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateVariable("x"),
                CreateBinary(ExprNodeType::MUL, "*", CreateVariable("x"), CreateNumber(2, "2"))
            );
            expected->right->value = 10;
            expected->value = 15;

            vector<Error> errors;
            map<string, double> variables = { {"x", 5} };

            double result = calculate(root, variables, errors);

            AssertDouble(15, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 24. Необъявленная переменная внутри выражения
        TEST_METHOD(UnknownVariableInsideExpression)
        {
            ExprNode* root = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateVariable("x"));
            ExprNode* expected = CreateBinary(ExprNodeType::ADD, "+", CreateNumber(2, "2"), CreateVariable("x"));
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::UNKNOWN_VARIABLE, -1, "x");
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 25. Необъявленная переменная в делителе
        TEST_METHOD(UnknownVariableInDivisor)
        {
            ExprNode* root = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(5, "5"), CreateVariable("x"));
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(5, "5"), CreateVariable("x"));
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::UNKNOWN_VARIABLE, -1, "x");
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 26. Сложное выражение с переменными, унарным минусом и степенью
        TEST_METHOD(ComplexExpressionWithVariablesUnaryMinusAndPower)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateVariable("a"),
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

            ExprNode* expected = CreateBinary(
                ExprNodeType::MUL, "*",
                CreateVariable("a"),
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
            expected->right->left->left->value = -2.5;
            expected->right->left->value = 2;
            expected->right->value = 8;
            expected->value = 16;

            vector<Error> errors;
            map<string, double> variables = {
                {"a", 2},
                {"b", 4.5}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(16, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 27. Сложное выражение с делением и несколькими операциями
        TEST_METHOD(ComplexExpressionWithDivisionAndSeveralOperations)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateBinary(ExprNodeType::DIV, "/", CreateNumber(10, "10"), CreateNumber(4, "4"))
            );

            ExprNode* expected = CreateBinary(
                ExprNodeType::ADD, "+",
                CreateBinary(ExprNodeType::POW, "^", CreateNumber(2, "2"), CreateNumber(3, "3")),
                CreateBinary(ExprNodeType::DIV, "/", CreateNumber(10, "10"), CreateNumber(4, "4"))
            );
            expected->left->value = 8;
            expected->right->value = 2.5;
            expected->value = 10.5;

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(10.5, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 28. Деление на значение переменной, равное нулю
        TEST_METHOD(DivisionByVariableEqualZero)
        {
            ExprNode* root = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(10, "10"), CreateVariable("x"));
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(10, "10"), CreateVariable("x"));
            vector<Error> errors;
            map<string, double> variables = { {"x", 0} };

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::DIVISION_BY_ZERO, -1, "");
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 29. Ноль в числителе при делении
        TEST_METHOD(ZeroInNumerator)
        {
            ExprNode* root = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(0, "0"), CreateNumber(5, "5"));
            ExprNode* expected = CreateBinary(ExprNodeType::DIV, "/", CreateNumber(0, "0"), CreateNumber(5, "5"));
            expected->value = 0;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

        // 30. Возведение в нулевую степень
        TEST_METHOD(PowerWithZeroExponent)
        {
            ExprNode* root = CreateBinary(ExprNodeType::POW, "^", CreateNumber(5, "5"), CreateNumber(0, "0"));
            ExprNode* expected = CreateBinary(ExprNodeType::POW, "^", CreateNumber(5, "5"), CreateNumber(0, "0"));
            expected->value = 1;
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(1, result);
            Assert::AreEqual(size_t(0), errors.size());
            AssertTreeEqual(root, expected);

            delete root;
            delete expected;
        }

    };
}
