%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "AST/AstDumper.hpp"

#include <vector>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <errno.h>
#include <cstdio>
#include <cstdint>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    class AstNode;
    struct Variable;
    struct Node_Type;
    struct Op_infos;
    struct Id_information;
    enum class Datatype;
    enum class Operator;   
    enum class Boolean_type; 
    #include <vector>
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    int integer;
    double real;
    AstNode *node;
    Variable *variable;
    Node_Type *node_type;
    Boolean_type *bool_type;
    std::vector <AstNode*> *nodelist;
    std::vector <Variable*> *varlist;
    std::vector <Id_information> *idlist;
    std::vector <Node_Type*> *node_type_list;
};

%type <identifier> ProgramName ID STRING_LITERAL FunctionName
%type <idlist> IdList
%type <node> CompoundStatement Declaration FunctionDeclaration FunctionDefinition Function Statement Simple Condition While For Return FunctionCall Expression FunctionInvocation VariableReference ElseOrNot 
%type <nodelist> DeclarationList Statements StatementList Declarations Functions FunctionList Expressions ExpressionList ArrRefList ArrRefs
%type <variable> Type ScalarType ArrType IntegerAndReal StringAndBoolean ArrDecl LiteralConstant ReturnType
%type <integer> NegOrNot INT_LITERAL
%type <real> REAL_LITERAL
%type <bool_type> TRUE FALSE
%type <node_type> FormalArg
%type <node_type_list> FormalArgs FormalArgList

    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

%%

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column, 
                                $1, "void", $3, $4, $5);

        free($1);
    }
;

ProgramName:
    ID { $$ = $1; }
;

DeclarationList:
    Epsilon { $$ = NULL; }
    |
    Declarations { $$ = $1; }
;

Declarations:
    Declaration
    {
        $$ = new std::vector <AstNode*> ();
        $$->push_back($1);
    }
    |
    Declarations Declaration
    {
        $1->push_back($2);
        $$ = $1;
    }
;

FunctionList:
    Epsilon { $$ = NULL; }
    |
    Functions{ $$ = $1; }
;

Functions:
    Function
    {
        $$ = new std::vector <AstNode*>;
        $$->push_back($1);
    }
    |
    Functions Function
    {
        $1->push_back($2);
        $$ = $1;
    }
;

Function:
    FunctionDeclaration { $$ = $1; }
    |
    FunctionDefinition { $$ = $1; }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON
    {
        std::vector<Variable*> *param_list = new std::vector <Variable*>;
        std::vector <AstNode*> *decl_list = new std::vector <AstNode*>;

        if($3 != NULL){
            for(int i = 0; i < $3->size(); i++){ 
                for(int j = 0; j < $3->at(i)->cnt; j++){ 
                    Variable* var = new Variable;
                    var->type = $3->at(i)->type->type;
                    var->int_t = $3->at(i)->type->int_t;
                    var->real_t = $3->at(i)->type->real_t;
                    var->str_t = $3->at(i)->type->str_t;
                    var->bool_t = $3->at(i)->type->bool_t;
                    var->arr_num = $3->at(i)->type->arr_num;

                    param_list->push_back(var);
                }

                decl_list->push_back($3->at(i)->ast_node);
            }
        }

        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, decl_list, param_list, $5, NULL);    
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END
    {
        std::vector<Variable*> *param_list = new std::vector <Variable*>;
        std::vector <AstNode*> *decl_list = new std::vector <AstNode*>;

        if($3 != NULL){
            for(int i = 0; i < $3->size(); i++){    
                for(int j = 0; j < $3->at(i)->cnt; j++){ 
                    Variable* var = new Variable;
                    var->type = $3->at(i)->type->type;
                    var->int_t = $3->at(i)->type->int_t;
                    var->real_t = $3->at(i)->type->real_t;
                    var->str_t = $3->at(i)->type->str_t;
                    var->bool_t = $3->at(i)->type->bool_t;
                    var->arr_num = $3->at(i)->type->arr_num;

                    param_list->push_back(var);
                }
                decl_list->push_back($3->at(i)->ast_node);
            }
        }


        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, decl_list, param_list, $5, $6);  
    }
;

FunctionName:
    ID { $$ = $1; }
;

FormalArgList:
    Epsilon { $$ = NULL; }
    |
    FormalArgs{ $$ = $1; }
;

