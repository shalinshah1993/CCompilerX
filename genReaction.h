/*
Chemical Compiler -> XML Code Generation Modules
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 23rd April
Date Created - 3rd April
*/

char* genReac(char* propensity, char* massAction, char* ID, char* reactantList[], int isReacRepeated, int noOfReactants, char* productList[], int isProdRepeated, int noOfProducts);
char* genCopyReac (char* varName, char* dest, int ID);
char* genClearReac(char* varName, int ID);
char* genSpeciesCode(char* name, int amount);
char* genMulReac (char* numA, char* numB, char* dest, int greater, int ID);
char* genDecReac (char* varName, int ID);
char* genSubReac (char* numA, char* numB, int ID);

extern const int FAST_REACT_PROPENSITY; 
extern const int SLOW_REACT_PROPENSITY;
extern const int DECIMAL_VALUE;
extern const int INTEGER_BUFFER_SIZE;