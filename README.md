# Lexical analyser for C++

Simple lexical analyser for C++ which uses a DFA to extract the tokens. The DFA can be easily modified so you can detect more tokens.
The function which does the tokenization also returns a vector of pairs (positions in file, token's length) so you can create a pointer 
to that token directly in the file. 

# Compiler

I used the Visual Studio 2017's compiler. C++ 11 should be enought. Keep in mind that you must disable _CRT_SECURE_NO_WARNINGS in order to
use fseek.
