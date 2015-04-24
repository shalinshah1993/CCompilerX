/*
Chemical Compiler -> XML Code Generation Modules
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 23rd April
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

char* genSpeciesCode(char* name, int amount)
{
    char *amountBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);    
    snprintf(amountBuffer, INTEGER_BUFFER_SIZE - 1,"%d", amount);

    char* outputString = appendString("<species initialAmount=\"", amountBuffer);
    outputString = appendString(outputString, "\" id=\"");
    outputString = appendString(outputString, name);
    outputString = appendString(outputString, "\"/>\n");

    remove(amountBuffer);
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

/* ********* Multiply Reactions **********
 * while (x > 0){
 *  z = z + y;
 *  x = x - 1;
 * }
 */
char* genMulReac (char* varA, char* varB, char* dest, int greater, int ID)
{
    char* outputString = appendString("\n<!-- Multiplication Reactions for ", varA);
    outputString = appendString(outputString, " and ");
    outputString = appendString(outputString, varB);  
    outputString = appendString(outputString, " -->\n"); 

    char* propensityBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID);
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);

    char* start = appendString("start", idBuffer);
    char* start_add;
    char* start_sub;
    
    char* varAComp = appendString(varA, "_");
    char* varBComp = appendString(varB, "_");
    char* varA_ab = appendString(varA, "_ab");
    char* varA_ab_ = appendString(varA, "_ab_");
    char* varB_ab_ = appendString(varB, "_ab_");

    /* Select the smaller number for looping - x */     
    /* SEQUENCE - CHECK -> COPY -> DECREMENT */
    if(greater)
    {
        /* ---------------------- Check Reactions(B smaller)  ---------------------- */
        // Reaction 1 - start + var1 + var2 ---slow--> var1 + start_add
        char* reactantList[] = {start, varA_ab_, varB_ab_};
        char* productList[] = {varB, start, "", ""};    /* Decrement B since smaller*/
        
        outputString = appendString(outputString, genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));

        //Reaction 2 - start + var1' ---fast--> var1'
        reactantList[0] = start;
        reactantList[1] = varAComp;
        productList[0] = varAComp;
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 1);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        
        outputString = appendString(outputString, genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        //Reaction 3 - start + var2' ---fast--> var2'
        reactantList[0] = start;
        reactantList[1] = varBComp;
        productList[0] = varBComp;
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 2);
        
        outputString = appendString(outputString, genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        //Reaction 4 - start ---slow--> start_add + start_sub
        reactantList[0] = start;
        productList[0] = start_add;
        productList[1] = start_sub;
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 3);
        start_add = appendString("start_add", idBuffer);
        start_sub = appendString("start_sub", idBuffer);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        
        outputString = appendString(outputString, genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 1, productList, 0, 2));

        /* ---------------------- Copy Reactions ---------------------- */
        //Start in copy reactions same as start_add of Raction 4
        //varA will be copied since bigger in value, dest = _localy
        char* varName = varA;
        char* varNameComp = varAComp;
        char* copyOutputString = "\n<!-- Copy Reactions -->";
        reactantList[0] = varName;
        reactantList[1] = start_add;
        reactantList[2] = "done_ab";
        productList[0] = varNameComp;
        productList[1] = start_add;
        
        // Reaction 5.1 -  var + start + done(ab) ---fast--> var' + start
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 0);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));
        
        //Reaction 5.2 - start + var(ab) ---slow--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 1);
        reactantList[0] = start_add; 
        reactantList[1] = appendString(varName, "_ab");
        productList[0] = "done";
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        //Reaction 5.3 - done + done ---fast--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 4 + 2);
        reactantList[0] = "done";
        reactantList[1] = "done";
        productList[0] = "done";
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));
        
        //Reaction 5.4 - done + var' ---slow--> done + var + dest
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 3);
        reactantList[0] = "done";
        reactantList[1] = varNameComp;
        productList[0] = "done";
        productList[1] = varName;
        productList[2] = dest;
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 3));

        //Reaction 5.5 - done + start ---fast--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 4);
        reactantList[0] = "done";
        reactantList[1] = start_add;
        productList[0] = "done";
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));
            
        
        /* ---------------------- Decrement Reactions ---------------------- */
        //Start in decrement reactions same as start_sub of Reaction 4
        //varB will be copied to varName 
        varName = varB;
        varNameComp = varBComp;
        char* decOutputString = "\n<!-- Decrement Reactions -->\n";
        reactantList[0] = varName;
        reactantList[1] = start_sub;
        reactantList[2] = "done_ab";
        productList[0] = varNameComp;
        productList[1] = start_sub;

        // Reaction 6.1 -  var + start + done(ab) ---slow--> var' + start
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 5 + 4 + 0);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));

        //Reaction 6.2 - start + var(ab) ---slow--> done
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 1);
        reactantList[0] = start_sub; 
        reactantList[1] = appendString(varName, "_ab");
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        //Reaction 6.3 - done + varName' + varName' ---fast--> done + varName + varName' + varName^(rx)
        char* reac3String = "";
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 2);

        reac3String = appendString(reac3String, "<reaction propensity=\"1000\" massAction=\"true\" id=\"_");
        reac3String = appendString(reac3String, idBuffer);
        reac3String = appendString(reac3String, "\">\n<listOfReactants>\n");
        reac3String = appendString(reac3String, "<speciesReference stoichiometry=\"2");
        reac3String = appendString(reac3String, "\" species=\"");
        reac3String = appendString(reac3String, varNameComp);
        reac3String = appendString(reac3String, "\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"done\"/>\n");
        reac3String = appendString(reac3String, "</listOfReactants>\n<listOfProducts>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"done\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"");
        reac3String = appendString(reac3String, varName);
        reac3String = appendString(reac3String, "\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"");
        reac3String = appendString(reac3String, varNameComp);
        reac3String = appendString(reac3String, "\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"");
        reac3String = appendString(reac3String, appendString(varName, "_rx"));
        reac3String = appendString(reac3String, "\"/>\n</listOfProducts>\n</reaction>\n");

        decOutputString = appendString(decOutputString, reac3String);

        //Reaction 6.4 - varName^(rx) ---slow--> 0
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 3);
        reactantList[0] = appendString(varName, "_rx");
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 1, productList, 0, 0));

        //Reaction 6.5 - done + varName^(rx)_(ab) + varName' ---slow--> done
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 4);
        reactantList[0] = "done";
        reactantList[1] = appendString(varName, "_rx_ab");
        reactantList[2] = varNameComp;
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 1));    

        //Reaction 6.6 - done + done ---fast--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 5);
        reactantList[0] = "done";
        reactantList[1] = "done";
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));

        //Reaction 6.7 - done + start ---fast--> done
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 6);
        reactantList[0] = "done";
        reactantList[1] = start_sub;
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        outputString = appendString(outputString, copyOutputString);
        outputString = appendString(outputString, decOutputString);
    }
    else
    {
       /* ---------------------- Check Reactions(A smaller)  ---------------------- */
        // Reaction 1 - start + var1 + var2 ---slow--> var1 + start_add
        char* reactantList[] = {start, varA_ab_, varB_ab_};
        char* productList[] = {varA, start, "", ""};    /* Decrement B since smaller*/
        
        genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2);

        //Reaction 2 - start + var1' ---fast--> var1'
        reactantList[0] = start;
        reactantList[1] = varBComp;
        productList[0] = varBComp;
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 1);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        
        genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1);

        //Reaction 3 - start + var2' ---fast--> var2'
        reactantList[0] = start;
        reactantList[1] = varAComp;
        productList[0] = varAComp;
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 2);
        
        genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1);

        //Reaction 4 - start ---slow--> start_add + start_sub
        reactantList[0] = start;
        productList[0] = start_add;
        productList[1] = start_sub;
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 3);
        start_add = appendString("start_add", idBuffer);
        start_sub = appendString("start_sub", idBuffer);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        
        genReac (propensityBuffer, "true", idBuffer, reactantList, 0, 1, productList, 0, 2);

        /* ---------------------- Copy Reactions ---------------------- */
        //Start in copy reactions same as start_add of Raction 4
        //varA will be copied since bigger in value, dest = _localy
        char* varName = varB;
        char* dest = "_localy";
        char* varNameComp = varBComp;
        char* copyOutputString = "\n<!-- Copy Reactions -->";
        reactantList[0] = varName;
        reactantList[1] = start_add;
        reactantList[2] = "done_ab";
        productList[0] = varNameComp;
        productList[1] = start_add;
        
        // Reaction 5.1 -  var + start + done(ab) ---fast--> var' + start
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 0);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));
        
        //Reaction 5.2 - start + var(ab) ---slow--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 1);
        reactantList[0] = start_add; 
        reactantList[1] = appendString(varName, "_ab");
        productList[0] = "done";
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        //Reaction 5.3 - done + done ---fast--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 4 + 2);
        reactantList[0] = "done";
        reactantList[1] = "done";
        productList[0] = "done";
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));
        
        //Reaction 5.4 - done + var' ---slow--> done + var + dest
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 3);
        reactantList[0] = "done";
        reactantList[1] = varNameComp;
        productList[0] = "done";
        productList[1] = varName;
        productList[2] = dest;
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 3));

        //Reaction 5.5 - done + start ---fast--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 4 + 4);
        reactantList[0] = "done";
        reactantList[1] = start_add;
        productList[0] = "done";
        copyOutputString = appendString(copyOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));
            
        
        /* ---------------------- Decrement Reactions ---------------------- */
        //Start in decrement reactions same as start_sub of Reaction 4
        //varB will be copied to varName 
        varName = varA;
        varNameComp = varAComp;
        char* decOutputString = "\n<!-- Decrement Reactions -->\n";
        reactantList[0] = varName;
        reactantList[1] = start_sub;
        reactantList[2] = "done_ab";
        productList[0] = varNameComp;
        productList[1] = start_sub;

        // Reaction 6.1 -  var + start + done(ab) ---slow--> var' + start
        snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID + 5 + 4 + 0);
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));

        //Reaction 6.2 - start + var(ab) ---slow--> done
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 1);
        reactantList[0] = start_sub; 
        reactantList[1] = appendString(varName, "_ab");
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        //Reaction 6.3 - done + varName' + varName' ---fast--> done + varName + varName' + varName^(rx)
        char* reac3String = "";
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 2);

        reac3String = appendString(reac3String, "<reaction propensity=\"1000\" massAction=\"true\" id=\"_");
        reac3String = appendString(reac3String, idBuffer);
        reac3String = appendString(reac3String, "\">\n<listOfReactants>\n");
        reac3String = appendString(reac3String, "<speciesReference stoichiometry=\"2");
        reac3String = appendString(reac3String, "\" species=\"");
        reac3String = appendString(reac3String, varNameComp);
        reac3String = appendString(reac3String, "\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"done\"/>\n");
        reac3String = appendString(reac3String, "</listOfReactants>\n<listOfProducts>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"done\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"");
        reac3String = appendString(reac3String, varName);
        reac3String = appendString(reac3String, "\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"");
        reac3String = appendString(reac3String, varNameComp);
        reac3String = appendString(reac3String, "\"/>\n");
        reac3String = appendString(reac3String, "<speciesReference species=\"");
        reac3String = appendString(reac3String, appendString(varName, "_rx"));
        reac3String = appendString(reac3String, "\"/>\n</listOfProducts>\n</reaction>\n");

        decOutputString = appendString(decOutputString, reac3String);

        //Reaction 6.4 - varName^(rx) ---slow--> 0
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 3);
        reactantList[0] = appendString(varName, "_rx");
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 1, productList, 0, 0));

        //Reaction 6.5 - done + varName^(rx)_(ab) + varName' ---slow--> done
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 4);
        reactantList[0] = "done";
        reactantList[1] = appendString(varName, "_rx_ab");
        reactantList[2] = varNameComp;
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 1));    

        //Reaction 6.6 - done + done ---fast--> done
        snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 5);
        reactantList[0] = "done";
        reactantList[1] = "done";
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));

        //Reaction 6.7 - done + start ---fast--> done
        snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5 + 4 + 6);
        reactantList[0] = "done";
        reactantList[1] = start_sub;
        productList[0] = "done";
        decOutputString = appendString(decOutputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

        outputString = appendString(outputString, copyOutputString);
        outputString = appendString(outputString, decOutputString);
    }
    
    /* Free up resources */
    remove(propensityBuffer);
    remove(idBuffer);

    return outputString;
}

