#include "Token_stream.h"

/// read characters from cin and compose a token
Token Token_stream::get() {
	if (full) { // do we already have a Token ready for use?
		full = false;
		return buffer;
	}
	char input;
	if (my_input.get(input)) {
		while (isspace(input)) {
			if (input == '\n') {
				return Token(print);
			}
			my_input.get(input);
		}
		switch (input) {
		case assignment:
		case '(':
		case ')':
		case '{':
		case '}':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case ',':
			return Token{ input }; // let character represent itself
		case '.':
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			my_input.putback(input); // put value back into input stream
			double val;
			my_input >> val; // read as floating point number instead
							 //narrow_cast<int>(val);
			return Token{ number, val };
		}
		case 'I': case 'V': case 'X': case 'L': case 'C': case 'D': case 'M':
		{
			roman::Roman_int temp;
			my_input.unget();
			my_input >> temp;
			return Token{ roman_numeral, temp };
		}
		default:
			if (isalpha(input)) {
				std::string s;
				s += input;
				while (my_input.get(input) && (isalnum(input) || input == '_' || input == '.')) {
					s += input;
				}
				my_input.putback(input);
				if (s == sqrtkey) {
					return Token(square);
				}
				else if (s == powkey) {
					return Token(power);
				}
				else if (s == exitkey) {
					return Token(exiting);
				}
				else if (s == helpkey) {
					return Token(help);
				}
				else if (s == varkey) {
					return Token(var);
				}
				else if (s == constkey) {
					return Token(constant);
				}
				else if (s == fromkey) {
					return Token(from);
				}
				else if (s == tokey) {
					return Token(to);
				}
				return Token{ name, s };
			}
			error("Bad input...");
		}
	}
	return Token{ exiting };
}

/// wipe and fill buffer
void Token_stream::putback(const Token &t) {
	if (full) {
		error("buffer is full");
	}
	buffer = t;
	full = true; // buffer is now full
}

void Token_stream::ignore(const char &c) {
	// c represents the kind of the Token
	// check buffer
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;
	// search input
	char input;
	while (input.get(input)) {
		if (input == c || input == '\n') {
			return;
		}
	}
}
