/*
Chemical Compiler -> AST Nodes Header File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 31st March
Date Created - 15th March
*/
//#ifndef SYMBOLTABLE_H
//#define SYMBOLTABLE_H

#include "lib/uthash.h"

typedef enum { typeCon, typeId, typeOpr } nodeEnum;

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

// /* identifiers */
// typedef struct {
//     char* name;			/* unique name in hash table */
//     int i;                      /* subscript to sym array */
// } idNodeType;

/* Node of Hash Table */
typedef struct {
    const char *name;
    int value;
    UT_hash_handle hh;         /* makes this structure hashable */
} symTableNode;

/* Node of Hash Table */
typedef struct {
    int key;
    char* value;
    UT_hash_handle hh;         /* makes this structure hashable */
} tempVarTableNode;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag **op;	/* operands */
} oprNodeType;

typedef struct {
    int isEmpty;
    int value;
} registers;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        //idNodeType id;          /* identifiers */
        symTableNode* id;
        oprNodeType opr;        /* operators */
    };
} nodeType;

extern symTableNode *symTable;
extern tempVarTableNode *tempVarTable;
extern registers _localx, _localy, _localz;
extern FILE *temp1, *temp2;

tempVarTableNode* putTempConst (const int key, char* value);
tempVarTableNode* getTempConst (const int key);
int getTempVarTableSize ();
void printTempConstTable ();


symTableNode* putSym (const char* key, int value);
symTableNode* updateSym(const char* key, int value);
symTableNode* getSym(const char* key);
int getSymTableSize();
void printSymTable();
//#endif