#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include "AST/PType.hpp"
#include <stack>
#include <string>
#include <vector>
#include <cstring>

// 6 kinds of symbols with respective strings: program, function, parameter, variable, loop_var, and constant
enum Kind{
  PROG,
  FUNC,
  PARA,
  VARS,
  LOOPVAR,
  CONST
};
class SymbolEntry {
public:
    SymbolEntry(std::string * names, Kind kind, int level, std::string * type, std::string * attribute);
    // Variable names
    std::string * names;
    // Kind
    Kind kind;
    // Level
    int level;
    // ...
    std::string * type;
    std::string * attribute;
    int error = 0;
    int dim = 0;
    PTypeSharedPtr ptype;
};

class SymbolTable {
public:
    void addSymbol(SymbolEntry *symbolEntry);
    // other methods
    std::vector<SymbolEntry *> entries;
    std::string source;
};

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
      // TODO: something like symbol manager (manage symbol tables)
      //       context manager, return type manager
    


  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    class SymbolManager {
        public:
            SymbolTable * top();
            void pushScope(SymbolTable *new_scope);
            void popScope();
            // other methods
            std::stack <SymbolTable *> tables;
    };
    SymbolManager symbolManager;
    SymbolTable *current_table;
    SymbolEntry *current_entry;
    int current_error = 0;
    // A nested class for a Sema instance to manage Symbol tables


};

#endif
