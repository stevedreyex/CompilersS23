#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 const char *name, Variable *type,
                 AstNode *constant_value);
    ~VariableNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCString();
    const char *getNameCType();

  private:
    // TODO: variable name, type, constant value
    Variable *type;
    const char *name;
    AstNode *constant_value;
};

#endif
