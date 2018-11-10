/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case ELSE:
    case WHILE:
    case RETURN:
    case INT:
    case VOID:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;
    case ASSIGN: fprintf(listing,"=\n"); break; 
    case EQ: fprintf(listing,"==\n"); break;
    case NE: fprintf(listing,"!=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case LE: fprintf(listing,"<=\n"); break;         
    case GT: fprintf(listing,">\n"); break;
    case GE: fprintf(listing,">=\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LBRACE: fprintf(listing,"[\n"); break;
    case RBRACE: fprintf(listing,"]\n"); break;
    case LCURLY: fprintf(listing,"{\n"); break;
    case RCURLY: fprintf(listing,"}\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case COMMA: fprintf(listing,",\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

TreeNode * allocTree(void) {
  TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * dangleTree(TreeNode * parent, TreeNode * child) {
  if (parent != NULL) {
    TreeNode *tmp = parent;
    while (tmp->sibling != NULL) tmp = tmp->sibling;
    tmp->sibling = child;
  } else {
    parent = child;
  }
  return parent;
}


/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ 
  TreeNode * t = allocTree();
  if (t != NULL) {
    t->nodekind = StmtK;
    t->kind.stmt = kind;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ 
  TreeNode * t = allocTree();
  if (t != NULL) {
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->type = Void;
  }
  return t;
}

TreeNode * newTypeNode(TypeKind kind)
{ 
  TreeNode * t = allocTree();
  if (t != NULL) {
    t->nodekind = TokenTypeK;
    t->kind.exp = kind;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */

void printTreeToken( TreeNode * tree ) {

    switch (tree->attr.type) {
      case ASSIGN:
        fprintf(listing, "=\n");
        break;
      case EQ:
        fprintf(listing, "==\n");
        break;
      case NE:
        fprintf(listing, "!=\n");
        break;
      case LT:
        fprintf(listing, "<\n");
        break;
      case LE:
        fprintf(listing, "<=\n");
        break;
      case GT:
        fprintf(listing, ">\n");
        break;
      case GE:
        fprintf(listing, ">=\n");
        break;
      case INT:
        fprintf(listing, "INT\n");
        break;
      case VOID:
        fprintf(listing, "VOID\n");
        break;
      case PLUS:
        fprintf(listing, "+\n");
        break;
      case MINUS:
        fprintf(listing, "-\n");
        break;
      case TIMES:
        fprintf(listing, "*\n");
        break;
      case OVER:
        fprintf(listing, "/\n");
        break;

      default:
        fprintf(listing,"Unknown Token kind\n");
        break;
    }
}

void printTree( TreeNode * tree )
{
  // TODO : c-minus syntax tree
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        // 6
        case FunctionK:
          fprintf(listing,"function declaration, name : return type : ");
          printTreeToken(tree->child[0]);
          printTree(tree->child[1]);
          printTree(tree->child[2]);
          break;
        // 10
        case CompoundK:
          fprintf(listing,"Compound statement : \n");
          printTree(tree->child[0]);
          printTree(tree->child[1]);
          break;
        // 15
        case IfK:
          fprintf(listing,"If (condition) (body) (else)\n");
          printTree(tree->child[0]);
          printTree(tree->child[1]);
          printTree(tree->child[2]);
          break;
        // 16
        case WhileK:
          fprintf(listing,"While %d\n", tree->lineno);
          printTree(tree->child[0]);
          printTree(tree->child[1]);
          break;
        // 17
        case ReturnK:
          fprintf(listing,"Return : \n");
          printTree(tree->child[0]);
          break;

        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        // 4
        case VarK:
          fprintf(listing,"Var declaration, name : %s type : ", tree->attr.name);
          printTreeToken(tree->child[0]);
          break;
        case VarArrayK:
          fprintf(listing,"Var declaration, name : %s length : %d, type : ", tree->attr.arr.name, tree->attr.arr.length);
          printTreeToken(tree->child[0]);
          break;
        // 9
        case SingleParamK:
          fprintf(listing,"SingleParameter, name : %s, type : ", tree->attr.name);
          printTreeToken(tree->child[0]);
          break;
        case ArrayParamK:
          fprintf(listing,"ArrayParamK %d\n", tree->lineno);
          printTreeToken(tree->child[0]);
          break;
        // 18
        case AssignK:
          fprintf(listing,"Assign : (destination) (source)\n");
          printTree(tree->child[0]);
          printTree(tree->child[1]);
          break;
        // 19
        case IdK:
          fprintf(listing,"Id : %s\n", tree->attr.name);
          break;
        case ArrayIdK:
          fprintf(listing,"Array Id : %s\n", tree->attr.name);
          break;
        // 20
        case OpK:
          fprintf(listing,"Op : ");
          printTreeToken(tree);
          printTree(tree->child[0]);
          printTree(tree->child[1]);
          break;
        // 26
        case ConstK:
          fprintf(listing, "Const : %d\n", tree->attr.val);
          break;
        // 27
        case CallK:
          fprintf(listing,"Call, name : %s, with arguments below\n", tree->attr.name);
          printTree(tree->child[0]);
          break;

        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==TokenTypeK) {
      if (tree->attr.type == VOID) {
        fprintf(listing,"Single parameter, name : (null), type :void\n");
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    /*
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);*/
    tree = tree->sibling;
  }
  UNINDENT;
}
