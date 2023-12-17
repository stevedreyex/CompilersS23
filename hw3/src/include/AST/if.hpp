#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
           AstNode* expression_node, AstNode* if_compound_statement_node, AstNode* else_compound_statement_node
           /* TODO: expression, compound statement, compound statement */);
    ~IfNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: expression, compound statement, compound statement
    AstNode* if_compound_statement_node;
    AstNode* else_compound_statement_node;
    AstNode* expression_node;
};

#endif
