#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "AST/PType.hpp"
#include <string>

class ExpressionNode : public AstNode {
  public:
    ~ExpressionNode() = default;
    ExpressionNode(const uint32_t line, const uint32_t col)
        : AstNode{line, col} {}
    int ref_dim_mismatch = false;
    PTypeSharedPtr type;
    const char *getType(){
      return type->getPTypeCString();
    }

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
};

#endif
