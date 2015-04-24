/*
Chemical Compiler -> AST Parser File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 23st March
Date Created - 15th March
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"
#include "y.tab.h"
#include "gdsl.h"
#include "genReaction.h"

/* To avoid warnings, we declare prototypes */
char* decimalToWords(int num);
char* appendString(const char* dest, const char* src);
char* prependString(const char* dest, const char* src);
void ex(FILE *xmlSpeciesFile, FILE *xmlReacFile, nodeType *root);
/*
int ex(nodeType *p) {
    int lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:       
	printf("\tpush\t%d\n", p->con.value); 
	break;
    case typeId:        
	printf("\tpush\t%c\n", p->id.i + 'a'); 
	break;
    case typeOpr:
	switch(p->opr.oper) {
	case WHILE:
	    printf("L%03d:\n", lbl1 = lbl++);
	    ex(p->opr.op[0]);
	    printf("\tjz\tL%03d\n", lbl2 = lbl++);
	    ex(p->opr.op[1]);
	    printf("\tjmp\tL%03d\n", lbl1);
	    printf("L%03d:\n", lbl2);
	    break;
	case IF:
	    ex(p->opr.op[0]);
	    if (p->opr.nops > 2) {
		// if else
		printf("\tjz\tL%03d\n", lbl1 = lbl++);
		ex(p->opr.op[1]);
		printf("\tjmp\tL%03d\n", lbl2 = lbl++);
		printf("L%03d:\n", lbl1);
		ex(p->opr.op[2]);
		printf("L%03d:\n", lbl2);
	    } else {
		// if 
		printf("\tjz\tL%03d\n", lbl1 = lbl++);
		ex(p->opr.op[1]);
		printf("L%03d:\n", lbl1);
	    }
	    break;
	case PRINT:     
	    ex(p->opr.op[0]);
	    printf("\tprint\n");
	    break;
	case '=':       
	    ex(p->opr.op[1]);
	    printf("\tpop\t%c\n", p->opr.op[0]->id.i + 'a');
	    break;
	case UMINUS:    
	    ex(p->opr.op[0]);
	    printf("\tneg\n");
	    break;
	default:
	    ex(p->opr.op[0]);
	    ex(p->opr.op[1]);
	    switch(p->opr.oper) {
	    case '+':   printf("\tadd\n"); break;
	    case '-':   printf("\tsub\n"); break; 
	    case '*':   printf("\tmul\n"); break;
	    case '/':   printf("\tdiv\n"); break;
	    case '<':   printf("\tcompLT\n"); break;
	    case '>':   printf("\tcompGT\n"); break;
	    case GE:    printf("\tcompGE\n"); break;
	    case LE:    printf("\tcompLE\n"); break;
	    case NE:    printf("\tcompNE\n"); break;
	    case EQ:    printf("\tcompEQ\n"); break;
	    }
	}
    }
    return 0;
}
*/
void parseTree(nodeType *root)
{
	static FILE *xmlSpeciesFile, *xmlReacFile;
	// char ch;

	xmlSpeciesFile = fopen("tmp/xmlSpeciesFile.xml", "a+");
	xmlReacFile = fopen("tmp/xmlReacFile.xml", "a+");

	ex(xmlSpeciesFile, xmlReacFile, root);	

	fclose(xmlSpeciesFile);
	fclose(xmlReacFile);
}


