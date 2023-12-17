#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"

class AssignmentNode : public AstNode {
  public:
    AssignmentNode( const uint32_t line,
                    const uint32_t col, 
                    AstNode* variable_reference_node,   //3
                    AstNode* expression_node            //4
                   /* TODO: variable reference, expression */);
    ~AssignmentNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: variable reference, expression
      AstNode* expression_node;
      AstNode* variable_reference_node;
};

#endif
