/*
Chemical Compiler -> Lexical Analyzer File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 23rd April
Date Created - 3rd April
*/

#include <stdio.h>
#include "symbolTable.h"
#include "y.tab.h"
#include <string.h>

char* appendString(const char* dest, const char* src);

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

    for(tempNode = symTable; tempNode != NULL; tempNode = tempNode->hh.next) 
    {
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

    for(tempNode = tempVarTable; tempNode != NULL; tempNode = tempNode->hh.next) 
    {
        printf("VARIABLE -- %d : VALUE -- %s\n", tempNode->key, tempNode->value);
    }
}


/* Methods for making Species tabel of hash map*/
speciesTableNode* putSpecies (const char* key, int value) {
    speciesTableNode* tempNode = NULL;
    
    HASH_FIND_STR(speciesTable, key, tempNode);  /* name: key already in the hash? */
    
    if(tempNode == NULL)
    {
        tempNode = (speciesTableNode*)malloc(sizeof(speciesTableNode));
        tempNode->name = key;
        tempNode->value = value;    /* Cannot use strcpy since const is used */
        HASH_ADD_KEYPTR(hh, speciesTable, tempNode->name, strlen(tempNode->name), tempNode); 
    }
    
    return tempNode;
}

speciesTableNode* updateSpecies (const char* key, int value)
{
    speciesTableNode* tempNode = NULL, *replaceNode = NULL;
    
    HASH_FIND_STR(speciesTable, key, tempNode);  /* name: key already in the hash? */
    
    replaceNode = (speciesTableNode*)malloc(sizeof(speciesTableNode));
    replaceNode->name = key;
    replaceNode->value = value; /* Cannot use strcpy since const is used*/
    
    if(tempNode)
    {
        HASH_REPLACE_STR(speciesTable, name, replaceNode, tempNode);
    }
    else
    {
        printf("Node doesn't exists");
        replaceNode = NULL;
    }
    
    return replaceNode;
}

speciesTableNode* getSpecies (const char* key)
{
    speciesTableNode *tempNode = NULL;
    
    HASH_FIND_STR(speciesTable, key, tempNode);
    
    return tempNode;
}

int getSpeciesTableSize ()
{
    return HASH_COUNT(speciesTable);
}

void printSpeciesTable ()
{
    speciesTableNode *tempNode = NULL;

    for(tempNode = speciesTable; tempNode != NULL; tempNode = tempNode->hh.next) 
    {
        printf("VARIABLE -- %s : VALUE -- %d\n", tempNode->name, tempNode->value);
    }
}

void writeVarToXML(FILE* xmlFile)
{
    speciesTableNode *tempNode = NULL;
    char *varComp = "", *varCompAb = "", *varAb = "";

    for(tempNode = speciesTable; tempNode != NULL; tempNode = tempNode->hh.next) 
    {
        varAb = varComp = appendString(tempNode->name, "_ab");
        varComp = appendString(tempNode->name, "_");
        varCompAb = appendString(tempNode->name, "_ab_");
        
        fprintf(xmlFile, "%s", genSpeciesCode((char *)tempNode->name, tempNode->value));
        fprintf(xmlFile, "%s", genSpeciesCode(varComp, 0));
        fprintf(xmlFile, "%s", genSpeciesCode(varAb, 10000));
        fprintf(xmlFile, "%s", genSpeciesCode(varCompAb, 10000));

        varAb = "";
        varComp = "";
        varCompAb = "";
    }
    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"_localz\"/>\n");
    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"_localz_\"/>\n");
    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"_localz_ab\"/>\n");
    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"_localy\"/>\n");
    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"_localy_\"/>\n");
    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"_localy_ab\"/>\n");

    fprintf(xmlFile, "%s", "<species initialAmount=\"0\" id=\"done\"/>\n");
    fprintf(xmlFile, "%s", "<species initialAmount=\"10000\" id=\"done_ab\"/>\n");
}