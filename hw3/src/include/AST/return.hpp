#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"

class ReturnNode : public AstNode {
  public:
    ReturnNode(const uint32_t line, const uint32_t col,
               AstNode* expression_node
               /* TODO: expression */);
    ~ReturnNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: expression
    AstNode* expression_node;
};

#endif
