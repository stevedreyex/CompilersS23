#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H

#include <cstdint>
#include <vector>
#include <visitor/AstNodeVisitor.hpp>
#include <AST/AstDumper.hpp>
#include <string>

class AstNodeVisitor;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

class AstNode {
  protected:
    Location location;

  public:
    virtual ~AstNode() = 0;
    AstNode(const uint32_t line, const uint32_t col);

    const Location &getLocation() const;

    virtual void print() = 0;
    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor){};
};


enum class Boolean_type{
  booltype_True,
  booltype_False
};

enum class Datatype{
  INT,
  REAL_,
  STR,
  BOOL,
  VOID
};

enum class Operator{
  OP_ASSIGN,
  OP_OR,
  OP_AND,
  OP_NOT,
  OP_LESS,
  OP_LESS_OR_EQUAL,
  OP_EQUAL,
  OP_GREATER,
  OP_GREATER_OR_EQUAL,
  OP_NOT_EQUAL,
  OP_PLUS,
  OP_MINUS,
  OP_MULTIPLY,
  OP_DEVIDE,
  OP_MOD
};

struct Id_information{
  uint32_t line_number;
  uint32_t col_number;
  char* name;
};

struct Op_infos {
  Operator operator_type;
};
struct Variable {
  uint32_t line_number;
  uint32_t col_number;
  Datatype type;
  int int_t;
  double real_t;
  char* str_t;
  Boolean_type bool_t;
  std::vector<int> arr_num;
};

struct Node_Type{
  AstNode* ast_node;
  Variable* type;
  int cnt;
};

#endif
