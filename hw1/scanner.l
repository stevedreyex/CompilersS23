%{
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define LIST                concatenateString(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
static char buffer[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *buffer_ptr = buffer;
char * tmp_str[MAX_LINE_LENG];
int str_counter = 0;
int dq_counter = 0;

static void concatenateString(const char *yytext_ptr);

%}

digit		[0-9]
letter		[a-zA-Z]
oct_digit	[0-7]

%x COMMENTS

%%

 /* "//".* { LIST; } */

"/*"						        {  BEGIN COMMENTS; LIST; }
<COMMENTS>\n                        { process_line(); }
<COMMENTS>.                         { LIST; }
<COMMENTS>"*/"						{ LIST; BEGIN 0; }

"//&S-".*                             { LIST; opt_src = 0; }
"//&S+".*                             { LIST; opt_src = 1; }
"//&T-".*                             { LIST; opt_tok = 0; }
"//&T+".*                             { LIST; opt_tok = 1; }

"//".*      { LIST; }

"var"		{ TOKEN(KWvar); }
"def"		{ TOKEN(KWdef); }
"array"		{ TOKEN(KWarray); }
"of"		{ TOKEN(KWof); }
"boolean"	{ TOKEN(KWboolean); }
"integer"	{ TOKEN(KWinteger); }
"real"		{ TOKEN(KWreal); }
"string"	{ TOKEN(KWstring); }
"true"		{ TOKEN(KWtrue); }
"false"		{ TOKEN(KWfalse); }
"while"		{ TOKEN(KWwhile); }
"do"		{ TOKEN(KWdo); }
"if"		{ TOKEN(KWif); }
"then"		{ TOKEN(KWthen); }
"else"		{ TOKEN(KWelse); }
"for"		{ TOKEN(KWfor); }
"to"		{ TOKEN(KWto); }
"begin"		{ TOKEN(KWbegin); }
"end"		{ TOKEN(KWend); }
"print"		{ TOKEN(KWprint); }
"read"		{ TOKEN(KWread); }
"return"	{ TOKEN(KWreturn); }

","     { TOKEN_CHAR(','); }
";"     { TOKEN_CHAR(';'); }
":"     { TOKEN_CHAR(':'); }
"("     { TOKEN_CHAR('('); }
")"     { TOKEN_CHAR(')'); }
"["     { TOKEN_CHAR('['); }
"]"     { TOKEN_CHAR(']'); }
"+"     { TOKEN_CHAR('+'); }
"-"     { TOKEN_CHAR('-'); }
"*"     { TOKEN_CHAR('*'); }
"/"     { TOKEN_CHAR('/'); }
">"     { TOKEN_CHAR('>'); }
"<"     { TOKEN_CHAR('<'); }
"="     { TOKEN_CHAR('='); }
":="    { TOKEN(:=); }
"<="    { TOKEN(<=); }
"<>"    { TOKEN(<>); }
">="    { TOKEN(>=); }
"mod"	{ TOKEN(mod); }
"and"	{ TOKEN(and); }
"or"	{ TOKEN(or); }
"not"	{ TOKEN(not);}

{letter}({letter}|{digit})*	                                { TOKEN_STRING(id, yytext); }
0|[1-9]{digit}*                                             { TOKEN_STRING(integer, yytext);}
[1-9]{digit}*.?{digit}*(E|e)[+-]?0?([1-9]{digit}*)*         { TOKEN_STRING(scientific, yytext); }
0({oct_digit})+				                                { TOKEN_STRING(oct_integer, yytext); }
"0.0"|[1-9]{digit}*"."{digit}*[1-9]		                    { TOKEN_STRING(float, yytext); }
\"([^"\n]*\"{2})*[^"\n]*\"			                        { process_string(); }
\n                                                          { process_line(); }
[ \t\f\r]*                                                  { LIST; }

    /* Catch the character which is not accepted by rules above */
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

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}

void process_string(){
    memset(tmp_str,0,strlen(tmp_str)); 
    LIST; 
    int t = 0;
    for (int i = 1; i < yyleng-1; i++) {
        if(yytext[i]=='\"' && yytext[i+1]=='\"') {
            ++i;
        }
        tmp_str[t] = yytext[i];
        t++;
    }
    tmp_str[t] = '\0';
    if(opt_tok) {
        printf("<string: ");
        for (int j = 0; j<sizeof(tmp_str)/sizeof(tmp_str[0]); j++){
        if (tmp_str[j] == '\0') break;
        printf("%c", tmp_str[j]);
        }
        printf(">\n");
    }
}

void process_line(){
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}