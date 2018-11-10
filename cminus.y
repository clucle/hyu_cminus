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
static int savedNumber;
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
  $$ = dangleTree($1, $2);
}
| declaration {
  $$ = $1;
};

/* 3. declaration -> var_declaration | fun_declaration */
declaration :
var_declaration {
  $$ = $1;
}
| fun_declaration {
  $$ = $1;
};

/* 4. var_declaration -> type_specifier ID ; | type_specifier ID [NUM] ; */
var_declaration :
type_specifier _id SEMI {
  $$ = newExpNode(VarK);
  $$->child[0] = $1;
  $$->lineno = lineno;
  $$->attr.name = savedName;
}
| type_specifier _id LBRACE _num RBRACE SEMI {
  $$ = newExpNode(VarArrayK);
  $$->child[0] = $1;
  $$->lineno = lineno;
  $$->attr.arr.name = savedName;
  $$->attr.arr.length = savedNumber;
};

/* 5. type_specifier -> int | void */
type_specifier :
INT {
  $$ = newTypeNode(TypeK);
  $$->attr.type = INT;
  $$->lineno = lineno;
}
| VOID {
  $$ = newTypeNode(TypeK);
  $$->attr.type = VOID;
  $$->lineno = lineno;
};

/* 6. fun_declaration -> type_specifier ID ( params ) compound_stmt */
fun_declaration :
type_specifier _id LPAREN params RPAREN compound_stmt {
  $$ = newStmtNode(FunctionK);
  $$->attr.name = savedName;
  $$->child[0] = $1;
  $$->child[1] = $4;
  $$->child[2] = $6;
  $$->lineno = lineno;
};

/* 7. params -> param_list | void */
params :
param_list {
  $$ = $1;
}
| VOID {
  $$ = newTypeNode(TypeK);
  $$->attr.type = VOID;
  $$->lineno = lineno;
};

/* 8. param_list -> param_list, param | param */
param_list :
param_list COMMA param {
  $$ = dangleTree($1, $3);
}
| param {
  $$ = $1;
};

/* 9. param -> type_specifier ID | type_specifier ID [ ] */
param :
type_specifier _id {
  $$ = newExpNode(SingleParamK);
  $$->attr.name = savedName;
  $$->child[0] = $1;
  $$->lineno = lineno;
}
| type_specifier _id LBRACE RBRACE {
  $$ = newExpNode(ArrayParamK);
  $$->attr.name = savedName;
  $$->child[0] = $1;
  $$->lineno = lineno;
};

/* 10. compound_stmt -> { local_declarations statement_list } */
compound_stmt :
LCURLY local_declarations statement_list RCURLY {
  $$ = newStmtNode(CompoundK);
  $$->child[0] = $2;
  $$->child[1] = $3;
};

/* 11. local_declarations -> local_declarations var_declaration | empty*/
local_declarations :
local_declarations var_declaration {
  $$ = dangleTree($1, $2);
}
| {
  $$ = NULL;
};

/* 12. statement_list -> statement_list statement | empty */
statement_list :
statement_list statement {
  $$ = dangleTree($1, $2);
}
| {
  $$ = NULL;
};

/* 13. statement -> expression_stmt | compound_stmt | selection_stmt | iteration_stmt | return_stmt */
statement :
expression_stmt {
  $$ = $1;
}
| compound_stmt {
  $$ = $1;
}
| selection_stmt {
  $$ = $1;
}
| iteration_stmt {
  $$ = $1;
}
| return_stmt {
  $$ = $1;
};

/* 14. expression_stmt -> expression ; | ; */
expression_stmt :
expression SEMI {
  $$ = $1;
}
| SEMI {
  $$ = NULL;
};

/* 15. selection_stmt -> if ( expression ) statment | if ( expression ) statement else statement */
selection_stmt :
IF LPAREN expression RPAREN statement {
  $$ = newStmtNode(IfK);
  $$->child[0] = $3;
  $$->child[1] = $5;
  $$->child[2] = NULL;
  $$->lineno = lineno;
}
| IF LPAREN expression RPAREN statement ELSE statement {
  $$ = newStmtNode(IfK);
  $$->child[0] = $3;
  $$->child[1] = $5;
  $$->child[2] = $7;
  $$->lineno = lineno;
};

/* 16. iteration_stmt -> while ( expression ) statement */
iteration_stmt :
WHILE LPAREN expression RPAREN statement {
  $$ = newStmtNode(WhileK);
  $$->child[0] = $3;
  $$->child[1] = $5;
  $$->lineno = lineno;
};

