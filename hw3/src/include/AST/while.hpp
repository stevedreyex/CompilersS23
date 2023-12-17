#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              AstNode* expr_node, AstNode* comp_stmt_node
              /* TODO: expression, compound statement */);
    ~WhileNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: expression, compound statement
    AstNode* comp_stmt_node;
    AstNode* expr_node;
};

#endif
