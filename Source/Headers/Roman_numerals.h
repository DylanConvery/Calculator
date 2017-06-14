#ifndef ROMAN_NUMERAL_H
#define ROMAN_NUMERAL_H

#include <iostream>
#include <fstream>
#include <string>

namespace roman {
class Roman_int {
public:
	Roman_int() : val(0), numerals("NULLA") {}
	Roman_int(std::string roman_numeral);
	Roman_int(int num);

	int value() const { return val; }
	std::string numeral() const { return numerals; }
private:
	int get_val(char c);

	int val;
	std::string numerals;
};

bool is_legal(int cnt, char c);
std::string to_roman_numeral(int n);

std::ostream &operator << (std::ostream &os, const Roman_int &ri);
std::istream &operator >> (std::istream &is, Roman_int &ri);
}
#endif // !ROMAN_NUMERAL_H