/* 17. return_stmt -> return ; | return expression ; */
return_stmt :
RETURN SEMI {
  $$ = newStmtNode(ReturnK);
  $$->child[0] = NULL;
  $$->lineno = lineno;
}
| RETURN expression SEMI {
  $$ = newStmtNode(ReturnK);
  $$->child[0] = $2;
  $$->lineno = lineno;
};

/* 18. expression -> var = expression | simple_expression*/
expression :
var ASSIGN expression {
  $$ = newExpNode(AssignK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->lineno = lineno;
}
| simple_expression {
  $$ = $1;
};

/* 19. var -> ID | ID [ expression ] */
var :
_id {
  $$ = newExpNode(IdK);
  $$->attr.name = savedName;
  $$->lineno = lineno;
}
| _id LBRACE expression RBRACE {
  $$ = newExpNode(ArrayIdK);
  $$->attr.name = savedName;
  $$->child[0] = $3;
  $$->lineno = lineno;
};

/* 20. simple_expression -> additive_expression relop additive_expression | additive_expression */
simple_expression :
additive_expression relop additive_expression {
  $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->attr.op = $2->attr.type;
  $$->child[1] = $3;
  $$->type = Integer;
  $$->lineno = lineno;
}
| additive_expression {
  $$ = $1;
};

/* 21. relop -> <= | < | > | >= | == | != */
relop :
LE {
  $$ = newTypeNode(TypeK);
  $$->attr.type = LE;
  $$->lineno = lineno;
}
| LT {
  $$ = newTypeNode(TypeK);
  $$->attr.type = LT;
  $$->lineno = lineno;
}
| GT {
  $$ = newTypeNode(TypeK);
  $$->attr.type = GT;
  $$->lineno = lineno;
}
| GE {
  $$ = newTypeNode(TypeK);
  $$->attr.type = GE;
  $$->lineno = lineno;
}
| EQ {
  $$ = newTypeNode(TypeK);
  $$->attr.type = EQ;
  $$->lineno = lineno;
}
| NE {
  $$ = newTypeNode(TypeK);
  $$->attr.type = NE;
  $$->lineno = lineno;
};

/* 22. additive_expression -> additive_expression addop term | term */
additive_expression :
additive_expression addop term {
  $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->attr.op = $2->attr.type;
  $$->child[1] = $3;
  $$->type = Integer;
  $$->lineno = lineno;
}
| term {
  $$ = $1;
};

/* 23. addop -> + | - */
addop :
PLUS {
  $$ = newTypeNode(TypeK);
  $$->attr.type = PLUS;
  $$->lineno = lineno;
}
| MINUS {
  $$ = newTypeNode(TypeK);
  $$->attr.type = MINUS;
  $$->lineno = lineno;
};

/* 24. term -> term mulop factor | factor */
term :
term mulop factor {
  $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->attr.op = $2->attr.type;
  $$->child[1] = $3;
  $$->type = Integer;
  $$->lineno = lineno;
}
| factor {
  $$ = $1;
};

/* 25. mulop -> * | / */
mulop :
TIMES {
  $$ = newTypeNode(TypeK);
  $$->attr.type = TIMES;
  $$->lineno = lineno;
}
| OVER {
  $$ = newTypeNode(TypeK);
  $$->attr.type = OVER;
  $$->lineno = lineno;
};

/* 26. factor -> ( expression ) | var | call | NUM */
factor :
LPAREN expression RPAREN {
  $$ = $2;
}
| var {
  $$ = $1;
}
| call {
  $$ = $1;
}
| _num {
  $$ = newExpNode(ConstK);
  $$->attr.val = savedNumber;
  $$->type = Integer;
};

/* 27. call -> ID ( args ) */
call :
_id LPAREN args RPAREN {
  $$ = newExpNode(CallK);
  $$->attr.name = savedName;
  $$->child[0] = $3;
  $$->lineno = lineno;
};

/* 28. args -> arg_list | empty */
args :
arg_list {
  $$ = $1;
}
| {
  $$ = NULL;
};

/* 29. arg_list -> arg_list , expression | expression */
arg_list :
arg_list COMMA expression {
  $$ = dangleTree($1, $3);
}
| expression {
  $$ = $1;
};

/* save val */
_id :
ID {
  savedName = copyString(tokenString);
  savedLineNo = lineno;
};

_num :
NUM {
  savedNumber = atoi(tokenString);
  savedLineNo = lineno;
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

