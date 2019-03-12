#include "dfa.h"

DFA::DFA(){
    keywords = {"include", "if", "while", "void", "int", "float", "for", "using", "main", "char", "return"};
}

void DFA::run(char in, char in1, vector<pair<State, History>>& nextTransition, State tokenState, History tokenHistory) {

	auto tokenPair = make_pair(tokenState, tokenHistory);
	int transitionSize = transitions[tokenPair].size();

	for (int i = 0; i < transitionSize; ++i) {
		if (isValidTransition(transitions[tokenPair][i].second, in, in1)) {
			nextTransition.push_back(transitions[tokenPair][i]);
		}
	}
}

void DFA::initialize() {

	initializeStart();
	initializeFloat();
	initializeComment();
	initializeOperator();
	initializeIdentifier();
	initializeInteger();
	initializeChar();
	initializeSeparator();
	initializeBracket();
	initializeString();
	initializeIO();
}

void DFA::initializeStart(){

    transitions[make_pair(State::START, History::NONE)].insert(
        transitions[make_pair(State::START, History::NONE)].begin(),
        {
        make_pair(State::INTEGER,             History::DIGIT_BEFORE),
        make_pair(State::COMMENT,             History::SLASH_BEFORE),
        make_pair(State::OPERATOR,            History::OP_BEFORE),
        make_pair(State::QUOTE_FIRST,         History::QUOTE_BEFORE),
		make_pair(State::IDENTIFIER,          History::FIRST_VAR_CHAR),
		make_pair(State::CHAR,                History::SINGLE_QUOTE_BEFORE),
		make_pair(State::SEPARATOR,           History::SEPARATOR_BEFORE),
		make_pair(State::BRACKET,             History::BRACKET_BEFORE),
		make_pair(State::IO,                  History::IO_BEFORE)
        }
    );
}

