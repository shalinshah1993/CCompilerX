/*
Chemical Compiler -> XML Code Generation Modules
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 3rd April
Date Created - 3rd April
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "genReaction.h"

/* To avoid warnings, we declare prototypes */
char* appendString(const char* dest, const char* src);
char* prependString(const char* dest, const char* src);

char* genReac(char* propensity, char* massAction, char* ID, char* reactantList[], int isReacRepeated, int noOfReactants, char* productList[], int isProdRepeated, int noOfProducts)
{
    char* outputString = "";
    
    outputString = appendString(outputString, "<reaction propensity=\"");
    outputString = appendString(outputString, propensity);
    outputString = appendString(outputString,"\" massAction=\"");
    outputString = appendString(outputString, massAction);
    outputString = appendString(outputString,"\" id=\"_");
    outputString = appendString(outputString, ID);
    outputString = appendString(outputString, "\">\n<listOfReactants>\n");
    
    if(!isReacRepeated)
    {
    	while(noOfReactants > 0 && reactantList != NULL)
    	{
    	    outputString = appendString(outputString, "<speciesReference species=\"");
    	    outputString = appendString(outputString, reactantList[noOfReactants-1]);
    	    outputString = appendString(outputString, "\"/>\n");
    	    noOfReactants--;
    	}
    }
    else
    {
    	char* stoicBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    	snprintf(stoicBuffer, INTEGER_BUFFER_SIZE - 1,"%d", noOfReactants);
    	
    	outputString = appendString(outputString, "<speciesReference stoichiometry=\"");
    	outputString = appendString(outputString, stoicBuffer);
    	outputString = appendString(outputString, "\" species=\"");
    	outputString = appendString(outputString, reactantList[0]);
    	outputString = appendString(outputString, "\"/>\n");
    }
    
    outputString = appendString(outputString, "</listOfReactants>\n<listOfProducts>\n");
    
    if(!isProdRepeated)
    {
    	while(noOfProducts > 0 && productList != NULL)
    	{
    	    outputString = appendString(outputString, "<speciesReference species=\"");
    	    outputString = appendString(outputString, productList[noOfProducts-1]);
    	    outputString = appendString(outputString, "\"/>\n");
    	    noOfProducts--;
    	}
    }
    else
    {
    	char* stoicBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
        snprintf(stoicBuffer, INTEGER_BUFFER_SIZE - 1,"%d", noOfReactants);
    	
    	outputString = appendString(outputString, "<speciesReference stoichiometry=\"");
    	outputString = appendString(outputString, stoicBuffer);
    	outputString = appendString(outputString, "\" species=\"");
    	outputString = appendString(outputString, reactantList[0]);
    	outputString = appendString(outputString, "\"/>\n");
    }
    outputString = appendString(outputString, "</listOfProducts>\n</reaction>\n");
    
    return outputString;
}

/* ********* Clear Reactions **************  
 * var + start + done(ab) ---fast--> var' + start
 * start + var(ab) ---slow--> done
 * done + done ---fast--> done
 * done + var' ---slow--> done
 * done + start ---fast--> done
 */
char* genClearReac(char* varName, int ID)
{
    char* outputString = appendString("\n<!-- Clear Reactions for ", varName);
    outputString = appendString(outputString, " -->\n");
    
    char* propensityBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID);
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
  
    char* varNameComp = appendString(varName, "_");
    char* startMolecule = appendString("start", idBuffer);
    char* reactantList[] = {varName, startMolecule, "done_ab"};
    char* productList[] = {startMolecule, varNameComp};
   
    // Reaction 1 -  var + start + done(ab) ---fast--> var' + start
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));
    
    //Reaction 2 - start + var(ab) ---slow--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 1);
    reactantList[0] = startMolecule; 
    reactantList[1] = appendString(varName, "_ab");
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

    //Reaction 3 - done + done ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 2);
    reactantList[0] = "done";
    reactantList[1] = "done";
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));
    
    //Reaction 4 - done + var' ---slow--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 3);
    reactantList[0] = "done";
    reactantList[1] = appendString(varName, "_");
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

    //Reaction 5 - done + start ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 4);
    reactantList[0] = "done";
    reactantList[1] = startMolecule;
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));
    
    //printf("%s \n", outputString);
    return outputString;
}


/* ********* Copy Reactions **************  
 * var + start + done(ab) ---fast--> var' + start
 * start + var(ab) ---slow--> done
 * done + done ---fast--> done
 * done + var' ---slow--> done + var + dest
 * done + start ---fast--> done
 */
char* genCopyReac (char* varName, char* dest, int ID)
{
    char* outputString = appendString("\n<!-- Copy Reactions for ", varName);
    outputString = appendString(outputString, " -->\n");
    
    char* propensityBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID);
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
  
    char* varNameComp = appendString(varName, "_");
    char* startMolecule = appendString("start", idBuffer);
    char* reactantList[] = {varName, startMolecule, "done_ab"};
    char* productList[] = {startMolecule, varNameComp, ""};
   
    // Reaction 1 -  var + start + done(ab) ---fast--> var' + start
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));
    
    //Reaction 2 - start + var(ab) ---slow--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 1);
    reactantList[0] = startMolecule; 
    reactantList[1] = appendString(varName, "_ab");
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

    //Reaction 3 - done + done ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 2);
    reactantList[0] = "done";
    reactantList[1] = "done";
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));
    
    //Reaction 4 - done + var' ---slow--> done + var + dest
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 3);
    reactantList[0] = "done";
    reactantList[1] = appendString(varName, "_");
    productList[0] = "done";
    productList[1] = varName;
    productList[2] = dest;
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 3));

    //Reaction 5 - done + start ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4);
    reactantList[0] = "done";
    reactantList[1] = startMolecule;
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));
        
    /* Free up resources */
    remove(idBuffer);
    remove(propensityBuffer);

    //printf("%s \n", outputString);
    return outputString;
}

char* genSpeciesCode(char* name, int amount)
{
    char *amountBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);    
    snprintf(amountBuffer, INTEGER_BUFFER_SIZE - 1,"%d", amount);

    char* outputString = appendString("<species initialAmount=\"", amountBuffer);
    outputString = appendString(outputString, amountBuffer);
    outputString = appendString(outputString, "\" name=\"");
    outputString = appendString(outputString, name);
    outputString = appendString(outputString, "\"/>\n");

    remove(amountBuffer);
    return outputString;
}
