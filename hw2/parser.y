%{
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token COMMA SEMICOLON COLON LEFTBRACE RIGHTBRACE LEFTARRAY RIGHTARRAY
%token ADD SUB MUL DIVIDE MOD ASSIGN LESSTHAN LESSEQ DIAMOND GREATEREQ GREATERTHAN EQUAL AND OR NOT
%token KWvar KWarray KWof KWboolean KWinteger KWreal KWstring KWtrue KWfalse KWdef KWreturn KWbegin KWend KWwhile KWdo KWif KWthen KWelse KWfor KWto KWprint KWread
%token ID INT OCT FLOAT SCIENTIFIC STRING

%%

ProgramName: ID SEMICOLON VnC_DECL FUNC_LIST COMP_STMT KWend
		   ;

STMT: COMP_STMT
	| SIMP_STMT
	| COND_STMT
	| WHILE_STMT
	| FOR_STMT
	| RET_STMT
	| FUNC_CALL_STMT
	;

STMT_LIST: STMT_LIST1
		 |
		 ;

STMT_LIST1: STMT
		  | STMT_LIST1 STMT
		  ;

VnC_DECL: VnC_DECL1
		|
		;

VnC_DECL1: VnC_TYPE
		 | VnC_DECL1 VnC_TYPE

VnC_TYPE: VAR
		| CONST
		;

FUNC_LIST: FUNC_LIST1
		 |
		 ;

FUNC_LIST1: FUNC_TYPE
		  | FUNC_LIST1 FUNC_TYPE
		  ;

FUNC_TYPE: FUNC_DEF
		 | FUNC_DECL
		 ;

COMP_STMT: KWbegin VnC_DECL STMT_LIST KWend
		 // {printf("matched!!!");}
		 ;

SIMP_STMT: VAR_REF ASSIGN EXP SEMICOLON
		 | KWprint PRINT_TYPE SEMICOLON
		 | KWread VAR_REF SEMICOLON
		 ;

PRINT_TYPE: EXP
		  | LITERAL
		  ;

VAR_REF: ID
	   | ID LEFTARRAY EXP_LIST RIGHTARRAY
	   ;

COND_STMT: KWif EXP KWthen COMP_STMT KWelse COMP_STMT KWend KWif
		 | KWif EXP KWthen COMP_STMT KWend KWif
		 ;

WHILE_STMT: KWwhile EXP KWdo COMP_STMT KWend KWdo
		  ;

FOR_STMT: KWfor ID ASSIGN INT KWto INT KWdo COMP_STMT KWend KWdo
		;

RET_STMT: KWreturn EXP SEMICOLON
		;

FUNC_CALL_STMT: ID LEFTBRACE EXP_LIST RIGHTBRACE SEMICOLON
			  ;

EXP: OPERAND OP OPERAND
   | OPERAND OP EXP
   | LEFTBRACE EXP RIGHTBRACE
   | LEFTBRACE EXP RIGHTBRACE OP EXP
   | OPERAND CONSECUTIVE_OP
   | CONSECUTIVE_OP OPERAND
   | NOT OPERAND
   | OPERAND
   | VAR_REF
   ;

CONSECUTIVE_OP: ADD ADD
			  | SUB SUB
			  | NOT NOT
			  ;

EXP_LIST: EXP_LIST1
		|
		;

EXP_LIST1: EXP 
		 | EXP_LIST COMMA EXP
		 ;

FUNC_DECL: ID LEFTBRACE ARG_LIST RIGHTBRACE COLON TYPE_WO_ARR SEMICOLON
		 | ID LEFTBRACE ARG_LIST RIGHTBRACE
		 ;

FUNC_DEF: ID LEFTBRACE ARG_LIST RIGHTBRACE COLON TYPE_WO_ARR COMP_STMT KWend
		| ID LEFTBRACE ARG_LIST RIGHTBRACE COMP_STMT KWend
		;


ARG_LIST: ARG_LIST1
		|
		;

ARG_LIST1: ARG
		 | ARG_LIST1 COMMA ARG
		 ;

ARG: ID
   | EXP
   | ID COLON TYPE
   ;


ID_LIST: ID_LIST1
	   |
	   ;

ID_LIST1: ID
		| ID_LIST1 COMMA ID
		;

VAR: KWvar ID_LIST COLON TYPE SEMICOLON
   | KWvar ID_LIST COLON KWarray INT KWof TYPE SEMICOLON
   ;

TYPE: KWarray
	| KWboolean
	| KWinteger
	| KWreal
	| KWstring
	| KWarray KWof TYPE
	| KWarray INT KWof TYPE
	;

TYPE_WO_ARR: KWboolean
		   |KWinteger
		   | KWreal
		   | KWstring
		   ;

SCALAR: INT
	  | OCT
	  | FLOAT
	  | SCIENTIFIC 
	  ;

LITERAL: SCALAR
	   | STRING
	   | KWtrue
	   | KWfalse
	   ;

CONST: KWvar ID_LIST COLON LITERAL SEMICOLON
	 

OPERAND: SCALAR
	   | SUB SCALAR
	   | ID
	   | SUB ID
	   | LEFTBRACE OPERAND RIGHTBRACE
	   | FUNC_DECL
	   | STRING
	   ;

OP: ADDOP
  | MULOP
  | RELOP
  | LOGOP
  ;

ADDOP: ADD
	 | SUB
	 ;

MULOP: MUL
	 | DIVIDE
	 | MOD
	 ;

RELOP: LESSTHAN
	 | LESSEQ
	 | DIAMOND
     | GREATERTHAN
	 | GREATEREQ
	 | EQUAL
	 ;

LOGOP: AND
	 | OR
	 | NOT
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
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
