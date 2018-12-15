/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SCOPE_SIZE is the size of the scope list */
#define SCOPE_SIZE 512

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

static ScopeList scopeList[SCOPE_SIZE];
static int nScopeList = 0;
static ScopeList stackScope = NULL;


/* the hash function */
static int hash(char *key)
{
    int temp = 0;
    int i = 0;
    while (key[i] != '\0')
    {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

ScopeList sc_create(char *name) {
    ScopeList s;
    s = malloc(sizeof(struct ScopeListRec));
    s->name = name;
    for (int i = 0; i < SIZE; i++) {
        s->bucket[i] = NULL;
    }
    s->loc = 0;
    return s;
}

ScopeList sc_top() {
    return stackScope;
}

void sc_pop() {
    stackScope = stackScope->parent;
}

void sc_push(ScopeList sc) {
    sc->parent = sc_top();
    stackScope = sc;
    scopeList[nScopeList++] = sc;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(char *scope, char *name, ExpType type, TreeNode *t)
{
    int lineno = t->lineno;
    int h = hash(name);
    ScopeList sc = sc_top();

    while (sc) {
        if (strcmp(sc->name, scope) == 0) {
            break;
        }
        sc = sc->parent;
    }

    BucketList l = sc->bucket[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) /* variable not yet in table */
    {
        l = (BucketList)malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->lines = (LineList)malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = sc->loc++;
        l->lines->next = NULL;
        l->next = sc->bucket[h];
        l->treeNode = t;
        l->type = type;
        sc->bucket[h] = l;
    }
    else /* found in table, so just add line number */
    {
        LineList t = l->lines;
        while (t->next != NULL)
            t = t->next;
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */

ScopeList sc_lookup(char *name)
{
    int h = hash(name);
    ScopeList sc = sc_top();
    while(sc) {
        BucketList l = sc->bucket[h];
        while (l != NULL) {
            if (strcmp(name, l->name) == 0) return sc;
            l = l->next;
        }
        sc = sc->parent;
    }
    return NULL;
}

BucketList st_lookup(char *scope, char *name)
{
    int h = hash(name);
    ScopeList sc = sc_top();
    while(sc) {
        BucketList l = sc->bucket[h];
        while (l != NULL) {
            if (strcmp(name, l->name) == 0) return l;
            l = l->next;
        }
        sc = sc->parent;
    }
    return NULL;
}

BucketList st_lookup_excluding_parent(char *scope, char *name)
{
    int h = hash(name);
    ScopeList sc = sc_top();
    if (strcmp(sc->name, scope) != 0) return NULL;

    BucketList l = sc->bucket[h];
    while (l != NULL) {
        if (strcmp(name, l->name) == 0) return l;
        l = l->next;
    }
    return NULL;
}


char *exp_to_string[] = {"Void", "Integer", "IntegerArray", "Boolean"};
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE *listing)
{
    int i, j;
    fprintf(listing, "  Name        Type      Location   Scope    Line Numbers\n");
    fprintf(listing, "--------  ------------  --------   -----    ------------\n");
    
    for (i = 0; i < nScopeList; i++) {
        ScopeList sc = scopeList[i];

        for (j = 0; j < SIZE; ++j)
        {
            if (sc->bucket[j] != NULL)
            {
                BucketList l = sc->bucket[j];
                while (l != NULL)
                {
                    LineList t = l->lines;
                    fprintf(listing, "%-8s  ", l->name);
                    fprintf(listing, "%-12s     ", exp_to_string[l->type]);
                    fprintf(listing, "%-6d  ", l->memloc);
                    fprintf(listing, "%-6s  ", sc->name);
                    
                    while (t != NULL)
                    {
                        fprintf(listing, "%4d ", t->lineno);
                        t = t->next;
                    }
                    fprintf(listing, "\n");
                    l = l->next;
                }
            }
        }
    }
} /* printSymTab */
