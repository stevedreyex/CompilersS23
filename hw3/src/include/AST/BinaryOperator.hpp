#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                        Op_infos *op, 
                        AstNode *left_operand, 
                        AstNode *right_operand
                       /* TODO: operator, expressions */);
    ~BinaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCType();

  private:
    // TODO: operator, expressions
    AstNode *left_operand;
    AstNode *right_operand;
    Op_infos *op;
};

#endif
