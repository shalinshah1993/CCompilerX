/*
Chemical Compiler -> Parsing File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 31st March
Date Created - 15th March
*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "symbolTable.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *declareId(const char* i, int j);
nodeType *checkId(const char* i);
nodeType *con(int value);
void freeNode(nodeType *p);
void parseTree(nodeType *p);
int yylex(void);
void terminate();

void yyerror(char *s);
symTableNode *symTable;
tempVarTableNode *tempVarTable;
registers _localx, _localy, _localz;

const int FAST_REACT_PROPENSITY = 1000;
const int SLOW_REACT_PROPENSITY = 10;
const int DECIMAL_VALUE = 10;
const int INTEGER_BUFFER_SIZE = 23;      /* Largest unsigned int is 20 bytes + NULL */

extern FILE *yyin;              /* take input from a file not stdin*/
FILE *xmlFile;                  /* write output to a xml file*/
//FILE *xmlSpeciesFile, *xmlReacFile;

const char* const FILE_EXTENSION = "ccx";
char* FILE_NAME = "xmlTest.xml";
%}

%union 
{
    int iValue;                 /* integer value */
    //char sIndex;                /* symbol table index */
    char* id;
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <id> VARIABLE
%token WHILE IF PRINT
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

/*Define type for each non-terminal*/
%type <nPtr> stmt expr stmt_list

%%

program:
        function                { terminate(); }
        ;

function:
          function stmt         { parseTree($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, declareId($1, 0), $3); /*printf("\nvariable declared\n");*/}
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'              { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | VARIABLE              { $$ = checkId($1); /*printf("\nvariable used\n");*/}
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;

%%

nodeType *con(int value) 
{
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *declareId(const char* name, int value) 
{
    nodeType* newTreeNode = NULL;
    	
    if(getSym(name) == NULL)
    {
	/* allocate node in AST */
	if ((newTreeNode = malloc(sizeof(nodeType))) == NULL)
	    yyerror("out of memory");

	/* copy information and put variable inside symbol table */
	newTreeNode->type = typeId;
	newTreeNode->id = putSym(name, value);
     }
    else
    {
	printf("\nSymbol is already declared. Updating..\n");
	/* allocate node in AST */
	if ((newTreeNode = malloc(sizeof(nodeType))) == NULL)
	    yyerror("out of memory");
	
	/* copy information and put variable inside symbol table */
	newTreeNode->type = typeId;
	newTreeNode->id = getSym(name);
    }
	
    return newTreeNode;
}

nodeType *checkId(const char* name) 
{
    nodeType* newTreeNode = NULL;
    symTableNode* tempNode = getSym(name);
    
    if(tempNode != NULL)
    {
	/* allocate node in AST */
	if ((newTreeNode = malloc(sizeof(nodeType))) == NULL)
	    yyerror("out of memory");

	/* copy information and put variable inside symbol table */
	newTreeNode->type = typeId;
	newTreeNode->id = tempNode;
     }
    else
    {
	printf("\nError! No such symbol %s found.\n", name);
	exit(0);
    }
	
    return newTreeNode;
}

nodeType *opr(int oper, int nops, ...) 
{
    va_list ap;
    nodeType *p;
    int i;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");
    if ((p->opr.op = malloc(nops * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) 
{
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
		free (p->opr.op);
    }
    free (p);
}

void yyerror(char *s) 
{
    fprintf(stdout, "%s\n", s);
}

void terminate()
{
    printf("Terminating\n");
    FILE *xmlSpeciesFile, *xmlReacFile;
    char ch;

    xmlSpeciesFile = fopen("tmp/xmlSpeciesFile.xml", "r");
    xmlReacFile = fopen("tmp/xmlReacFile.xml", "r");
    xmlFile = fopen(FILE_NAME, "a+");
    
    while((ch = fgetc(xmlSpeciesFile) ) != EOF )
        fputc(ch, xmlFile);

    fprintf(xmlFile, "%s\n", "</listOfSpecies>\n\n<listOfReactions>");

    while((ch = fgetc(xmlReacFile) ) != EOF )
        fputc(ch, xmlFile);

    fprintf(xmlFile, "%s\n", "</listOfReactions>\n</model>\n</listOfModels>\n<listOfMethods>\n<method equilibrationTime=\"0.0\" startTime=\"0.0\" recordingTime=\"1.0\" id=\"1\" category=\"0\" numberOfBins=\"32\" multiplicity=\"4\" method=\"0\" timeDependence=\"1\" numberOfFrames=\"11\" options=\"0\"/>\n<method equilibrationTime=\"0.0\" startTime=\"0.0\" recordingTime=\"1.0\" id=\"2\" category=\"0\" numberOfBins=\"32\" multiplicity=\"4\" method=\"0\" timeDependence=\"0\" numberOfFrames=\"11\" options=\"0\"/>\n</listOfMethods>\n<random seed=\"2147483648\">\n</random>\n</cain>");

    fclose(xmlFile);
    fclose(xmlSpeciesFile);
    fclose(xmlReacFile);
    printf("Terminated\n");

}

/* Starting point of code */
int main(int argc, char *argv[]) 
{
    
    /* Remove tmp files from last session */
    remove("tmp/xmlSpeciesFile.xml");
    remove("tmp/xmlReacFile.xml");
    remove("xmlFile.xml");
    
    /* uthash lib requires to set symTable pointer be set to null */
    symTable = NULL;
    tempVarTable = NULL;
    _localz.isEmpty = 1;
        
    // Check the file extension and arguments
    strtok(argv[1], ".");
    if(argc == 1)
    {
    	/* Take input from stdin */
    	printf("Chemical Compiler V-1.0 (Interpreter Mode)\n");
    	yyparse();
    }
    else if(argc == 2)
    {
        yyin = fopen(strcat(argv[1], ".ccx"), "r");

        /* fopen returns 0, the NULL pointer, on failure */
        if(!yyin)
        {
            printf("Sorry, error handling the file.\n\n");
        }
        else
        {
            yyparse();
        }
    }
    else if(argc == 3)
    {
        FILE_NAME = strcat(argv[2], ".xml");
        yyin = fopen(strcat(argv[1], ".ccx"), "r");

        /* fopen returns 0, the NULL pointer, on failure */
        if(!yyin)
        {
            printf("Sorry, error handling the file.\n\n");
        }
        else
        {
            yyparse();
        }
    }
    else if(strcmp(strtok(NULL, "."), FILE_EXTENSION) != 0)
    {
        printf("Please enter a chemical copiler file name as argument.\n\n");
    }
    return 0;
}
