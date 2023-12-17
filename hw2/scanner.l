%{
#include <stdint.h>

#include "parser.h"

#define LIST                concatenateString(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char string_literal[MAX_LINE_LENG];
static char *buffer_ptr = buffer;

static void concatenateString(const char *yytext_ptr);

%}

integer 0|[1-9][0-9]*
float {integer}\.(0|[0-9]*[1-9])

    /* exclusive start condition */
%x CCOMMENT

%%
    /* Delimiter */
"," { TOKEN_CHAR(','); return(COMMA); }
";" { TOKEN_CHAR(';'); return(SEMICOLON); }
":" { TOKEN_CHAR(':'); return(COLON); }
"(" { TOKEN_CHAR('('); return(LEFTBRACE); }
")" { TOKEN_CHAR(')'); return(RIGHTBRACE); }
"[" { TOKEN_CHAR('['); return(LEFTARRAY); }
"]" { TOKEN_CHAR(']'); return(RIGHTARRAY); }

    /* Operator */
"+"   { TOKEN_CHAR('+'); return(ADD); }
"-"   { TOKEN_CHAR('-'); return(SUB); }
"*"   { TOKEN_CHAR('*'); return(MUL); }
"/"   { TOKEN_CHAR('/'); return(DIVIDE); }
"mod" { TOKEN(mod); return(MOD); }
":="  { TOKEN(:=); return(ASSIGN); }
"<"   { TOKEN_CHAR('<'); return(LESSTHAN); }
"<="  { TOKEN(<=); return(LESSEQ); }
"<>"  { TOKEN(<>); return(DIAMOND); }
">="  { TOKEN(>=); return(GREATEREQ); }
">"   { TOKEN_CHAR('>'); return(GREATERTHAN); }
"="   { TOKEN_CHAR('='); return(EQUAL); }
"and" { TOKEN(and); return(AND); }
"or"  { TOKEN(or); return(OR); }
"not" { TOKEN(not); return(NOT); }

    /* Reserved Word */
"var"     { TOKEN(KWvar); return(KWvar); }
"array"   { TOKEN(KWarray); return(KWarray); }
"of"      { TOKEN(KWof); return(KWof); }
"boolean" { TOKEN(KWboolean); return(KWboolean); }
"integer" { TOKEN(KWinteger); return(KWinteger); }
"real"    { TOKEN(KWreal); return(KWreal); }
"string"  { TOKEN(KWstring); return(KWstring); }

"true"    { TOKEN(KWtrue); return(KWtrue); }
"false"	  { TOKEN(KWfalse); return(KWfalse); } 
"def"     { TOKEN(KWdef); return(KWdef); }
"return"  { TOKEN(KWreturn); return(KWreturn); }

"begin"   { TOKEN(KWbegin); return(KWbegin); }
"end"     { TOKEN(KWend); return(KWend); }

"while"   { TOKEN(KWwhile); return(KWwhile); }
"do"      { TOKEN(KWdo); return(KWdo); }

"if"      { TOKEN(KWif); return(KWif); }
"then"    { TOKEN(KWthen); return(KWthen); }
"else"    { TOKEN(KWelse); return(KWelse); }

"for"     { TOKEN(KWfor); return(KWfor); }
"to"      { TOKEN(KWto); return(KWto); }

"print"   { TOKEN(KWprint); return(KWprint); }
"read"    { TOKEN(KWread); return(KWread); }

    /* Identifier */
[a-zA-Z][a-zA-Z0-9]* { TOKEN_STRING(id, yytext); return(ID); }

    /* Integer (decimal/octal) */
{integer} { TOKEN_STRING(integer, yytext); return(INT); }
0[0-7]+   { TOKEN_STRING(oct_integer, yytext); return(OCT); }

    /* Floating-Point */
{float} { TOKEN_STRING(float, yytext); return(FLOAT); }

    /* Scientific Notation [Ee][+-]?[0-9]+ */
({integer}|{float})[Ee][+-]?({integer}) { TOKEN_STRING(scientific, yytext); return(SCIENTIFIC); }

    /* String */
\"([^"\n]|\"\")*\" {
    char *yyt_ptr = yytext + 1;  // +1 for skipping the first double quote "
    char *str_ptr = string_literal;

    while (*yyt_ptr) {
        if (*yyt_ptr == '"') {
            // Handle the situation of two double quotes "" in string literal
            if (*(yyt_ptr + 1) == '"') {
                *str_ptr = *yyt_ptr;
                ++str_ptr;
                yyt_ptr += 2; // move to the next character of ""
            } else {
                ++yyt_ptr;
            }
        } else {  // normal character
            *str_ptr = *yyt_ptr;
            ++str_ptr;
            ++yyt_ptr;
        }
    }
    *str_ptr = '\0';
    TOKEN_STRING(string, string_literal);
	return(STRING); 
}

    /* Whitespace */
[ \t]+ { LIST; }

    /* Pseudocomment */
"//&"[ST][+-].* {
    LIST;
    char option = yytext[3];
    switch (option) {
    case 'S':
        opt_src = (yytext[4] == '+') ? 1 : 0;
        break;
    case 'T':
        opt_tok = (yytext[4] == '+') ? 1 : 0;
        break;
    }
}

    /* C++ Style Comment */
"//".* { LIST; }

    /* C Style Comment */
"/*"           { LIST; BEGIN(CCOMMENT); }
<CCOMMENT>"*/" { LIST; BEGIN(INITIAL); }
<CCOMMENT>.    { LIST; }

    /* Newline */
<INITIAL,CCOMMENT>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

    /* Catch the character which is not accepted by all rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void concatenateString(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *buffer_ptr = *yytext_ptr;
        ++buffer_ptr;
        ++yytext_ptr;
    }
    *buffer_ptr = '\0';
}
