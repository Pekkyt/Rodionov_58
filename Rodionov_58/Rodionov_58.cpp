#pragma warning(disable : 4996)
#include <iostream>
#include "Header.h"

vector<Token> tokenize(const string& expression, vector<Error>& errors)
{
    return {};
}

vector<Token> toPostfix(const vector<Token>& tokens, vector<Error>& errors)
{
    return {};
}

ExprNode* buildTree(const vector<Token>& postfix, vector<Error>& errors)
{
    return nullptr;
}

ExprNode* parseExpression(const string& expression, vector<Error>& errors)
{
    bool onlySpaces = true;
    for (char ch : expression)
    {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r')
        {
            onlySpaces = false;
            break;
        }
    }
    if (expression.empty() || onlySpaces)
    {
        errors.push_back(Error{ ErrorType::MISSING_OPERATOR, -1, "" });
        return nullptr;
    }
    return nullptr;
}

ExprNode::ExprNode(double val)
{
    value = val;
    type = ExprNodeType::NUMBER;
    left = nullptr;
    right = nullptr;
    nodeId = 0;
    token = "";
}

ExprNode::ExprNode(ExprNodeType type, ExprNode* left, ExprNode* right)
{
    value = 0;
    this->type = type;
    this->left = left;
    this->right = right;
    nodeId = 0;
    token = "";
}

ExprNode::~ExprNode()
{
    delete left;
    delete right;
}

double calculate(ExprNode* node, const map<string, double>& variables, vector<Error>& errors)
{
    if (node == nullptr)
    {
        return 0;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    return 0;
}
