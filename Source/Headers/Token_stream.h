#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H


#include "Token.h"
#include <stdexcept>

namespace token_stream{

const char number = '~';
const char print = ';';
const char assignment = '=';
const std::string prompt = ":> ";
const std::string result = ":< ";

const char name = 'A'; // name token
const char var = 'V'; // declaration token
const char square = 'S';
const char power = 'P';
const char help = 'H';
const char exiting = 'E';
const char constant = 'C';
const char variable = 'V';
const char roman_numeral = 'R';
const char from = 'F';
const char to = 'T';
const char eof = '\0';

const std::string constkey = "const";
const std::string varkey = "var";
const std::string exitkey = "exit";
const std::string helpkey = "help";
const std::string sqrtkey = "sqrt";
const std::string powkey = "pow";
const std::string fromkey = "from";
const std::string tokey = "to";

class Token_stream {
public:
	Token_stream() : my_input(std::cin) {}
	Token_stream(std::istream &is) : my_input(is) {}
	std::istream &my_input;

	Token get();
	void putback(const Token &t);
	void ignore(const char &c);
private:
	Token buffer;
	bool full{ false };
};
}

#endif // !TOKEN_STREAM_H