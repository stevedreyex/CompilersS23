#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>

class ProgramNode final : public AstNode {
  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *name, const char *return_type,
                std::vector <AstNode*> *declaration_list,
                std::vector <AstNode*> *function_list,
                AstNode *body
                );
    // visitor pattern version: 
    void print() override;
    const char *getNameCString() const { return name.c_str(); }
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    const std::string name;
    const std::string return_type;
    std::vector <AstNode*> *declaration_list;
    std::vector <AstNode*> *function_list;
    AstNode *body;
    // TODO: return type, declarations, functions, compound statement
};

#endif