FormalArgs:
    FormalArg
    {
        $$ = new std::vector <Node_Type*>;
        $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg
    {
        $1->push_back($3);
        $$ = $1;
    }
;

FormalArg:
    IdList COLON Type
    {
        $$ = new Node_Type;
        $$->type = $3;
        $$->cnt = 0;

        std::vector<AstNode*> *var_list = new std::vector <AstNode*>;
        for(int i = 0; i < $1->size(); i++){
            Variable *var = new Variable;
            var->type = $3->type;
            var->int_t = $3->int_t;
            var->real_t = $3->real_t;
            var->str_t = $3->str_t;
            var->bool_t = $3->bool_t;
            var->arr_num = $3->arr_num;           

            VariableNode *var_node = new VariableNode($1->at(i).line_number, $1->at(i).col_number, $1->at(i).name, var, NULL); 
            var_list->push_back(var_node);
            $$->cnt++;
        }        
        
        $$->ast_node = new DeclNode(@1.first_line, @1.first_column, var_list); 
    }
;

IdList:
    ID
    {
        $$ = new std::vector <Id_information>;

        Id_information id_tmp;
        id_tmp.name = $1;
        id_tmp.line_number = @1.first_line;
        id_tmp.col_number = @1.first_column;
        
        $$->push_back(id_tmp);
    }
    |
    IdList COMMA ID
    {
        Id_information id_tmp;
        id_tmp.name = $3;
        id_tmp.line_number = @3.first_line;
        id_tmp.col_number = @3.first_column;

        $1->push_back(id_tmp);

        $$ = $1;
    }
;

ReturnType:
    COLON ScalarType { $$ = $2; }
    |
    Epsilon
    {
        $$ = new Variable;
        $$->type = Datatype::VOID;
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON
    {
        std::vector <AstNode*> *var_list = new std::vector <AstNode*>;      
        for(int i = 0; i < $2->size(); i++){
            Variable* var_tmp = new Variable;
            var_tmp->type = $4->type;
            var_tmp->int_t = $4->int_t;
            var_tmp->real_t = $4->real_t;
            var_tmp->str_t = $4->str_t;
            var_tmp->bool_t = $4->bool_t;
            var_tmp->arr_num = $4->arr_num;
            
            VariableNode* variable_node = new VariableNode($2->at(i).line_number, $2->at(i).col_number, $2->at(i).name, var_tmp, NULL);

            var_list->push_back(variable_node);
        }               

        $$ = new DeclNode(@1.first_line, @1.first_column, var_list);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON
    {
        std::vector <AstNode*> *var_list = new std::vector <AstNode*>;     
        for(int i = 0; i < $2->size(); i++){
            Variable* var_tmp = new Variable;
            var_tmp->type = $4->type;
            var_tmp->int_t = $4->int_t;
            var_tmp->real_t = $4->real_t;
            var_tmp->str_t = $4->str_t;
            var_tmp->bool_t = $4->bool_t;
            var_tmp->arr_num = $4->arr_num;        

            ConstantValueNode* constant_value_node = new ConstantValueNode($4->line_number, $4->col_number, var_tmp);

            VariableNode* variable_node = new VariableNode($2->at(i).line_number, $2->at(i).col_number, $2->at(i).name, var_tmp, constant_value_node);
            
            var_list->push_back(variable_node);        
        }               

        $$ = new DeclNode(@1.first_line, @1.first_column, var_list);
    }
;

Type:
    ScalarType { $$ = $1; }
    |
    ArrType { $$ = $1; }
;

ScalarType:
    INTEGER
    {
        $$ = new Variable;
        $$->type = Datatype::INT;
    }
    |
    REAL
    {
        $$ = new Variable;
        $$->type = Datatype::REAL_;
    }
    |
    STRING
    {
        $$ = new Variable;
        $$->type = Datatype::STR;
    }
    |
    BOOLEAN
    {
        $$ = new Variable;
        $$->type = Datatype::BOOL;
    }
;

ArrType:
    ArrDecl ScalarType
    {
        $$ = new Variable;
        $$->type = $2->type;
        $$->arr_num = $1->arr_num;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF
    {
        $$ = new Variable;
        $$->arr_num.push_back($2);
    }
    |
    ArrDecl ARRAY INT_LITERAL OF
    {
        $1->arr_num.push_back($3);
        $$ = $1;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL
    {
        $$ = new Variable;
        $$->type = Datatype::INT;
        $$->int_t = $1 * $2;

        if($1 == 1){
            $$->line_number = @2.first_line;
            $$->col_number = @2.first_column;                      
        }
        if($1 == -1){
            $$->line_number = @1.first_line;
            $$->col_number = @1.first_column;
        }
    }
    |
    NegOrNot REAL_LITERAL
    {
        $$ = new Variable;
        $$->type = Datatype::REAL_;
        $$->real_t = $1 * $2;

        if($1 == 1){
            $$->line_number = @2.first_line;
            $$->col_number = @2.first_column;  
        }
        if($1 == -1){
            $$->line_number = @1.first_line;
            $$->col_number = @1.first_column;
        }
    }
    |
    StringAndBoolean
    {
        $$ = new Variable;
        $$ = $1;
    }
;

NegOrNot:
    Epsilon { $$ = 1; }
    |
    MINUS %prec UNARY_MINUS { $$ = -1; }
;

StringAndBoolean:
    STRING_LITERAL
    {
        $$ = new Variable;
        $$->type = Datatype::STR;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->str_t = $1;
    }
    |
    TRUE
    {
        $$ = new Variable;
        $$->type = Datatype::BOOL;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->bool_t = Boolean_type::booltype_True;
    }
    |
    FALSE
    {
        $$ = new Variable;
        $$->type = Datatype::BOOL;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->bool_t = Boolean_type::booltype_False;
    }
;

IntegerAndReal:
    INT_LITERAL
    {
        $$ = new Variable;
        $$->type = Datatype::INT;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->int_t = $1;
    }
    |
    REAL_LITERAL
    {
        $$ = new Variable;
        $$->type = Datatype::REAL_;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->real_t = $1;
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement { $$ = $1; }
    |
    Simple { $$ = $1; }
    |
    Condition { $$ = $1; }
    |
    While { $$ = $1; }
    |
    For { $$ = $1; }
    |
    Return { $$ = $1; }
    |
    FunctionCall { $$ = $1; }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END
    {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON { $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3); }
    |
    PRINT Expression SEMICOLON { $$ = new PrintNode(@1.first_line, @1.first_column, $2); }
    |
    READ VariableReference SEMICOLON { $$ = new ReadNode(@1.first_line, @1.first_column, $2); }
;

VariableReference:
    ID ArrRefList { $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2); }
;

ArrRefList:
    Epsilon { $$ = NULL; }
    |
    ArrRefs { $$ = $1; }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET
    {
        $$ = new std::vector<AstNode*>;
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET
    {
        $1->push_back($3);
        $$ = $1;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF
    {
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement { $$ = $2; }
    |
    Epsilon { $$ = NULL; }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO { $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4); }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO
    {
        // declaration node
        Variable* variable = new Variable;
        variable->type = Datatype::INT;
        VariableNode* variable_node = new VariableNode(@2.first_line, @2.first_column, $2, variable, NULL);

        std::vector<AstNode*> *variable_list = new std::vector<AstNode*>;
        variable_list->push_back(variable_node);
        DeclNode* declaration_node = new DeclNode(@2.first_line, @2.first_column, variable_list);


        //assignment node
        VariableReferenceNode* variable_reference_node = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
        Variable* variable_2 = new Variable;
        variable_2->type = Datatype::INT;
        variable_2->int_t = $4;
        ConstantValueNode* constant_value_node_1 = new ConstantValueNode(@4.first_line, @4.first_column, variable_2);

        AssignmentNode* assignment_node = new AssignmentNode(@3.first_line, @3.first_column, variable_reference_node, constant_value_node_1);
       

       //expression node
       Variable* variable_3 = new Variable;
       variable_3->type = Datatype::INT;
       variable_3->int_t = $6;
       ConstantValueNode* constant_value_node_2 = new ConstantValueNode(@6.first_line, @6.first_column, variable_3);
       
       //for node
       $$ = new ForNode(@1.first_line, @1.first_column, declaration_node, assignment_node, constant_value_node_2, $8); 
    }
;

Return:
    RETURN Expression SEMICOLON
    {
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON { $$ = $1; }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS { $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3); }
;

ExpressionList:
    Epsilon { $$ = NULL; }
    |
    Expressions { $$ = $1; }
;

Expressions:
    Expression
    {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression
    {
        $1->push_back($3);
        $$=$1;
    }
;

StatementList:
    Epsilon { $$ = NULL; }
    |
    Statements { $$ = $1; }
;

Statements:
    Statement
    {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Statements Statement
    {
        $1->push_back($2);
        $$=$1;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS { $$ = $2; }
    |
    MINUS Expression %prec UNARY_MINUS
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_MINUS;
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, op_information, $2);
    }
    |
    Expression MULTIPLY Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_MULTIPLY;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression DIVIDE Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_DEVIDE;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression MOD Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_MOD;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression PLUS Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_PLUS;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression MINUS Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_MINUS;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression LESS Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_LESS;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_LESS_OR_EQUAL;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression GREATER Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_GREATER;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_GREATER_OR_EQUAL;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression EQUAL Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_EQUAL;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression NOT_EQUAL Expression{
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_NOT_EQUAL;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    NOT Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_NOT;

        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, op_information, $2);
    }
    |
    Expression AND Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_AND;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    Expression OR Expression
    {
        Op_infos *op_information = new Op_infos;
        op_information->operator_type = Operator::OP_OR;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    IntegerAndReal
    {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);
    }
    |
    StringAndBoolean
    {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);
    }
    |
    VariableReference
    {
        $$ = $1;
    }
    |
    FunctionInvocation
    {
        $$ = $1;
    }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
