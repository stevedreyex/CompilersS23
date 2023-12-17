#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       AstNode* expression_node)
    : AstNode{line, col}, expression_node(expression_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

// void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
}