/*
Chemical Compiler -> AST Parser File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 31st March
Date Created - 15th March
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"
#include "y.tab.h"
#include "gdsl.h"
#include "genReaction.h"

//static int lbl;
//static gdsl_stack_t stack;
//extern struct symTableNode* symTable;

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
void generateXMLCode();

int ex(nodeType *root){
  
    static int toInitialize = 0;
    static gdsl_stack_t stack;
    static int REAC_ID = 0;
    
    if(!toInitialize){
	stack = gdsl_stack_alloc("aExpStack", NULL, NULL);
	//symTable = NULL;
	toInitialize++;
    }
    static char* xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?> \
    <cain version=\"1.10\">";
    
    if(!root)
	return 0;
    
    //For arithmetic expression our types can be VARIABLE, CONSTANT and OPERATOR
    //swtich case can be used since ID is associated with each token
    switch(root->type)
    {
	case typeCon:
		    gdsl_stack_insert(stack, root);
		    break;
	    
	case typeId:
		    gdsl_stack_insert(stack, root);
		    break;
		     
	case typeOpr:
		    switch(root->opr.oper) 
		    {
			case PRINT:     
				  ex(root->opr.op[0]);
				  printf("\tprint\n");
				  break;
				  
			case '=':   
				  ex(root->opr.op[1]);
				  if(!gdsl_stack_is_empty(stack))
				  {
				      nodeType* rOperand = gdsl_stack_remove(stack);
				      
				      if(rOperand->type == typeCon)
				      {
					  updateSym(root->opr.op[0]->id->name, rOperand->con.value);
				      }
				      else if(rOperand->type == typeId)
				      {
					  updateSym(root->opr.op[0]->id->name, rOperand->id->value);
				      }
				  }
// 				  printf("\nSYMBOL TABLE\n");
// 				  printSymTable();
				  break;
				  
			default :
				  ex(root->opr.op[0]);
				  ex(root->opr.op[1]);
				  
				  /* FLOW -- KEYWORDS -> OPERATORS -> CONSTANTS*/
				  switch(root->opr.oper) 
				  {
				      case '+':   
						//printf("add\t");
						if(!gdsl_stack_is_empty(stack)){
						    nodeType* lOperand = gdsl_stack_remove(stack);
						    nodeType* rOperand = gdsl_stack_remove(stack);
						    
						    //Check type of operands and generate XML code
						    if(lOperand->type == typeCon && rOperand->type == typeCon)
						    {
							lOperand->con.value = lOperand->con.value + rOperand->con.value;
						    }
						    else if(lOperand->type == typeId && rOperand->type == typeId)
						    {
							lOperand->con.value = getSym(lOperand->id->name)->value + getSym(rOperand->id->name)->value; 
							lOperand->type = typeCon;
						    }
						    else if(lOperand->type == typeCon && rOperand->type == typeId)
						    {
							lOperand->con.value = lOperand->con.value + getSym(rOperand->id->name)->value;
						    }
						    else if(lOperand->type == typeId && rOperand->type == typeCon)
						    {
							lOperand->con.value = getSym(lOperand->id->name)->value + rOperand->con.value;
							lOperand->type = typeCon;
						    }
						    
						    gdsl_stack_insert(stack, lOperand);
						    generateXMLCode();
						    
						    //printf("\nSize %lu , Value %d\n",gdsl_stack_get_size(stack), lOperand->con.value);
						}
						
						break;
						
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
}

void generateXMLCode(){}