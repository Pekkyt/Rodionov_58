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
            node->nodeId = 0;
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
            node->nodeId = 0;
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
            node->nodeId = 0;
            node->token = "~";

            return node;
        }

        ExprNode* CreateBinary(ExprNodeType type,
            const string& token,
            ExprNode* left,
            ExprNode* right)
        {
            ExprNode* node = new ExprNode(0.0);

            node->value = 0;
            node->type = type;
            node->left = left;
            node->right = right;
            node->nodeId = 0;
            node->token = token;

            return node;
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

        void AssertDouble(double expected, double actual)
        {
            Assert::AreEqual(expected, actual, 0.000001);
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
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(123, result);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 3. Дерево из одного вещественного числа
        TEST_METHOD(OneRealNumber)
        {
            ExprNode* root = CreateNumber(12.34, "12.34");
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(12.34, result);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 4. Дерево из одной объявленной переменной
        TEST_METHOD(OneDeclaredVariable)
        {
            ExprNode* root = CreateVariable("x");
            vector<Error> errors;
            map<string, double> variables = {
                {"x", 7}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(7, result);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 5. Дерево из одной необъявленной переменной
        TEST_METHOD(OneUnknownVariable)
        {
            ExprNode* root = CreateVariable("x");
            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::UNKNOWN_VARIABLE, -1, "x");

            delete root;
        }

        // 6. Операция сложения
        TEST_METHOD(AdditionOperation)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateNumber(2, "2"),
                CreateNumber(3, "3")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(5, result);
            AssertDouble(5, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 7. Операция вычитания
        TEST_METHOD(SubtractionOperation)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::SUB,
                "-",
                CreateNumber(5, "5"),
                CreateNumber(2, "2")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(3, result);
            AssertDouble(3, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 8. Операция умножения
        TEST_METHOD(MultiplicationOperation)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::MUL,
                "*",
                CreateNumber(4, "4"),
                CreateNumber(6, "6")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(24, result);
            AssertDouble(24, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 9. Операция деления
        TEST_METHOD(DivisionOperation)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(8, "8"),
                CreateNumber(2, "2")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(4, result);
            AssertDouble(4, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 10. Деление с вещественным результатом
        TEST_METHOD(DivisionWithRealResult)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(10, "10"),
                CreateNumber(4, "4")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(2.5, result);
            AssertDouble(2.5, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 11. Операция возведения в степень
        TEST_METHOD(PowerOperation)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::POW,
                "^",
                CreateNumber(2, "2"),
                CreateNumber(3, "3")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(8, result);
            AssertDouble(8, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 12. Унарный минус перед числом
        TEST_METHOD(UnaryMinusBeforeNumber)
        {
            ExprNode* root = CreateUnary(CreateNumber(5, "5"));

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(-5, result);
            AssertDouble(-5, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 13. Несколько унарных минусов подряд
        TEST_METHOD(SeveralUnaryMinuses)
        {
            ExprNode* root = CreateUnary(
                CreateUnary(
                    CreateNumber(5, "5")
                )
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(5, result);
            AssertDouble(5, root->value);
            AssertDouble(-5, root->left->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 14. Деление на ноль
        TEST_METHOD(DivisionByZero)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(5, "5"),
                CreateNumber(0, "0")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::DIVISION_BY_ZERO, -1, "");

            delete root;
        }

        // 15. Деление на выражение, равное нулю
        TEST_METHOD(DivisionByExpressionEqualZero)
        {
            ExprNode* right = CreateBinary(
                ExprNodeType::SUB,
                "-",
                CreateNumber(2, "2"),
                CreateNumber(2, "2")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(5, "5"),
                right
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::DIVISION_BY_ZERO, -1, "");

            delete root;
        }

        // 16. Сложное выражение со скобками
        TEST_METHOD(ComplexExpressionWithBrackets)
        {
            ExprNode* left = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateNumber(2, "2"),
                CreateNumber(3, "3")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::MUL,
                "*",
                left,
                CreateNumber(4, "4")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(20, result);
            AssertDouble(20, root->value);
            AssertDouble(5, root->left->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 17. Правое подвыражение внутри бинарной операции
        TEST_METHOD(RightSubexpressionInsideBinaryOperation)
        {
            ExprNode* right = CreateBinary(
                ExprNodeType::MUL,
                "*",
                CreateNumber(3, "3"),
                CreateNumber(4, "4")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateNumber(2, "2"),
                right
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(14, result);
            AssertDouble(14, root->value);
            AssertDouble(12, root->right->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 18. Цепочка делений с сохранением порядка
        TEST_METHOD(DivisionChainOrder)
        {
            ExprNode* left = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(9, "9"),
                CreateNumber(3, "3")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                left,
                CreateNumber(3, "3")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(1, result);
            AssertDouble(1, root->value);
            AssertDouble(3, root->left->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 19. Цепочка возведения в степень с сохранением порядка
        TEST_METHOD(PowerChainOrder)
        {
            ExprNode* left = CreateBinary(
                ExprNodeType::POW,
                "^",
                CreateNumber(2, "2"),
                CreateNumber(3, "3")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::POW,
                "^",
                left,
                CreateNumber(2, "2")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(64, result);
            AssertDouble(64, root->value);
            AssertDouble(8, root->left->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 20. Приоритет унарного минуса выше степени уже отражён в дереве
        TEST_METHOD(UnaryMinusPriorityReflectedInTree)
        {
            ExprNode* left = CreateUnary(CreateNumber(2, "2"));

            ExprNode* root = CreateBinary(
                ExprNodeType::POW,
                "^",
                left,
                CreateNumber(3, "3")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(-8, result);
            AssertDouble(-8, root->value);
            AssertDouble(-2, root->left->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 21. Выражение с вещественными числами
        TEST_METHOD(ExpressionWithRealNumbers)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateNumber(1.5, "1.5"),
                CreateNumber(2.25, "2.25")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(3.75, result);
            AssertDouble(3.75, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 22. Выражение с несколькими переменными
        TEST_METHOD(ExpressionWithSeveralVariables)
        {
            ExprNode* right = CreateBinary(
                ExprNodeType::MUL,
                "*",
                CreateVariable("b"),
                CreateVariable("c")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateVariable("a"),
                right
            );

            vector<Error> errors;
            map<string, double> variables = {
                {"a", 2},
                {"b", 3},
                {"c", 4}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(14, result);
            AssertDouble(14, root->value);
            AssertDouble(12, root->right->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 23. Повторное использование одной переменной
        TEST_METHOD(ReusingSameVariable)
        {
            ExprNode* right = CreateBinary(
                ExprNodeType::MUL,
                "*",
                CreateVariable("x"),
                CreateNumber(2, "2")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateVariable("x"),
                right
            );

            vector<Error> errors;
            map<string, double> variables = {
                {"x", 5}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(15, result);
            AssertDouble(15, root->value);
            AssertDouble(10, root->right->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 24. Необъявленная переменная внутри выражения
        TEST_METHOD(UnknownVariableInsideExpression)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                CreateNumber(2, "2"),
                CreateVariable("x")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::UNKNOWN_VARIABLE, -1, "x");

            delete root;
        }

        // 25. Необъявленная переменная в делителе
        TEST_METHOD(UnknownVariableInDivisor)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(5, "5"),
                CreateVariable("x")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::UNKNOWN_VARIABLE, -1, "x");

            delete root;
        }

        // 26. Сложное выражение с переменными, унарным минусом и степенью
        TEST_METHOD(ComplexExpressionWithVariablesUnaryMinusAndPower)
        {
            ExprNode* unary = CreateUnary(CreateNumber(2.5, "2.5"));

            ExprNode* sum = CreateBinary(
                ExprNodeType::ADD,
                "+",
                unary,
                CreateVariable("b")
            );

            ExprNode* power = CreateBinary(
                ExprNodeType::POW,
                "^",
                sum,
                CreateNumber(3, "3")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::MUL,
                "*",
                CreateVariable("a"),
                power
            );

            vector<Error> errors;
            map<string, double> variables = {
                {"a", 2},
                {"b", 4.5}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(16, result);
            AssertDouble(16, root->value);
            AssertDouble(8, root->right->value);
            AssertDouble(2, root->right->left->value);
            AssertDouble(-2.5, root->right->left->left->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 27. Сложное выражение с делением и несколькими операциями
        TEST_METHOD(ComplexExpressionWithDivisionAndSeveralOperations)
        {
            ExprNode* left = CreateBinary(
                ExprNodeType::POW,
                "^",
                CreateNumber(2, "2"),
                CreateNumber(3, "3")
            );

            ExprNode* right = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(10, "10"),
                CreateNumber(4, "4")
            );

            ExprNode* root = CreateBinary(
                ExprNodeType::ADD,
                "+",
                left,
                right
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(10.5, result);
            AssertDouble(10.5, root->value);
            AssertDouble(8, root->left->value);
            AssertDouble(2.5, root->right->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 28. Деление на значение переменной, равное нулю
        TEST_METHOD(DivisionByVariableEqualZero)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(10, "10"),
                CreateVariable("x")
            );

            vector<Error> errors;
            map<string, double> variables = {
                {"x", 0}
            };

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            Assert::AreEqual(size_t(1), errors.size());
            AssertError(errors[0], ErrorType::DIVISION_BY_ZERO, -1, "");

            delete root;
        }

        // 29. Ноль в числителе при делении
        TEST_METHOD(ZeroInNumerator)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::DIV,
                "/",
                CreateNumber(0, "0"),
                CreateNumber(5, "5")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(0, result);
            AssertDouble(0, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }

        // 30. Возведение в нулевую степень
        TEST_METHOD(PowerWithZeroExponent)
        {
            ExprNode* root = CreateBinary(
                ExprNodeType::POW,
                "^",
                CreateNumber(5, "5"),
                CreateNumber(0, "0")
            );

            vector<Error> errors;
            map<string, double> variables;

            double result = calculate(root, variables, errors);

            AssertDouble(1, result);
            AssertDouble(1, root->value);
            Assert::AreEqual(size_t(0), errors.size());

            delete root;
        }
    };
}