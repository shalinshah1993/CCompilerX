/*
Chemical Compiler -> XML Code Generation Modules
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 3rd April
Date Created - 3rd April
*/

#include <stdio.h>
#include "extraModules.h"
#include <string.h>
#include <stdlib.h>

char* appendString(const char* old, const char* new)
{
    // find the size of the string to allocate
    const size_t old_len = strlen(old), new_len = strlen(new);
    const size_t out_len = old_len + new_len + 1;

    // allocate a pointer to the new string
    char *out = malloc(out_len);

    // concat both strings and return
    memcpy(out, old, old_len);
    memcpy(out + old_len, new, new_len + 1);
    
    return out;
}

char* prependString(const char* old, const char* new)
{
    // find the size of the string to allocate
    const size_t old_len = strlen(old), new_len = strlen(new);
    const size_t out_len = old_len + new_len + 1;

    // allocate a pointer to the new string
    char *out = malloc(out_len);

    // concat both strings and return
    //memcpy(out, old, old_len);
    memcpy(out, new, new_len);
    memcpy(out + new_len, old, old_len + 1);
    
    return out;
}

char* decimalToWords(int num)
{
    int remainder;
    char* outputString = "";
    
    while(num > 0)
    {
	remainder = num % 10;
	num /= 10;
	
	switch(remainder)
	{
	  case 1:
	    outputString = prependString(outputString, "_one");
	    break;
	    
	  case 2:
	    outputString = prependString(outputString, "_two");
	    break;
	    
	  case 3:
	    outputString = prependString(outputString, "_three");
	    break;
	    
	  case 4:
	    outputString = prependString(outputString, "_four");
	    break;
	    
	  case 5:
	    outputString = prependString(outputString, "_five");
	    break;
	    
	  case 6:
	    outputString = prependString(outputString, "_six");
	    break;
	    
	  case 7:
	    outputString = prependString(outputString, "_seven");
	    break;
	    
	  case 8:
	    outputString = prependString(outputString, "_eight");
	    break;
	    
	  case 9:
	    outputString = prependString(outputString, "_nine");
	    break;
	    
	  default:
	    outputString = prependString(outputString, "_zero");
	    break; 
	}
    }
    
    //printf("%s string value", ++outputString);
    
    return ++outputString;
}