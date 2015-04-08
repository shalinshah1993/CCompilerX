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

char* appendString(char* dest, char* src)
{
    char *buffer = (char *)malloc(strlen(src) + strlen(dest) + 1);
    
    if(buffer != NULL)
    {
	strcpy(buffer, dest);
	strcat(buffer, src);
    }
    else
    {
      printf("\nError! Out of memory.\n");
    }
    
    return buffer;
}
