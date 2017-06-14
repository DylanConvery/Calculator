#include "Roman_numerals.h"

namespace roman {
std::ostream &operator << (std::ostream &os, const Roman_int &ri) {
	return os << ri.numeral();
}

std::istream &operator >> (std::istream &is, Roman_int &ri) {
	std::string new_numeral;
	char ch;
	is.get(ch);
	if (isalpha(ch)) {
		new_numeral += ch;
		while (is.get(ch) && isalpha(ch)) {
			new_numeral += ch;
		}
	}
	is.unget();
	ri = Roman_int{ new_numeral };
	return is;
}

Roman_int::Roman_int(std::string s) {
	int total = get_val(s[s.size() - 1]);
	int count = 1;
	for (int i = s.size() - 2; i >= 0; --i) {
		s[i] == s[i + 1] ? ++count : count = 1;
		if (is_legal(count, s[i])) {
			throw std::runtime_error("illegal string");
		}

		if (get_val(s[i]) >= get_val(s[i + 1])) {
			total += get_val(s[i]);
		}
		else if (get_val(s[i]) < get_val(s[i + 1])) {
			total -= get_val(s[i]);
		}
		else {
			throw std::runtime_error("illegal string");
		}
	}
	val = total;
	numerals = s;
}

Roman_int::Roman_int(int num) : val(num) {
	const char I = 'I'; //1
	const char V = 'V'; //5
	const char X = 'X'; //10
	const char L = 'L'; //50
	const char C = 'C'; //100
	const char D = 'D'; //500
	const char M = 'M'; //1000

	int temp_num = num;
	std::string temp_numerals;

	while (temp_num >= get_val(M)) {
		temp_numerals += M;
		temp_num -= get_val(M);
	}

	if (temp_num >= 900) {
		temp_numerals += C;
		temp_numerals += M;
		temp_num -= 900;
	}
	else if (temp_num >= get_val(D)) {
		temp_numerals += D;
		temp_num -= get_val(D);
	}
	else if (temp_num >= 400) {
		temp_numerals += C;
		temp_numerals += D;
		temp_num -= 400;
	}

	while (temp_num >= get_val(C)) {
		temp_numerals += C;
		temp_num -= get_val(C);
	}
	if (temp_num >= 90) {
		temp_numerals += X;
		temp_numerals += C;
		temp_num -= 90;
	}
	else if (temp_num >= get_val(L)) {
		temp_numerals += L;
		temp_num -= get_val(L);
	}
	else if (temp_num >= 40) {
		temp_numerals += X;
		temp_numerals += L;
		temp_num -= 40;
	}

	while (temp_num >= get_val(X)) {
		temp_numerals += X;
		temp_num -= get_val(X);
	}
	if (temp_num >= 9) {
		temp_numerals += I;
		temp_numerals += X;
		temp_num -= 9;
	}
	else if (temp_num >= get_val(V)) {
		temp_numerals += V;
		temp_num -= get_val(V);
	}
	else if (temp_num >= 4) {
		temp_numerals += I;
		temp_numerals += V;
		temp_num -= 4;
	}
	while (temp_num >= get_val(I)) {
		temp_numerals += I;
		temp_num -= get_val(I);
	}

	numerals = temp_numerals;
}

bool is_legal(int cnt, char c) {
	switch (c) {
	case 'I': //1
	case 'X': //10
	case 'C': //100
	case 'M': //1000
		return cnt > 3;
	case 'V': //5
	case 'L': //50
	case 'D': //500
		return cnt > 1;
	default:
		throw std::runtime_error("illegal character");
	}
}

int Roman_int::get_val(char c) {
	switch (c) {
	case 'I':
		return 1;
	case 'V':
		return 5;
	case 'X':
		return 10;
	case 'L':
		return 50;
	case 'C':
		return 100;
	case 'D':
		return 500;
	case 'M':
		return 1000;
	default:
		throw std::runtime_error("illegal character");
	}
}

std::string to_roman_numeral(int n) {
	Roman_int temp(n);
	return temp.numeral();
}
}