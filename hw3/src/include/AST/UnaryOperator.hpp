#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      Op_infos *op, 
                      AstNode *operand
                      /* TODO: operator, expression */);
    ~UnaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCType();

  private:
    // TODO: operator, expression
    AstNode *operand;
    Op_infos *op;
};

#endif
