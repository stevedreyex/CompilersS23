#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     AstNode* expr_node, AstNode* comp_stmt_node)
    : AstNode{line, col}, expr_node(expr_node), comp_stmt_node(comp_stmt_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr_node != NULL){
        expr_node->accept(p_visitor);
    }
    if(comp_stmt_node != NULL){
        comp_stmt_node->accept(p_visitor);
    }
}