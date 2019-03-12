#include "tokenizer.h"
#include <stdio.h>
#include <utility>

void findToken(int offset, int length, string filename) {

	FILE* file = fopen(filename.c_str(), "r");
	fseek(file, offset, SEEK_SET);

	while (length--) {
		unsigned char chr = fgetc(file);
		cout << chr;
	}
}

int main(){

	string filename = "in.txt";

    Tokenizer tokenizer(filename);
	vector<pair<int, int>> mapping = tokenizer.tokenize();
	
	cout << "\nTokenizer has finished!\n\n";

	int tokenIndex = 7;
	int offset = mapping[tokenIndex].first;
	int length = mapping[tokenIndex].second;
	cout << "Searching for the " << tokenIndex << "th token on position " << offset << " having length " << length << "...\n";
	findToken(offset, length, filename);

	int x;
	cin >> x;

    return 0;
}