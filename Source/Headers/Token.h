#ifndef TOKEN_H
#define TOKEN_H

#include "Roman_numerals.h"

class Token {
public:
	char kind;
	double value;
	std::string name;
	roman::Roman_int roman;

	Token() = default;
	Token(char ch) : kind{ ch } {}
	Token(char ch, double val) : kind{ ch }, value{ val } {}
	Token(char ch, std::string n) : kind{ ch }, name{ n } {}
	Token(char ch, roman::Roman_int r) : kind{ ch }, roman{ r } {}
};
#endif // !TOKEN_H