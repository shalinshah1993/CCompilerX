/*
Chemical Compiler -> AST Nodes Header File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 23rd April
Date Created - 15th March
*/
//#ifndef SYMBOLTABLE_H
//#define SYMBOLTABLE_H

#include "lib/uthash.h"
#include "genReaction.h"

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

/* Node of Symbol Table */
typedef struct {
    const char *name;
    int value;
    UT_hash_handle hh;         /* makes this structure hashable */
} symTableNode;

/* Node of Constant Hash Table */
typedef struct {
    int key;
    char* value;
    UT_hash_handle hh;         /* makes this structure hashable */
} tempVarTableNode;

/* Node of Sepcies Table */
typedef struct {
    const char *name;
    int value;                  /* Inital amount of reactant*/
    UT_hash_handle hh;         /* makes this structure hashable */
} speciesTableNode;

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
extern speciesTableNode *speciesTable;
extern registers _localx, _localy, _localz;
extern const int START_AMOUNT;

tempVarTableNode* putTempConst (const int key, char* value);
tempVarTableNode* getTempConst (const int key);
int getTempVarTableSize ();
void printTempConstTable ();


symTableNode* putSym (const char* key, int value);
symTableNode* updateSym(const char* key, int value);
symTableNode* getSym(const char* key);
int getSymTableSize();
void printSymTable();

speciesTableNode* putSpecies (const char* key, int value);
speciesTableNode* updateSpecies (const char* key, int value);
speciesTableNode* getSpecies (const char* key);
int getSpeciesTableSize();
void printSpeciesTable();

void writeVarToXML(FILE* fp);
//#endif