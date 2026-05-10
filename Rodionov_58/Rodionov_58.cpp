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

ExprNode::~ExprNode()
{
    delete left;
    delete right;
}

int main(int argc, char* argv[])
{
    return 0;
}