void ex(FILE *xmlSpeciesFile, FILE *xmlReacFile, nodeType *root)
{

	static int toInitialize = 0;
	static gdsl_stack_t stack;
	static int REAC_ID = 0;
	
	if(!toInitialize){
		stack = gdsl_stack_alloc("aExpStack", NULL, NULL);
      	fprintf(xmlSpeciesFile, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<cain version=\"1.10\">\n<listOfModels>\n<model id=\"1\">\n<listOfSpecies>\n");
     	toInitialize++;
	}

	if(!root)
	{
		printf("Came here\n");
		return;
	}
    //For arithmetic expression our types can be VARIABLE, CONSTANT and OPERATOR
    //swtich case can be used since ID is associated with each token
	switch(root->type)
	{
		case typeCon:
			gdsl_stack_insert(stack, root);
			char* decimalInWords = decimalToWords(root->con.value);
			if(!getTempConst(root->con.value))
			{
				putTempConst (root->con.value, decimalInWords);
				putSpecies (decimalInWords, root->con.value);
		 		// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(decimalInWords, root->con.value));	
		 		// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(decimalInWords, strcat(root->con.value, "_"));	
		 		// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(decimalInWords, strcat(root->con.value, "ab"));
		 		// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(decimalInWords, strcat(root->con.value, "_"));				
		 	}
		break;

		case typeId:
			gdsl_stack_insert(stack, root);
		break;

		case typeOpr:
			switch(root->opr.oper) 
			{
				case WHILE:
				    ex(xmlSpeciesFile, xmlReacFile, root->opr.op[0]);
				    ex(xmlSpeciesFile, xmlReacFile, root->opr.op[1]);
				    printf("while\n");
				    
			    break;

				case PRINT:     
					ex(xmlSpeciesFile, xmlReacFile, root->opr.op[0]);
					nodeType* rOperand = gdsl_stack_remove(stack);
					printf("\tprint\n");
				break;

				case '=':   
					ex(xmlSpeciesFile, xmlReacFile, root->opr.op[1]);
					if(!gdsl_stack_is_empty(stack))
					{
						nodeType* rOperand = gdsl_stack_remove(stack);
						
						/* Add start to species part */
						char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
						snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID);
						snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID);
						fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start", idBuffer), START_AMOUNT));
						snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID + 5);
						fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start", idBuffer), START_AMOUNT));

						if(rOperand->type == typeCon)
						{
							updateSym(root->opr.op[0]->id->name, rOperand->con.value);

							if(_localz.isEmpty)
							{
								fprintf(xmlReacFile, "%s", genCopyReac (decimalToWords(rOperand->con.value), (char *)root->opr.op[0]->id->name, REAC_ID));
								REAC_ID = REAC_ID + 5;
							}
							else
							{
								fprintf(xmlReacFile, "%s", genCopyReac ("_localz", (char *)root->opr.op[0]->id->name, REAC_ID));
								REAC_ID = REAC_ID + 5;
							}
							if(!getSpecies(decimalToWords(rOperand->con.value)))
										putSpecies (decimalToWords(rOperand->con.value), rOperand->con.value);
						}
						else if(rOperand->type == typeId)
						{
							updateSym(root->opr.op[0]->id->name, rOperand->id->value);

							fprintf(xmlReacFile, "%s", genCopyReac ((char *)rOperand->id->name, (char *)root->opr.op[0]->id->name, REAC_ID));
							REAC_ID = REAC_ID + 5;

							if(!getSpecies(rOperand->id->name))
										putSpecies (rOperand->id->name, 0);
						}
						if(!getSpecies(root->opr.op[0]->id->name))
							putSpecies (root->opr.op[0]->id->name, 0);

						fprintf(xmlReacFile, "%s", genClearReac ("_localz", REAC_ID));
						_localz.isEmpty = 1;
						REAC_ID = REAC_ID + 5;
						remove(idBuffer);
					}
					// printf("\nSYMBOL TABLE\n");
					// printSymTable();
					// printf("\nCONST TABLE\n");
					// printTempConstTable();
					// printf("\n SPECIES TABLE\n");
					// printSpeciesTable();
				break;


				default :
					ex(xmlSpeciesFile, xmlReacFile, root->opr.op[0]);
					ex(xmlSpeciesFile, xmlReacFile, root->opr.op[1]);

					/* FLOW -- KEYWORDS -> OPERATORS -> CONSTANTS*/
					switch(root->opr.oper) 
					{
						case '+':   
							//printf("add\t");
							if(!gdsl_stack_is_empty(stack))
							{
								nodeType* lOperand = gdsl_stack_remove(stack);
								nodeType* rOperand = gdsl_stack_remove(stack);

								/* Add start to species part */
								char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    							snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID);
								fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start", idBuffer), START_AMOUNT));
								snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID + 5);
								fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start", idBuffer), START_AMOUNT));
								//Occupy the register so change state
								_localz.isEmpty = 0;

								//Check type of operands and generate XML code
								if(lOperand->type == typeCon && rOperand->type == typeCon)
								{
									if(!getSpecies(decimalToWords(lOperand->con.value)))
										putSpecies (decimalToWords(lOperand->con.value), lOperand->con.value);
									if(!getSpecies(decimalToWords(rOperand->con.value)))
										putSpecies (decimalToWords(rOperand->con.value), rOperand->con.value);

									fprintf(xmlReacFile, "%s", genCopyReac (decimalToWords(lOperand->con.value), "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;
									
									fprintf(xmlReacFile, "%s", genCopyReac (decimalToWords(rOperand->con.value), "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;

									lOperand->con.value = lOperand->con.value + rOperand->con.value;
								}
								else if(lOperand->type == typeId && rOperand->type == typeId)
								{
									if(!getSpecies(lOperand->id->name))
										putSpecies (lOperand->id->name, 0);
									if(!getSpecies(rOperand->id->name))
										putSpecies (rOperand->id->name, 0);

									fprintf(xmlReacFile, "%s", genCopyReac ((char *)lOperand->id->name, "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;
									
									fprintf(xmlReacFile, "%s", genCopyReac ((char *)rOperand->id->name, "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;

									lOperand->con.value = getSym(lOperand->id->name)->value + getSym(rOperand->id->name)->value; 
									lOperand->type = typeCon;
								}
								else if(lOperand->type == typeCon && rOperand->type == typeId)
								{
									if(!getSpecies(decimalToWords(lOperand->con.value)))
										putSpecies (decimalToWords(lOperand->con.value), lOperand->con.value);
									if(!getSpecies(rOperand->id->name))
										putSpecies (rOperand->id->name, 0);

									fprintf(xmlReacFile, "%s", genCopyReac (decimalToWords(lOperand->con.value), "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;
									
									fprintf(xmlReacFile, "%s", genCopyReac ((char *)rOperand->id->name, "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;

									lOperand->con.value = lOperand->con.value + getSym(rOperand->id->name)->value;
								}
								else if(lOperand->type == typeId && rOperand->type == typeCon)
								{
									if(!getSpecies(lOperand->id->name))
										putSpecies (lOperand->id->name, 0);
									if(!getSpecies(decimalToWords(rOperand->con.value)))
										putSpecies (decimalToWords(rOperand->con.value), rOperand->con.value);

									fprintf(xmlReacFile, "%s", genCopyReac ((char *)lOperand->id->name, "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;
									
									fprintf(xmlReacFile, "%s", genCopyReac (decimalToWords(rOperand->con.value), "_localz", REAC_ID));
									REAC_ID = REAC_ID + 5;

									lOperand->con.value = getSym(lOperand->id->name)->value + rOperand->con.value;
									lOperand->type = typeCon;
								}
								remove(idBuffer);

								gdsl_stack_insert(stack, lOperand);
							}
						break;
					
						case '-':   printf("\tsub\n"); break; 
						
						case '*':   
							printf("\tmul\n"); 
							if(!gdsl_stack_is_empty(stack))
							{
								nodeType* lOperand = gdsl_stack_remove(stack);
								nodeType* rOperand = gdsl_stack_remove(stack);
								//Occupy the register so change state
								_localz.isEmpty = 0;

								/* Add start to species part and var_rx, var_rx_ab*/
								char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    							snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID);
								fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start", idBuffer), START_AMOUNT));
								snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", REAC_ID + 3);
								fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start_add", idBuffer), START_AMOUNT));
								fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString("start_sub", idBuffer), START_AMOUNT));

								//Check type of operands and generate XML code. Also, generate species for the same.
								if(lOperand->type == typeCon && rOperand->type == typeCon)
								{
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(lOperand->con.value), "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(rOperand->con.value), "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(lOperand->con.value), "_rx_ab"), START_AMOUNT));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(rOperand->con.value), "_rx_ab"), START_AMOUNT));
									if(lOperand->con.value > rOperand->con.value)
									{
										char* rxSpecies = appendString(decimalToWords(rOperand->con.value), "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);
									}
									else
									{
										char* rxSpecies = appendString(decimalToWords(lOperand->con.value), "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);	
									}

									fprintf(xmlReacFile, "%s", genMulReac (decimalToWords(lOperand->con.value), decimalToWords(rOperand->con.value),"_localz", lOperand->con.value > rOperand->con.value, REAC_ID));
									REAC_ID = REAC_ID + 16;

									lOperand->con.value = lOperand->con.value * rOperand->con.value;
								}
								else if(lOperand->type == typeId && rOperand->type == typeId)
								{
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)lOperand->id->name, "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)rOperand->id->name, "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)lOperand->id->name, "_rx_ab"), START_AMOUNT));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)rOperand->id->name, "_rx_ab"), START_AMOUNT));
									if(getSym(lOperand->id->name)->value > getSym(rOperand->id->name)->value)
									{
										char* rxSpecies = appendString(rOperand->id->name, "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);
									}
									else
									{
										char* rxSpecies = appendString(lOperand->id->name, "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);
									}

									fprintf(xmlReacFile, "%s", genMulReac ((char *)lOperand->id->name, (char *)rOperand->id->name,"_localz", getSym(lOperand->id->name)->value > getSym(rOperand->id->name)->value, REAC_ID));
									REAC_ID = REAC_ID + 16;

									lOperand->con.value = getSym(lOperand->id->name)->value * getSym(rOperand->id->name)->value; 
									lOperand->type = typeCon;
								}
								else if(lOperand->type == typeCon && rOperand->type == typeId)
								{
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(lOperand->con.value), "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)lOperand->id->name, "_rx_ab"), START_AMOUNT));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(lOperand->con.value), "_rx_ab"), START_AMOUNT));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)rOperand->id->name, "_rx_ab"), START_AMOUNT));
									if(lOperand->con.value > getSym(rOperand->id->name)->value)
									{
										char* rxSpecies = appendString(rOperand->id->name, "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);
									}
									else
									{
										char* rxSpecies = appendString(decimalToWords(lOperand->con.value), "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);	
									}

									fprintf(xmlReacFile, "%s", genMulReac (decimalToWords(lOperand->con.value), (char *)rOperand->id->name,"_localz", lOperand->con.value > getSym(rOperand->id->name)->value, REAC_ID));
									REAC_ID = REAC_ID + 16;

									lOperand->con.value = lOperand->con.value * getSym(rOperand->id->name)->value; 
								}
								else if(lOperand->type == typeId && rOperand->type == typeCon)
								{
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)lOperand->id->name, "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(rOperand->con.value), "_rx"), 0));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString((char *)lOperand->id->name, "_rx_ab"), START_AMOUNT));
									// fprintf(xmlSpeciesFile, "%s", genSpeciesCode(appendString(decimalToWords(rOperand->con.value), "_rx_ab"), START_AMOUNT));
									if(getSym(lOperand->id->name)->value > rOperand->con.value)
									{
										char* rxSpecies = appendString(decimalToWords(rOperand->con.value), "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);
									}
									else
									{
										char* rxSpecies = appendString(lOperand->id->name, "_rx");
										if(!getSpecies(rxSpecies))
											putSpecies (rxSpecies, 0);
									}
									
									fprintf(xmlReacFile, "%s", genMulReac ((char *)lOperand->id->name, decimalToWords(rOperand->con.value),"_localz", getSym(lOperand->id->name)->value > rOperand->con.value, REAC_ID));
									REAC_ID = REAC_ID + 16;

									lOperand->con.value = getSym(lOperand->id->name)->value * rOperand->con.value; 
									lOperand->type = typeCon;
								}

								gdsl_stack_insert(stack, lOperand);
							}
						break;
						
						case '/':   printf("\tdiv\n"); break;
						case '<':   printf("\tcompLT\n"); break;
						case '>':   printf("\tcompGT\n"); break;
						case GE:    printf("\tcompGE\n"); break;
						case LE:    printf("\tcompLE\n"); break;
						case NE:    printf("\tcompNE\n"); break;
						case EQ:    printf("\tcompEQ\n"); break;
					}
				break;
			}
		break;
	}
}