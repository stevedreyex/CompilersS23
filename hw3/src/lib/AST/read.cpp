#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col,
                   AstNode* variable_reference_node)
    : AstNode{line, col}, variable_reference_node(variable_reference_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

// void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(variable_reference_node != NULL){
        variable_reference_node->accept(p_visitor);
    }
}