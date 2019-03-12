#ifndef __DFA_H__
#define __DFA_H__

#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string.h>

using namespace std;

enum History{
	DIGIT_BEFORE,
	EQUAL_BEFORE,
	SLASH_BEFORE,
	QUOTE_BEFORE,
	ARITHMETIC_BEFORE,
	RELATIONAL_BEFORE,
	DOUBLE_ARITHMETIC_BEFORE,
	FIRST_STAR_BEFORE,
	WIN_ENDLINE_BEFORE,
	SINGLE_QUOTE_BEFORE,
	NON_LAST_SLASH_BEFORE,
	LAST_SLASH_BEFORE,
	SECOND_STAR_BEFORE,
	SECOND_SINGLE_QUOTE_BEFORE,
	SECOND_DOUBLE_QUOTE_BEFORE,
	SECOND_SLASH,
	IO_BEFORE,
	OP_BEFORE,
	BRACKET_BEFORE,
	FIRST_VAR_CHAR,
	NON_ENDLINE,
	ENDLINE_BEFORE,
	DOT_BEFORE,
	SEPARATOR_BEFORE,
	VAR_CHAR, //variable character
	NON_VAR_CHAR,
	NON_OPERATOR,
	NON_DIGIT,
	NON_QUOTE,
	NON_STAR,
	NONE
};

enum State{
	INTEGER,
	FLOAT,
	COMMENT,
	OPERATOR,
	GROUP_OPERATOR,
	KEYWORD,
	CHAR,
	STRING,
	VARIABLE,
	QUOTE_FIRST,
	FIRST_SLASH,
	ASSIGNMENT,
	IO,
	BRACKET,
	SECOND_START,
	DOUBLE_ARITHMETIC,
	DOUBLE_RELATIONAL,
	ARITHMETIC_OPERATOR,
	RELATIONAL_OPERATOR,
	IDENTIFIER,
	SEPARATOR,
	NEWLINE,
	START,
	END,
	STATE_COUNT
};

struct hash_pair { 
    unsigned long operator()(const pair<State, History>& p) const{ 
        auto hash1 = hash<State>{}(p.first); 
        auto hash2 = hash<History>{}(p.second); 
        return hash1 ^ hash2; 
    } 
};

class DFA{

    private:
        unordered_map<pair<State, History>, vector<pair<State, History>>, hash_pair> transitions; 
        vector<string> keywords;

    public:
	DFA();

	void run(char, char, vector<pair<State, History>>&, State, History);
	void initialize();
	string stateMapper(State, string);

    private:
        void initializeStart();
        void initializeFloat();
        void initializeComment();
        void initializeOperator();
	void initializeString();
	void initializeIdentifier();
        void initializeInteger();
	void initializeChar();
	void initializeSeparator();
	void initializeBracket();
	void initializeIO();

	bool isValidTransition(History, char, char);
	bool contains(string, char);
}; 

#endif
