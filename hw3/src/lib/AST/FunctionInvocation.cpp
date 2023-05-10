#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char* name, std::vector<AstNode*> *expression)
    : ExpressionNode{line, col}, name(name), expression(expression) {}

// TODO: You may use code snippets in AstDumper.cpp

const char *FunctionInvocationNode::getNameCString() const
{
    return name;
}
void FunctionInvocationNode::print() {}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression != NULL){
        for (auto &expression : *expression) {
            expression->accept(p_visitor);
        }
    }
}