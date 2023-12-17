#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector <AstNode*> *variable_list)
    : AstNode{line, col}, variable_list(variable_list) {}

// TODO
//DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

// void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(variable_list != NULL){
        for (auto &variable_list : *variable_list) {
            variable_list->accept(p_visitor);
        }
    }
}