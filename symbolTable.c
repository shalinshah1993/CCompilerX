/*
Chemical Compiler -> Lexical Analyzer File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 3rd April
Date Created - 3rd April
*/

#include <stdio.h>
#include "symbolTable.h"
#include "y.tab.h"
#include <string.h>

/* Methods for making symbol tabel of hash map*/
symTableNode* putSym (const char* key, int value) {
    symTableNode* tempNode = NULL;
    
    HASH_FIND_STR(symTable, key, tempNode);  /* name: key already in the hash? */
    
    if(tempNode == NULL)
    {
	tempNode = (symTableNode*)malloc(sizeof(symTableNode));
	tempNode->name = key;
	tempNode->value = value;	/* Cannot use strcpy since const is used */
	HASH_ADD_KEYPTR(hh, symTable, tempNode->name, strlen(tempNode->name), tempNode); 
    }
    
    return tempNode;
}

symTableNode* updateSym (const char* key, int value)
{
    symTableNode* tempNode = NULL, *replaceNode = NULL;
    
    HASH_FIND_STR(symTable, key, tempNode);  /* name: key already in the hash? */
    
    replaceNode = (symTableNode*)malloc(sizeof(symTableNode));
    replaceNode->name = key;
    replaceNode->value = value;	/* Cannot use strcpy since const is used*/
    
    if(tempNode)
    {
	//HASH_DEL(symTable, tempNode);  /* user: pointer to delete */    
	//HASH_ADD_KEYPTR(hh, symTable, tempNode->name, strlen(tempNode->name), tempNode); /* name: key of hash map */
	HASH_REPLACE_STR(symTable, name, replaceNode, tempNode);
    }
    else
    {
	printf("Node doesn't exists");
	replaceNode = NULL;
    }
    
    return replaceNode;
}

symTableNode* getSym (const char* key)
{
    symTableNode *tempNode = NULL;
    
    HASH_FIND_STR(symTable, key, tempNode);
	
    return tempNode;
}

int getSymTableSize ()
{
    return HASH_COUNT(symTable);
}

void printSymTable ()
{
    symTableNode *tempNode = NULL;

    for(tempNode = symTable; tempNode != NULL; tempNode = tempNode->hh.next) {
        printf("VARIABLE -- %s : VALUE -- %d\n", tempNode->name, tempNode->value);
    }
}


/* Methods for making hash map for constants*/
tempVarTableNode* putTempConst (const int key, char* value) {
    tempVarTableNode* tempNode = NULL;
    
    HASH_FIND_INT(tempVarTable, &key, tempNode);  /* name: key already in the hash? */
    
    if(tempNode == NULL)
    {
	tempNode = (tempVarTableNode*)malloc(sizeof(tempVarTableNode));
	tempNode->key = key;
	tempNode->value = value;
	HASH_ADD_INT(tempVarTable, key, tempNode); 
    }
    
    return tempNode;
}

tempVarTableNode* getTempConst (const int key)
{
    tempVarTableNode *tempNode = NULL;
    
    HASH_FIND_INT(tempVarTable, &key, tempNode);
	
    return tempNode;
}


int getTempConstTableSize ()
{
    return HASH_COUNT(tempVarTable);
}

void printTempConstTable ()
{
    tempVarTableNode *tempNode = NULL;

    for(tempNode = tempVarTable; tempNode != NULL; tempNode = tempNode->hh.next) {
        printf("VARIABLE -- %d : VALUE -- %s\n", tempNode->key, tempNode->value);
    }
}