/*
*Parent State  : Integer, Digit_Before
*End Condition : Finds a separator
*Examples      : 12. , 12.35, 12.00
*/
void DFA::initializeFloat(){

    transitions[make_pair(State::FLOAT, History::DOT_BEFORE)].insert(
        transitions[make_pair(State::FLOAT, History::DOT_BEFORE)].end(),
        {
            make_pair(State::FLOAT,   History::DIGIT_BEFORE),
            make_pair(State::END,     History::SEPARATOR_BEFORE)
        }
    );

	transitions[make_pair(State::FLOAT, History::DIGIT_BEFORE)].insert(
		transitions[make_pair(State::FLOAT, History::DIGIT_BEFORE)].end(),
		{
			make_pair(State::FLOAT,   History::DIGIT_BEFORE),
			make_pair(State::END,     History::SEPARATOR_BEFORE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : Finds a separator
*Examples      : 0, 12, 1215
*/
void DFA::initializeInteger() {

	transitions[make_pair(State::INTEGER, History::DIGIT_BEFORE)].insert(
		transitions[make_pair(State::INTEGER, History::DIGIT_BEFORE)].end(),
		{
			make_pair(State::INTEGER, History::DIGIT_BEFORE),
			make_pair(State::FLOAT,   History::DOT_BEFORE),
			make_pair(State::END,     History::NON_DIGIT)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : After one iteration - excepting Windows endline(\r\n)
*Examples      : \t, \r\t ;
*/
void DFA::initializeSeparator() {

	transitions[make_pair(State::SEPARATOR, History::SEPARATOR_BEFORE)].insert(
		transitions[make_pair(State::SEPARATOR, History::SEPARATOR_BEFORE)].end(),
		{
			make_pair(State::SEPARATOR, History::WIN_ENDLINE_BEFORE),
			make_pair(State::END,       History::NONE)
		}
	);

	transitions[make_pair(State::SEPARATOR, History::WIN_ENDLINE_BEFORE)].insert(
		transitions[make_pair(State::SEPARATOR, History::WIN_ENDLINE_BEFORE)].end(),
		{
			make_pair(State::END, History::ENDLINE_BEFORE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : After one iteration
*Examples      : {}()[]
*/
void DFA::initializeBracket() {

	transitions[make_pair(State::BRACKET, History::BRACKET_BEFORE)].insert(
		transitions[make_pair(State::BRACKET, History::BRACKET_BEFORE)].end(),
		{
			make_pair(State::END,  History::NONE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : After two iterations
*Examples      : << , >>
*/
void DFA::initializeIO() {

	transitions[make_pair(State::IO, History::IO_BEFORE)].insert(
		transitions[make_pair(State::IO, History::IO_BEFORE)].end(),
		{
			make_pair(State::END,  History::NONE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : Reaching an invalid character which can't be contained by an identifier
*Examples      : abc, x, y, isOpened, _aqz
*/
void DFA::initializeIdentifier() {

	transitions[make_pair(State::IDENTIFIER, History::FIRST_VAR_CHAR)].insert(
		transitions[make_pair(State::IDENTIFIER, History::FIRST_VAR_CHAR)].end(),
		{
			make_pair(State::IDENTIFIER, History::VAR_CHAR),
			make_pair(State::END, History::NON_VAR_CHAR)
		}
	);

	transitions[make_pair(State::IDENTIFIER, History::VAR_CHAR)].insert(
		transitions[make_pair(State::IDENTIFIER, History::VAR_CHAR)].end(),
		{
			make_pair(State::IDENTIFIER, History::VAR_CHAR),
			make_pair(State::END, History::NON_VAR_CHAR)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : Reaching the second '
*Examples      : 'm', 'x', 'y', '"'
*/
void DFA::initializeChar() {

	//first state
	transitions[make_pair(State::CHAR, History::SINGLE_QUOTE_BEFORE)].insert(
		transitions[make_pair(State::CHAR, History::SINGLE_QUOTE_BEFORE)].end(),
		{
			make_pair(State::CHAR, History::NONE),
			make_pair(State::END,  History::SINGLE_QUOTE_BEFORE)
		}
	);

	//loop state
	transitions[make_pair(State::CHAR, History::NONE)].insert(
		transitions[make_pair(State::CHAR, History::NONE)].end(),
		{
			make_pair(State::CHAR,  History::SECOND_SINGLE_QUOTE_BEFORE)
		}
	);

	//last state
	transitions[make_pair(State::CHAR, History::SECOND_SINGLE_QUOTE_BEFORE)].insert(
		transitions[make_pair(State::CHAR, History::SECOND_SINGLE_QUOTE_BEFORE)].end(),
		{
			make_pair(State::END,  History::NONE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : Reaching the second "
*Examples      : "abc", "example", "x"
*/
void DFA::initializeString() {

	//first state
	transitions[make_pair(State::QUOTE_FIRST, History::QUOTE_BEFORE)].insert(
		transitions[make_pair(State::QUOTE_FIRST, History::QUOTE_BEFORE)].end(),
		{
			make_pair(State::STRING, History::NON_QUOTE),
			make_pair(State::END,    History::QUOTE_BEFORE)
		}
	);

	//second state
	transitions[make_pair(State::STRING, History::QUOTE_BEFORE)].insert(
		transitions[make_pair(State::STRING, History::QUOTE_BEFORE)].end(),
		{
			make_pair(State::END,    History::QUOTE_BEFORE)
		}
	);

	//loop state
	transitions[make_pair(State::STRING, History::NON_QUOTE)].insert(
		transitions[make_pair(State::STRING, History::NON_QUOTE)].end(),
		{
			make_pair(State::STRING, History::NON_QUOTE),
			make_pair(State::STRING, History::SECOND_DOUBLE_QUOTE_BEFORE)
		}
	);

	//last state
	transitions[make_pair(State::STRING, History::SECOND_DOUBLE_QUOTE_BEFORE)].insert(
		transitions[make_pair(State::STRING, History::SECOND_DOUBLE_QUOTE_BEFORE)].end(),
		{
			make_pair(State::END, History::NONE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : Depends on the operator type:
	- first slash of a comment - see initializeComment function(//a, //comment)
	- first character of an IO operator - see initializeIO function(<<, >>)
	- double operators : others end after two iterations(-=, /=, ==)
	- single operators : end after first iteration (+, -, *)
*Examples : see above
*/
void DFA::initializeOperator() {

	transitions[make_pair(State::OPERATOR, History::OP_BEFORE)].insert(
		transitions[make_pair(State::OPERATOR, History::OP_BEFORE)].end(),
		{
			make_pair(State::COMMENT,           History::SLASH_BEFORE),
			make_pair(State::DOUBLE_ARITHMETIC, History::DOUBLE_ARITHMETIC_BEFORE),
			make_pair(State::DOUBLE_RELATIONAL, History::EQUAL_BEFORE),
			make_pair(State::IO,                History::IO_BEFORE),
			make_pair(State::END,	            History::NON_OPERATOR)
		}
	);

	transitions[make_pair(State::DOUBLE_ARITHMETIC, History::DOUBLE_ARITHMETIC_BEFORE)].insert(
		transitions[make_pair(State::DOUBLE_ARITHMETIC, History::DOUBLE_ARITHMETIC_BEFORE)].end(),
		{
			make_pair(State::END, History::NONE)
		}
	);

	transitions[make_pair(State::DOUBLE_RELATIONAL, History::EQUAL_BEFORE)].insert(
		transitions[make_pair(State::DOUBLE_RELATIONAL, History::EQUAL_BEFORE)].end(),
		{
			make_pair(State::END, History::NONE)
		}
	);
}

/*
*Parent State  : Start, None
*End Condition : Depends on the operator type:
- first slash of a comment - see initializeComment function(//a, //comment)
- first character of an IO operator - see initializeIO function(<<, >>)
- double operators : others end after two iterations(-=, /=, ==)
- single operators : end after first iteration (+, -, *)
*Examples : //single line comment, this comment
*/
void DFA::initializeComment(){

	//first common state
	transitions[make_pair(State::COMMENT, History::SLASH_BEFORE)].insert(
		transitions[make_pair(State::COMMENT, History::SLASH_BEFORE)].end(),
		{
			make_pair(State::COMMENT, History::SECOND_SLASH),
			make_pair(State::COMMENT, History::FIRST_STAR_BEFORE),
			make_pair(State::END,     History::SEPARATOR_BEFORE)
		}
	);

	//second state for a single line comment
	transitions[make_pair(State::COMMENT, History::SECOND_SLASH)].insert(
		transitions[make_pair(State::COMMENT, History::SECOND_SLASH)].end(),
		{
			make_pair(State::COMMENT, History::NON_ENDLINE),
			make_pair(State::END,     History::ENDLINE_BEFORE)
		}
	);

	//loop state for single line comment
	transitions[make_pair(State::COMMENT, History::NON_ENDLINE)].insert(
		transitions[make_pair(State::COMMENT, History::NON_ENDLINE)].end(),
		{
			make_pair(State::COMMENT, History::NON_ENDLINE),
			make_pair(State::END,     History::ENDLINE_BEFORE)
		}
	);

	//second state for multi line comment
	transitions[make_pair(State::COMMENT, History::FIRST_STAR_BEFORE)].insert(
		transitions[make_pair(State::COMMENT, History::FIRST_STAR_BEFORE)].end(),
		{
			make_pair(State::COMMENT, History::SECOND_STAR_BEFORE),
			make_pair(State::COMMENT, History::NON_STAR)
		}
	);

	//loop state for multi line comment
	transitions[make_pair(State::COMMENT, History::NON_STAR)].insert(
		transitions[make_pair(State::COMMENT, History::NON_STAR)].end(),
		{
			make_pair(State::COMMENT, History::SECOND_STAR_BEFORE),
			make_pair(State::COMMENT, History::NON_STAR)
		}
	);

	//state which announces the end state
	transitions[make_pair(State::COMMENT, History::SECOND_STAR_BEFORE)].insert(
		transitions[make_pair(State::COMMENT, History::SECOND_STAR_BEFORE)].end(),
		{
			make_pair(State::COMMENT, History::LAST_SLASH_BEFORE),
			make_pair(State::COMMENT, History::NON_LAST_SLASH_BEFORE)
		}
	);

	//last state of a multi line comment
	transitions[make_pair(State::COMMENT, History::LAST_SLASH_BEFORE)].insert(
		transitions[make_pair(State::COMMENT, History::LAST_SLASH_BEFORE)].end(),
		{
			make_pair(State::END, History::NONE),
		}
	);

	//if a pair Comment, Second_star_before doesn't precede an end state we are jumping back to the loop state
	transitions[make_pair(State::COMMENT, History::NON_LAST_SLASH_BEFORE)].insert(
		transitions[make_pair(State::COMMENT, History::NON_LAST_SLASH_BEFORE)].end(),
		{
			make_pair(State::COMMENT, History::SECOND_STAR_BEFORE),
			make_pair(State::COMMENT, History::NON_STAR)
		}
	);
}

bool DFA::isValidTransition(History tokenHistory, char in, char in1) {
	
	switch (tokenHistory) {
		case DIGIT_BEFORE:
			return (iswdigit(in));
		case EQUAL_BEFORE:
			return in == '=';
		case SLASH_BEFORE:
			return in == '/';
		case QUOTE_BEFORE:
			return in == '"';
		case DOT_BEFORE:
			return in == '.';
		case BRACKET_BEFORE:
			return contains("{}()[]", in);
		case IO_BEFORE:
			return in == in1 && (in1 == '<' || in1 == '>');
		case ARITHMETIC_BEFORE:
			return contains("+-/*%", in);
		case RELATIONAL_BEFORE:
			return contains("<>!=", in);
		case SECOND_SLASH:
			return in == '/';
		case SINGLE_QUOTE_BEFORE:
			return in == '\'';
		case WIN_ENDLINE_BEFORE:
			return in == '\r';
		case SECOND_SINGLE_QUOTE_BEFORE:
			return in == '\'';
		case SECOND_DOUBLE_QUOTE_BEFORE:
			return in == '"';
		case SECOND_STAR_BEFORE:
			return in == '*';
		case FIRST_STAR_BEFORE:
			return in == '*';
		case ENDLINE_BEFORE:
			return in == '\n';
		case LAST_SLASH_BEFORE:
			return in == '/';
		case DOUBLE_ARITHMETIC_BEFORE:
			return in == '+' || in == '-';
		case OP_BEFORE:
			return contains("*+-/=><!%", in);
		case FIRST_VAR_CHAR:
			return iswalpha(in) || in == '_';
		case VAR_CHAR:
			return iswalpha(in) || iswdigit(in);
		case NON_VAR_CHAR:
			return !(iswalpha(in) || iswdigit(in));
		case NON_OPERATOR:
			return !contains("*+-[]{}:=.,><!;", in);
		case NON_QUOTE:
			return in != '"';
		case NON_ENDLINE:
			return in != '\n';
		case NON_LAST_SLASH_BEFORE:
			return in != '/';
		case NON_DIGIT:
			return !iswdigit(in);
		case NON_STAR:
			return in != '*';
		case SEPARATOR_BEFORE:
			return in == ' ' || in == '\n' || in == '\t' || in == '\r' || in == ';';
		case NONE:
			return true;
		default:
			cout << "Token has wrong infos! - History";
			return false;
	}
}

string DFA::stateMapper(State state, string token) {

	switch (state) {
		case START:
			return "START";
		case END:
			return "END";
		case INTEGER:
			return "LITERAL(INTEGER)";
		case FLOAT:
			return "LITERAL(FLOAT)";
		case OPERATOR:
			return "OPERATOR";
		case STRING:
			return "STRING";
		case COMMENT:
			return "COMMENT";
		case CHAR:
			return "CHAR";
		case BRACKET:
			return "BRACKET";
		case ASSIGNMENT:
			return "ASSIGNMENT OPERATOR";
		case IO:
			return "IO OPERATOR";
		case ARITHMETIC_OPERATOR:
			return "ARITHMETIC OPERATOR";
		case DOUBLE_ARITHMETIC:
			return "DOUBLE ARITHMETIC OPERATOR";
		case DOUBLE_RELATIONAL:
			return "DOUBLE RELATIONAL OPERATOR";
		case IDENTIFIER:
			if (find(keywords.begin(), keywords.end(), token) != keywords.end()) {
				return "KEYWORD";
			}
			return "IDENTIFIER";
		default:
			return "UNDEFINED";
	}
}

bool DFA::contains(string target, char in) {

	int targetSize = target.size();
	for (int i = 0; i < targetSize; ++i) {
		if (target[i] == in) {
			return true;
		}
	}

	return false;
}