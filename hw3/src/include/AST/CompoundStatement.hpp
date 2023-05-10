#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          std::vector <AstNode*> *declaration_list,
                          std::vector <AstNode*> *statement_list
                          /* TODO: declarations, statements */);
    ~CompoundStatementNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;

private:
    std::vector <AstNode*> *statement_list;
    std::vector <AstNode*> *declaration_list;
// TODO: declarations, statements
};

#endif
