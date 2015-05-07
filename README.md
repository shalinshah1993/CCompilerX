DNA based chemical compiler - CCompiler X
========================================================

This is an attempt to build a trans-compiler which can take C like code as input and generates chemical reactions XML file as output supported by [CAIN](http://www.cacr.caltech.edu/~sean/cain/Welcome.htm). This XML file can be converted to [Visual DSD](http://boson.research.microsoft.com/webdna/) supported file using [CRN2DSD](http://guptalab.org/crn2dsd/) software developed at Gupta Lab. This compiler was developed as a part of my B.Tech project at [DA-IICT](http://www.daiict.ac.in). The supplimentary file for compiler are available at this URL : `http://tiny.cc/chemical-compiler`.

**The compiler supports following operations :-** 

* Addition
* Subtraction (non-negative)
* Multiplication
* Identifier declaration 

**The compiler also supports following operation but is not able to generate code:-**

* Division
* Conditional operators
* 'if' branching statement
* 'while' looping statement

**The precedence and associativity of operator supported:-** 

Precedence | Operator | Associativity
------------ | ------------- | -------------
1 | *, / | Left
2 | +, - | Left
3 | >, <, >=, <=, ==, != | Left

**Required Libraries and Tools**

* uthash - Required for Hash Map.
* flex - Required to tokenize source code i.e lexical analyzer part.
* bison - Required to parse code as per Context Free Grammar i.e parser.
* gdsl - Required to use data structure like stack, queue, etc.

**Steps to build the source code**

    > bison -y -d parser.y
        
    > flex lexer.l
        
    > gcc -c y.tab.c lex.yy.c
        
    > gcc y.tab.o lex.yy.o symbolTable.c genReaction.c extraModules.c treeParser.c -o ccx -I/usr/local/include/gdsl -L/usr/local/lib -lgdsl

__For linking GDSL Lib__

    export LD_LIBRARY_PATH=/usr/local/include/gdsl:$LD_LIBRARY_PATH

`Note: Confirm paths of GDSL flags after installing it in your UNIX system.`

**Error Handling**

The compiler supports following kinds of error reporting:

* Out of memory
* Undeclared variable used
* Incorrect Syntax
