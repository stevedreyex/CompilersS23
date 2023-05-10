#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char* name, std::vector<AstNode*> *expression_node)
    : ExpressionNode{line, col}, name(name), expression_node(expression_node) {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        for (auto &expression_node : *expression_node) {
            expression_node->accept(p_visitor);
        }
    }
}

const char *VariableReferenceNode::getNameCString(){
    return name;
}
