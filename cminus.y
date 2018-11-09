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
/* 2. declaration-list -> declaration-list declaration | declaration */
/* 3. declaration -> var-declaration | fun-declaration */
/* 4. var-declaration -> type-specifier ID ; | type-specifier ID [NUM] ; */
/* 5. type-specifier -> int | void */
/* 6. fun-declaration -> type-specifier ID (params) compound-stmt */
/* 7. params -> param-list | void */
/* 8. param-list -> param-list, param | param */
/* 9. parama -> type-specifier ID | typeSspecifier ID [ ] */
/* 10. compound-stmt -> { local-declarations statement-list } */
/* 11. local-declarations -> local-declarations var-declarations | empty*/
/* 12. statement-list -> statement | empty*/
/* 13. statement -> expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt */
/* 14. expression-stmt -> expression ; | ; */
/* 15. selection-stmt -> if ( expression ) statment | if ( expression ) statement else statement */
/* 16. iteration-stmt -> while ( expression ) statement */
/* 17. return-stmt -> return ; | return expression ; */
/* 18. expression -> var = expression | simple-expression*/
/* 19. var -> ID | ID [ expression ] */
/* 20. simple-expression -> additive-expression relop additive-expression | additive-expression */
/* 21. relop -> <= | < | > | >= | == | != */
/* 22. additive-expression -> additive-expression addop term | term */
/* 23. addop -> + | - */
/* 24. term -> term mulop factor | factor */
/* 25. mulop -> * | / */
/* 26. factor -> ( expression ) | var | call | NUM */
/* 27. call -> ID ( args ) */
/* 28. args -> arg-list | empty */
/* 29. arg-list -> arg-list , expression | expression */

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

