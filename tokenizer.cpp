#include "tokenizer.h"

Tokenizer::Tokenizer(std::string filename){

    this->filename = filename;
	pointer = -1;
	lineNumber = 1;
	columnNumber = 0;
}

Tokenizer::~Tokenizer() {
	file.close();
}

void Tokenizer::prepare(){

	//open the input file
	openFile();
	
	//initialize the variables for the first token
	resetTokenizer();

	//build the dfa
	dfa.initialize();
}

vector<pair<int, int>> Tokenizer::tokenize(){
	
	prepare();
	
	vector<char> bytes(
		(istreambuf_iterator<char>(file)),
		(istreambuf_iterator<char>()));

	vector<pair<int, int>> tokenPointerMap;

	int tokensFirstElement = 0;
	bool isEndline = false;
	bool lastEndline = false;
	bool setFirstPointer = true;
	bool wasEndState = false;

	vector<char>::iterator bytesIt = bytes.begin();
	while(bytesIt != bytes.end()) {
		
		/*
		*check if the actual character was already counted
		*if not increment the global pointer
		*/
		if (!wasEndState) {
			++pointer;
			++columnNumber;
		}
		else {
			wasEndState = false;
		}

		/*
		*check if we are starting a new token
		*if yes store the first's element position and unset the corresponding flag
		*/
		if (setFirstPointer) {
			tokensFirstElement = pointer;
			setFirstPointer = false;
		}

		vector<pair<State, History >>::iterator transitionIt = actualTransition.begin();
		for (; transitionIt != actualTransition.end(); ++transitionIt) {
			dfa.run(*bytesIt, (!token.empty()) ? token.back() : ' ', nextTransition, (*transitionIt).first, (*transitionIt).second);
		}

		if(*bytesIt == '\n' && !lastEndline){
			isEndline = true;
			lastEndline = true;
			columnNumber = 1;
		}

		int actualTransitionSize = nextTransition.size();
		if (actualTransitionSize == 0) {
			cout << "Actual state has no valid transition!Moving forward until we get a separator\n";
			while (*bytesIt != ' ' && *bytesIt != '\n' && *bytesIt != '\t' || *bytesIt != '\r') {
				++bytesIt;
			}
		}
		else if (actualTransitionSize == 1 && nextTransition[0].first == State::END) {
			if (mustShowToken(actualTransition[0].first)) {
				showToken(actualTransition[0], token);
				tokenPointerMap.push_back(make_pair(tokensFirstElement, (int)(token.size())));
			}
			wasEndState = true;
			setFirstPointer = true;
			resetTokenizer();
		}
		else{
			token += *bytesIt;
			actualTransition = nextTransition;
			nextTransition.clear();

			if (*bytesIt != '\n') {
				lastEndline = false;
			}

			++bytesIt;
		}

		if (isEndline) {
			++lineNumber;
			isEndline = false;
		}
	}

	/*
	*check for an unsolved state
	*if we are in an end state or a separator one(such as endline, space, tab etc) we can "close" the state 
	*else we signalize the error
	*/
	int actualTransitionSize = actualTransition.size();
	if (actualTransitionSize == 1 && actualTransition[0].first == State::END || actualTransition[0].first == State::SEPARATOR) {
		if (mustShowToken(actualTransition[0].first)) {
			showToken(actualTransition[0], token);
		}
		resetTokenizer();
	}
	else {
		cout << "Actual state has no valid transition!Moving forward until we get a separator\n";
	}

	return tokenPointerMap;
}

void Tokenizer::resetTokenizer() {

	token.clear();

	nextTransition.clear();
	actualTransition.clear();
	actualTransition.push_back(make_pair(State::START, History::NONE));
}

void Tokenizer::openFile() {

	file.open(filename, ios::binary);
	if (!file) {
		cout << "Can't open " << filename << "!\n";
		throw -1;
	}
}

bool Tokenizer::mustShowToken(State state) {
	return !(state == State::SEPARATOR) && !(state == State::COMMENT);
}

string Tokenizer::validate(string s) {
	s.erase(remove(s.begin(), s.end(), '\r'), s.end());
	return s;
}

// token - type - length - global position - position(line, column)
void Tokenizer::showToken(pair<State, History> infos, string token) {
	cout << validate(token) << " - " << dfa.stateMapper(infos.first, token) << " - " << token.size() << " - " << pointer << "(" << lineNumber << "," << (columnNumber - token.size()) << ")\n";
}

