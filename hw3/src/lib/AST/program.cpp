#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *name,const char *return_type,
                         std::vector <AstNode*> *declaration_list,
                         std::vector <AstNode*> *function_list,
                         AstNode *body)
    : AstNode{line, col}, name(name), return_type(return_type),
      declaration_list(declaration_list), function_list(function_list), body(body) {}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print()
{
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
    if(declaration_list != NULL){
        for (auto &declaration_list : *declaration_list) {
            declaration_list->accept(p_visitor);
        }
    }

    if(function_list != NULL){
        for (auto &function_list : *function_list) {
            function_list->accept(p_visitor);
        }
    }

    body->accept(p_visitor);
}
