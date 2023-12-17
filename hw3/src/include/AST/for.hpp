#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            AstNode* declaration_node, 
            AstNode* assignment_node,
            AstNode* expression_node, 
            AstNode* compound_statement_node
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: declaration, assignment, expression, compound statement
    AstNode* declaration_node;
    AstNode* compound_statement_node;
    AstNode* expression_node;
    AstNode* assignment_node;
};

#endif
