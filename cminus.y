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

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror (char *);

%}

%token IF ELSE WHILE RETURN INT VOID
%token ID NUM
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY SEMI COMMA
%token ERROR 

%% /* Grammar for TINY */

/* Appendix A.2 */
/* 1. program -> declaration_list */
program : 
declaration_list { savedTree = $1;};

/* 2. declaration_list -> declaration_list declaration | declaration */
declaration_list : 
declaration_list declaration {

}
| declaration {

};

/* 3. declaration -> var_declaration | fun_declaration */
declaration :
var_declaration {

}
| fun_declaration {

};

/* 4. var_declaration -> type_specifier ID ; | type_specifier ID [NUM] ; */
var_declaration :
type_specifier ID SEMI {

}
| type_specifier ID LBRACE NUM RBRACE SEMI {

};

/* 5. type_specifier -> int | void */
type_specifier :
INT {

}
| VOID {

};

/* 6. fun_declaration -> type_specifier ID ( params ) compound_stmt */
fun_declaration :
type_specifier ID LPAREN params RPAREN compound_stmt {

};

/* 7. params -> param_list | void */
params :
param_list {

}
| VOID {

};

/* 8. param_list -> param_list, param | param */
param_list :
param_list COMMA param {

}
| param {

};

/* 9. param -> type_specifier ID | type_specifier ID [ ] */
param :
type_specifier ID {

}
| type_specifier ID LBRACE RBRACE {

};

/* 10. compound_stmt -> { local_declarations statement_list } */
compound_stmt :
LCURLY local_declarations statement_list RCURLY {

};

/* 11. local_declarations -> local_declarations var_declaration | empty*/
local_declarations :
local_declarations var_declaration {

}
| {

};

/* 12. statement_list -> statement_list statement | empty */
statement_list :
statement_list statement {

}
| {

};

/* 13. statement -> expression_stmt | compound_stmt | selection_stmt | iteration_stmt | return_stmt */
statement :
expression_stmt {

}
| compound_stmt {

}
| selection_stmt {

}
| iteration_stmt {

}
| return_stmt {

};

/* 14. expression_stmt -> expression ; | ; */
expression_stmt :
expression SEMI {

}
| SEMI {

};

/* 15. selection_stmt -> if ( expression ) statment | if ( expression ) statement else statement */
selection_stmt :
IF LPAREN expression RPAREN statement {

}
| IF LPAREN expression RPAREN statement ELSE statement {

};

/* 16. iteration_stmt -> while ( expression ) statement */
iteration_stmt :
WHILE LPAREN expression RPAREN statement {

};

/* 17. return_stmt -> return ; | return expression ; */
return_stmt :
RETURN SEMI {

}
| RETURN expression SEMI {

};

/* 18. expression -> var = expression | simple_expression*/
expression :
var ASSIGN expression {

}
| simple_expression {

};

/* 19. var -> ID | ID [ expression ] */
var :
ID {

}
| ID LBRACE expression RBRACE {

};

/* 20. simple_expression -> additive_expression relop additive_expression | additive_expression */
simple_expression :
additive_expression relop additive_expression {

}
| additive_expression {

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

/* 22. additive_expression -> additive_expression addop term | term */
additive_expression :
additive_expression addop term {

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

/* 28. args -> arg_list | empty */
args :
arg_list {

}
| {

};

/* 29. arg_list -> arg_list , expression | expression */
arg_list :
arg_list COMMA expression {

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

