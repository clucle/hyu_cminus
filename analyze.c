/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"

ScopeList globalScope;
static char name[] = "global";
static char *scopeName = name;

/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode *t,
                     void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *))
{
    if (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
                traverse(t->child[i], preProc, postProc);
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}


static void forPop(TreeNode *t)
{
    if (t->nodekind == StmtK && t->kind.stmt == CompoundK) {
        sc_pop();
        scopeName = sc_top()->name;
    }
}

static void forPush(TreeNode *t)
{
    // TODO
}


/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode(TreeNode *t)
{
    switch (t->nodekind)
    {
    case StmtK:
        switch (t->kind.stmt)
        {
        case FunctionK:
            scopeName = sc_top()->name;
            if (st_lookup_excluding_parent(scopeName, t->attr.name)) {
                // scope 에 같은 함수 존재 에러
                // TODO : 에러처리
            }
            st_insert(scopeName, t->attr.name, t->type, t);
            scopeName = t->attr.name;
            break;
        case CompoundK:
            sc_push(sc_create(scopeName));
            break;
        case AssignK:

            break;
        default:
            break;
        }
        break;
    case ExpK:
        switch (t->kind.exp)
        {
        case VarK:
            scopeName = sc_top()->name;
            if (st_lookup_excluding_parent(scopeName, t->attr.name)) {
                // scope 에 같은 변수 존재 에러
                // TODO : 에러처리
            }
            st_insert(scopeName, t->attr.name, t->type, t);
            break;
        case IdK:
            /*
            if (st_lookup(NULL, t->attr.name) == -1)
                // not yet in table, so treat as new definition 
                st_insert(NULL, t->attr.name, t->type, t->lineno, location++);
            else
                //already in table, so ignore location, add line number of use only 
                st_insert(NULL, t->attr.name, t->type, t->lineno, 0);
            */
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode *syntaxTree)
{
    globalScope = sc_create(scopeName);
    sc_push(globalScope);

    TreeNode *input_func;
    input_func = newStmtNode(FunctionK);
    input_func->type = Integer;
    input_func->attr.name = malloc(strlen("input")+1);
    input_func->lineno = 0;
    strcpy(input_func->attr.name, "input");
    input_func->child[0] = NULL;
    input_func->child[1] = NULL;
    st_insert(scopeName, input_func->attr.name, input_func->type, input_func);


    TreeNode *output_func;
    output_func = newStmtNode(FunctionK);
    output_func->type = Void;
    output_func->attr.name = malloc(strlen("input")+1);
    output_func->lineno = 0;
    strcpy(output_func->attr.name, "output");
    output_func->child[0] = NULL;
    output_func->child[1] = NULL;
    st_insert(scopeName, output_func->attr.name, output_func->type, output_func);
    
    ScopeList tmpScope = sc_create("output");
    sc_push(tmpScope);

    TreeNode *arg;
    arg = newExpNode(SingleParamK);
    arg->type = Integer;
    arg->attr.name = malloc(strlen("arg")+1);
    arg->lineno = 0;
    strcpy(arg->attr.name, "arg");
    arg->child[0] = NULL;
    arg->child[1] = NULL;
    st_insert(sc_top()->name, arg->attr.name, arg->type, arg);

    sc_pop();

    traverse(syntaxTree, insertNode, forPop);
    if (TraceAnalyze)
    {
        fprintf(listing, "\nSymbol table:\n\n");
        printSymTab(listing);
    }
}

static void typeError(TreeNode *t, char *message)
{
    fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
    Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode *t)
{
    switch (t->nodekind)
    {
    case ExpK:
        switch (t->kind.exp)
        {
        case OpK:
            if ((t->child[0]->type != Integer) ||
                (t->child[1]->type != Integer))
                typeError(t, "Op applied to non-integer");
            if ((t->attr.op == EQ) || (t->attr.op == LT))
                t->type = Boolean;
            else
                t->type = Integer;
            break;
        // case ConstK:
        case IdK:
            t->type = Integer;
            break;
        default:
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
            if (t->child[0]->type == Integer)
                typeError(t->child[0], "if test is not Boolean");
            break;
        case AssignK:
            if (t->child[0]->type != Integer)
                typeError(t->child[0], "assignment of non-integer value");
            break;
        case CompoundK:
            sc_pop();
            scopeName = sc_top()->name;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode *syntaxTree)
{
    sc_push(globalScope);
    traverse(syntaxTree, forPush, checkNode);
    sc_pop();
}
