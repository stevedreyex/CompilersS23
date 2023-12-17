#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"

class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char* name, std::vector<AstNode*> *expression_node
                          /* TODO: name */);
    // array reference
    // VariableReferenceNode(const uint32_t line, const uint32_t col
    //                       /* TODO: name, expressions */);
    ~VariableReferenceNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCString();

  private:
    // TODO: variable name, expressions
    const char* name;
    std::vector<AstNode*> *expression_node;
};

#endif
