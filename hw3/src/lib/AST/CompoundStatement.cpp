#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector <AstNode*> *declaration_list,
                                             std::vector <AstNode*> *statement_list)
    : AstNode{line, col}, declaration_list(declaration_list), statement_list(statement_list) {}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO

    if(declaration_list != NULL){
        for (auto &declaration_list : *declaration_list) {
            declaration_list->accept(p_visitor);
        }
    }

    if(statement_list != NULL){
        for (auto &statement_list : *statement_list) {
            statement_list->accept(p_visitor);
        }
    }
}
