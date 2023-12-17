#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                 AstNode* declaration_node, 
                 AstNode* assignment_node,
                 AstNode* expression_node, 
                 AstNode* compound_statement_node)
    : AstNode{line, col}, declaration_node(declaration_node), assignment_node(assignment_node),
      expression_node(expression_node), compound_statement_node(compound_statement_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declaration_node != NULL){
        declaration_node->accept(p_visitor);
    }
    if(assignment_node != NULL){
        assignment_node->accept(p_visitor);
    }
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
    if(compound_statement_node != NULL){
        compound_statement_node->accept(p_visitor);
    }
}