/* ********* Decrement Reactions **************  
 * varName + start + done(ab) ---slow--> varName' + start
 * start + varName(ab) ---slow--> done
 * done + varName' + varName' ---fast--> done + varName + varName' + varName^(rx)
 * varName^(rx) ---slow--> 0
 * done + varName^(rx)_(ab) + varName' ---slow--> done
 * done + done ---fast--> done
 * done + start ---fast--> done
 */
char* genDecReac (char* varName, int ID)
{
    char* outputString = appendString("\n<!-- Decrement Reactions for ", varName);
    outputString = appendString(outputString, " -->\n");

    char* propensityBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    char* idBuffer = (char *) malloc(INTEGER_BUFFER_SIZE);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE - 1,"%d", ID);
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", SLOW_REACT_PROPENSITY);

    char* varNameComp = appendString(varName, "_");
    char* startMolecule = appendString("start", idBuffer);
    char* reactantList[] = {varName, startMolecule, "done_ab"};
    char* productList[] = {startMolecule, varNameComp, "", ""};

    // Reaction 1 -  var + start + done(ab) ---slow--> var' + start
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 2));

    //Reaction 2 - start + var(ab) ---slow--> done
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 1);
    reactantList[0] = startMolecule; 
    reactantList[1] = appendString(varName, "_ab");
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));

    //Reaction 3 - done + varName' + varName' ---fast--> done + varName + varName' + varName^(rx)
    char* reac3String = "";
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 2);
    
    reac3String = appendString(reac3String, "<reaction propensity=\"1000\" massAction=\"true\" id=\"_");
    reac3String = appendString(reac3String, idBuffer);
    reac3String = appendString(reac3String, "\">\n<listOfReactants>\n");
    reac3String = appendString(reac3String, "<speciesReference stoichiometry=\"2");
    reac3String = appendString(reac3String, "\" species=\"");
    reac3String = appendString(reac3String, varNameComp);
    reac3String = appendString(reac3String, "\"/>\n");
    reac3String = appendString(reac3String, "<speciesReference species=\"done\"/>\n");
    reac3String = appendString(reac3String, "</listOfReactants>\n<listOfProducts>\n");
    reac3String = appendString(reac3String, "<speciesReference species=\"done\"/>\n");
    reac3String = appendString(reac3String, "<speciesReference species=\"");
    reac3String = appendString(reac3String, varName);
    reac3String = appendString(reac3String, "\"/>\n");
    reac3String = appendString(reac3String, "<speciesReference species=\"");
    reac3String = appendString(reac3String, varNameComp);
    reac3String = appendString(reac3String, "\"/>\n");
    reac3String = appendString(reac3String, "<speciesReference species=\"");
    reac3String = appendString(reac3String, appendString(varName, "_rx"));
        reac3String = appendString(reac3String, "\"/>\n</listOfProducts>\n</reaction>\n");
    
    outputString = appendString(outputString, reac3String);

    //Reaction 4 - varName^(rx) ---slow--> 0
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 3);
    reactantList[0] = appendString(varName, "_rx");
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 1, productList, 0, 0));

    //Reaction 5 - done + varName^(rx)_(ab) + varName' ---slow--> done
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 4);
    reactantList[0] = "done";
    reactantList[1] = appendString(varName, "_rx_ab");
    reactantList[2] = varNameComp;
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 3, productList, 0, 1));    

    //Reaction 6 - done + done ---fast--> done
    snprintf(propensityBuffer, INTEGER_BUFFER_SIZE - 1,"%d", FAST_REACT_PROPENSITY);
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 5);
    reactantList[0] = "done";
    reactantList[1] = "done";
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 1, 2, productList, 0, 1));
    
    //Reaction 7 - done + start ---fast--> done
    snprintf(idBuffer, INTEGER_BUFFER_SIZE,"%d", ID + 6);
    reactantList[0] = "done";
    reactantList[1] = startMolecule;
    productList[0] = "done";
    outputString = appendString(outputString, genReac(propensityBuffer, "true", idBuffer, reactantList, 0, 2, productList, 0, 1));
    
    /* Free up resources */
    remove(idBuffer);
    remove(propensityBuffer);

    return outputString;
}