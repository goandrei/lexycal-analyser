#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <fstream>
#include <sstream>
#include <cstdio>
#include "dfa.h"

using namespace std;

class Tokenizer{

    private:
		ifstream file;
        string filename;
		string token;

        int lineNumber, columnNumber;
        int pointer;
		
		vector<pair<State, History >> actualTransition, nextTransition;
        DFA dfa;

    public:
        Tokenizer(string);
		~Tokenizer();
		vector<pair<int, int>> tokenize();

		void resetTokenizer();
		void openFile();
        void prepare();

	private:
		void showToken(pair<State, History>, string);
		bool mustShowToken(State);
		string validate(string);
};

#endif