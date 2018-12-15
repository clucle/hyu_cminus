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
    if (t->nodekind == StmtK) {
        if (t->kind.stmt == CompoundK) sc_push(t->scope);
        else if (t->kind.stmt == FunctionK) scopeName = t->attr.name;
    }
}

char *nestedScope(char *foo) {
    char *ptr = malloc(strlen(foo) + 2 + 1);
    strcpy(ptr, foo);
    strcat(ptr, "_n");
    return ptr;
}

static void symbolError(TreeNode *t, char *message) {
    fprintf(listing, "Symbol error at line %d: %s\n", t->lineno, message);
    Error = TRUE;
}

int compoundFlag = 0;
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
                symbolError(t, "function already declared in scope");
                break;
            }
            st_insert(scopeName, t->attr.name, t->child[0]->type, t);
            scopeName = t->attr.name;
            sc_push(sc_create(scopeName));
            compoundFlag = 1;
            break;
        case CompoundK:
            if (compoundFlag == 1) compoundFlag = 0;
            else {
                char *newScopeName = nestedScope(sc_top()->name);
                sc_push(sc_create(newScopeName));
            }
            t->scope = sc_top();
            break;
        default:
            break;
        }
        break;
    case ExpK:
        switch (t->kind.exp)
        {
        case VarK:
        case SingleParamK:
            if (st_lookup_excluding_parent(sc_top()->name, t->attr.name)) {
                symbolError(t, "variable already declared in scope");
                break;
            }
            t->type = t->child[0]->type;
            st_insert(sc_top()->name, t->attr.name, t->type, t);
            break;
        case VarArrayK:
        case ArrayParamK:
            if (st_lookup_excluding_parent(sc_top()->name, t->attr.name)) {
                symbolError(t, "variable already declared in scope");
                break;
            }
            st_insert(sc_top()->name, t->attr.name, t->type, t);
            break;
        case ArrayIdK:
        case IdK:
        case CallK:
            if (sc_lookup(t->attr.name) != NULL) {
                st_insert(sc_lookup(t->attr.name)->name, t->attr.name, t->type, t);
            } else {
                symbolError(t, "variable or function undeclared");
                break;
            }
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
    sc_pop();
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

/*
        case AssignK:
            if (t->child[0]->type != Integer)
                typeError(t->child[0], "assignment of non-integer value");
            break;
            */
static void checkNode(TreeNode *t)
{
    switch (t->nodekind)
    {
    case ExpK:
        switch (t->kind.exp)
        {
        case OpK: 
            {
                ExpType left = t->child[0]->type;
                ExpType right = t->child[1]->type;
                TokenType op = t->attr.op;

                if (left == Void) {
                    typeError(t->child[0], "can not be void in op");
                    break;
                }
                if (right == Void) {
                    typeError(t->child[0], "can not be void in op");
                    break;
                }
                // EQ NE LT LE GT GE
                if (op == ASSIGN) {
                    if (left != right) {
                        typeError(t->child[0], "Assign should be same two var's type");
                        break;
                    } else {
                        t->type = t->child[0]->type;
                    }
                } else {
                    
                    if (left != right) {
                        typeError(t->child[0], "Op should be same two var's type");
                        break;
                    }
                    // + - * /
                    if (op == PLUS || op == MINUS || op == TIMES || op == OVER) {
                        if (left == IntegerArray) {
                            typeError(t->child[0], "+ - * / should be integer");
                            break;
                        }
                    }
                    // == != >= > <= <
                    if (op == EQ || op == NE || op == LT || op == LE ||
                        op == GT || op == GE) {
                        if (left == IntegerArray) {
                            typeError(t->child[0], "== != >= > <= < should be integer");
                            break;
                        }
                    }
                    t->type = Integer;
                }
            }
            break;
        case CallK:
            {
                BucketList b = st_lookup(scopeName, t->attr.name);
                if (b == NULL){
                    break;
                }

                TreeNode *functionDeclaredArgs = b->treeNode->child[1];
                TreeNode *curArgs =t->child[0];

                while (functionDeclaredArgs != NULL) {
                    if (curArgs == NULL) {
                        typeError(curArgs, "function argument param cnt not correct");
                        break;
                    }
                    else if (curArgs->type == Void) {
                        typeError(curArgs, "can not come void");
                        break;
                    }
                    else if (curArgs->type != functionDeclaredArgs->type) {
                        
                        typeError(curArgs, "should be same type args");
                        break;
                    } else {
                        curArgs = curArgs->sibling;
                        functionDeclaredArgs = functionDeclaredArgs->sibling;
                    }
                }
                t->type = b->type;
            }
            break;
        case ConstK:
            t->type = Integer;
            break;
        case IdK:
            {
                BucketList b = st_lookup(scopeName, t->attr.name);
                if (b == NULL){
                    break;
                }
                t->type = b->type;
            }
            break;
        case ArrayIdK:
            {
                BucketList b = st_lookup(scopeName, t->attr.name);
                if (b == NULL){
                    break;
                }
                if(t->child[0]->type != Integer)
                    typeError(t->child[0], "ArrayIdK not Integer");
                else{
                    t->type = Integer;
                }
            }
            break;
        default:
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
        case WhileK:
            if (t->child[0]->type == Void)
                typeError(t->child[0], "can not void in if or while state");
            break;
        case CompoundK:
            sc_pop();
            scopeName = sc_top()->name;
            break;
        case ReturnK:
            {
                ExpType returnType = st_lookup(scopeName, scopeName)->type;
                
                if (returnType == Void) {
                    if (t->child[0] != NULL || t->child[0]->type != Void) {
                        typeError(t, "void function return not void");
                    }
                }
                else if (returnType == Integer) {
                    if (t->child[0] == NULL || t->child[0]->type != Integer) {
                        typeError(t, "integer function return not integer");
                    }
                }
            }
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
