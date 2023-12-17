#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
               AstNode* expression_node, 
               AstNode* if_compound_statement_node, 
               AstNode* else_compound_statement_node)
    : AstNode{line, col}, expression_node(expression_node), 
      if_compound_statement_node(if_compound_statement_node), else_compound_statement_node(else_compound_statement_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

// void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
    if(if_compound_statement_node != NULL){
        if_compound_statement_node->accept(p_visitor);
    }
    if(else_compound_statement_node != NULL){
        else_compound_statement_node->accept(p_visitor);
    }
}