#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 const char *name, 
                 std::vector <AstNode*> *declaration_list, 
                 std::vector <Variable*> *parameter_list,
                 Variable *return_type, 
                 AstNode *compuound_statement);
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCString();
    const char *getNameCType();
    std::string getNameCParam();

  private:
    // TODO: name, declarations, return type, compound statement
    const char *name;
    Variable *return_type;
    AstNode *compuound_statement;
    std::vector <Variable*> *parameter_list;
    std::vector <AstNode*> *declaration_list;
};

#endif
