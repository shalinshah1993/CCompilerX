/*
Chemical Compiler -> Lexical Analyzer File
Author - Shalin Shah
Mentor - Manish K Gupta
Last Modified - 31st March
Date Created - 15th March
*/

%{
#include <stdlib.h>
#include "symbolTable.h"
#include "y.tab.h"
void yyerror(char *);
%}

IDENTIFIER	[a-z][a-z0-9]*
WHITESPACE	[ \t\n]+ 
%%

0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[-()<>=+*/;{}.] {
		    return *yytext;
		}

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;

{IDENTIFIER} {
		yylval.id = (char *) strdup(yytext);
                return VARIABLE;
             }


{WHITESPACE}       ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%

int yywrap(void) {
    return 1;
}
