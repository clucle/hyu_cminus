/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);

%}

%token IF ELSE WHILE RETURN INT VOID
%token ID NUM
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY SEMI COMMA
%token ERROR ENDFILE 

%% /* Grammar for TINY */

/* Appendix A.2 */
/* 1. program -> declaration-list */
program : 
declaration-list { savedTree = $1;};

/* 2. declaration-list -> declaration-list declaration | declaration */
declaration-list : 
declaration-list declaration {

}
| declaration {

};

/* 3. declaration -> var-declaration | fun-declaration */
declaration :
var-declaration {

}
| fun-declaration {

};

/* 4. var-declaration -> type-specifier ID ; | type-specifier ID [NUM] ; */
var-declaration :
type-specifier ID SEMI {

}
| type-specifier ID LBRACE NUM RBRACE SEMI {

};

/* 5. type-specifier -> int | void */
type-specifier :
INT {

}
| VOID {

};

/* 6. fun-declaration -> type-specifier ID ( params ) compound-stmt */
fun-declaration :
type-specifier ID LPAREN params RPAREN compound-stmt {

};

/* 7. params -> param-list | void */
params :
param-list {

}
| VOID {

};

/* 8. param-list -> param-list, param | param */
param-list :
param-list COMMA param {

}
| param {

};

/* 9. param -> type-specifier ID | type-specifier ID [ ] */
param :
type-specifier ID {

}
| type-specifier ID LBRACE RBRACE {

};

/* 10. compound-stmt -> { local-declarations statement-list } */
compound-stmt :
LCURLY local-declarations statement-list RCURLY {

};

/* 11. local-declarations -> local-declarations var-declarations | empty*/
local-declarations :
local-declarations var-declarations {

}
| {

};

/* 12. statement-list -> statement-list statement | empty */
statement-list :
statement-list statement {

}
| {

};

/* 13. statement -> expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt */
statement :
expression-stmt {

}
| compound-stmt {

}
| selection-stmt {

}
| iteration-stmt {

}
| return-stmt {

};

/* 14. expression-stmt -> expression ; | ; */
expression-stmt :
expression SEMI {

}
| SEMI {

};

/* 15. selection-stmt -> if ( expression ) statment | if ( expression ) statement else statement */
selection-stmt :
if LPAREN expression RPAREN statement {

}
| if LPAREN expression RPAREN statement ELSE statement {

};

/* 16. iteration-stmt -> while ( expression ) statement */
iteration-stmt :
WHILE LPAREN expression RPAREN statement {

};

/* 17. return-stmt -> return ; | return expression ; */
return-stmt :
RETURN SEMI {

}
| RETURN expression SEMI {

};

/* 18. expression -> var = expression | simple-expression*/
expression :
var EQ expression {

}
| simple-expression {

};

/* 19. var -> ID | ID [ expression ] */
var :
ID {

}
| ID LBRACE expression RBRACE {

};

/* 20. simple-expression -> additive-expression relop additive-expression | additive-expression */
simple-expression :
additive-expression relop additive-expression {

}
| additive-expression {

};

/* 21. relop -> <= | < | > | >= | == | != */
relop :
LE {

}
| LT {

}
| GT {

}
| GE {

}
| EQ {

}
| NE {

};

/* 22. additive-expression -> additive-expression addop term | term */
additive-expression :
additive-expression addop term {

}
| term {

};

/* 23. addop -> + | - */
addop :
PLUS {

}
| MINUS {

};

/* 24. term -> term mulop factor | factor */
term :
term mulop factor {

}
| factor {

};

/* 25. mulop -> * | / */
mulop :
TIMES {

}
| OVER {

};

/* 26. factor -> ( expression ) | var | call | NUM */
factor :
LPAREN expression RPAREN {

}
| var {

}
| call {

}
| NUM {

};

/* 27. call -> ID ( args ) */
call :
ID LPAREN args RPAREN {

};

/* 28. args -> arg-list | empty */
args :
arg-list {

}
| {

};

/* 29. arg-list -> arg-list , expression | expression */
arg-list :
arg-list COMMA expression {

}
| expression {

};

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

