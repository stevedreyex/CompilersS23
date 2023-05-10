#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                                AstNode* variable_reference_node,   //3
                                AstNode* expression_node)           //4
    : AstNode{line, col}, variable_reference_node(variable_reference_node), expression_node(expression_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

// void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    
    if(variable_reference_node != NULL){
        variable_reference_node->accept(p_visitor);
    }

    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
}

void AssignmentNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}