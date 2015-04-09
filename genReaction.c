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
    char propensityBuffer[4], idBuffer[4];
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID);
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE,"%d", FAST_REACT_PROPENSITY);
  
    char* varNameComp = appendString(varName, "_");
    char* startMolecule = appendString("start", idBuffer);
    char* reactantList[] = {varName, startMolecule, "done_ab"};
    char* productList[] = {startMolecule, varNameComp};
   
    // Reaction 1 -  var + start + done(ab) ---fast--> var' + start
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));
    
    //Reaction 2 - start + var(ab) ---slow--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE,"%d", SLOW_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 1);
    reactantList[0] = startMolecule; 
    reactantList[1] = appendString(varName, "_ab");
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

    //Reaction 3 - done + done ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 2);
    reactantList[0] = "done";
    reactantList[1] = "done";
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));
    
    //Reaction 4 - done + var' ---slow--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE,"%d", SLOW_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 3);
    reactantList[0] = "done";
    reactantList[1] = appendString(varName, "_");
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

    //Reaction 5 - done + start ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 4);
    reactantList[0] = "done";
    reactantList[1] = startMolecule;
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));
    
    printf("%s \n", outputString);
    return outputString;
}

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
	char stoicBuffer[4];
	snprintf(stoicBuffer, INTEGER_BUFFER_SIZE,"%d", noOfReactants);
	
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
	char stoicBuffer[4];
	snprintf(stoicBuffer, INTEGER_BUFFER_SIZE,"%d", noOfReactants);
	
	outputString = appendString(outputString, "<speciesReference stoichiometry=\"");
	outputString = appendString(outputString, stoicBuffer);
	outputString = appendString(outputString, "\" species=\"");
	outputString = appendString(outputString, reactantList[0]);
	outputString = appendString(outputString, "\"/>\n");
    }
    outputString = appendString(outputString, "</listOfProducts>\n</reaction>\n");
    
    return outputString;
}