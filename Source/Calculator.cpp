#include "Token_stream.h"
#include "Symbol_table.h"
#include <cmath>
#include <cstring>
#include <fstream>

token_stream::Token_stream ts;
Symbol_table st;

static const std::string manualPath = "\manual";

template<class R, class A> R narrow_cast(const A& a)
{
	R r = R(a);
	if (A(r)!=a) throw std::runtime_error(std::string("info loss"));
	return r;
}

double expression(token_stream::Token_stream &ts, Symbol_table &st);
double declaration(bool type, token_stream::Token_stream &ts, Symbol_table &st) {
	Token t;
	if (type) {
		t = ts.get();
		if (t.kind != token_stream::var) {
			throw std::runtime_error("declaration expected");
		}
	}
	t = ts.get();
	if (t.kind != token_stream::name) {
		throw std::runtime_error("name expected in declaration");
	}
	std::string var_name = t.name;
	t = ts.get();
	if (t.kind != token_stream::assignment) {
		throw std::runtime_error("'=' sign missing in delcaration of " + var_name);
	}
	double d = expression(ts, st);
	t = ts.get();
	if (t.kind != token_stream::print) {
		throw std::runtime_error("Bad input...");
	}
	ts.putback(t);
	st.declare(var_name, d, type);
	return d;
}

void to_y();
void from_x(std::ostream &);
void statement(std::ostream &os, token_stream::Token_stream &ts, Symbol_table &st) {
	Token t = ts.get();
	switch (t.kind) {
	case token_stream::var:
		os << token_stream::result << declaration(false, ts, st) << "\n";
		break;
	case token_stream::constant:
		os << token_stream::result << declaration(true, ts, st) << "\n";
		break;
	case token_stream::roman_numeral:
		ts.putback(t);
		os << token_stream::result << roman::Roman_int(static_cast<int>(expression(ts, st))) << "\n";
		break;
	case token_stream::from:
		from_x(os);
		break;
	case token_stream::to:
		to_y();
		break;
	default:
		ts.putback(t);
		os << token_stream::result << expression(ts, st) << "\n";
		break;
	}
}

double primary(token_stream::Token_stream &ts, Symbol_table &st) { // deals with numbers and parentheses
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ')') {
			throw std::runtime_error("')' expected");
		}
		return d;
	}
	case '{':
	{
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != '}') {
			throw std::runtime_error("'}' expected");
		}
		return d;
	}
	case '-':
	{
		return -primary(ts, st);
	}
	case '+':
	{
		return primary(ts, st);
	}
	case token_stream::number:
	{
		return t.value;
	}
	case token_stream::name:
	{
		std::string var_name = t.name;
		if (st.is_declared(var_name)) {
			t = ts.get();
			if (t.kind == token_stream::assignment) {
				double d = expression(ts, st);
				t = ts.get();
				if (t.kind != token_stream::print) {
					throw std::runtime_error("Bad input...");
				}
				ts.putback(t);
				st.set(var_name, d);
				return d;
			}
			else {
				ts.putback(t);
			}
		}
		return st.get(var_name);
	}
	case token_stream::square:
	{
		t = ts.get();
		if (t.kind != '(') {
			throw std::runtime_error("'(' expected");
		}
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ')') {
			throw std::runtime_error("')' expected");
		}
		if (d <= 0) {
			throw std::runtime_error("Cannot get square root of negative number");
		}
		d = sqrt(d);
		return d;
	}
	case token_stream::power:
	{
		t = ts.get();
		if (t.kind != '(') {
			throw std::runtime_error("'(' expected");
		}
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ',') {
			throw std::runtime_error("',' expected");
		}
		int times = narrow_cast<int>(expression(ts, st));
		t = ts.get();
		if (t.kind != ')') {
			throw std::runtime_error("')' expected");
		}
		d = pow(d, times);
		return d;
	}
	case token_stream::roman_numeral:
	{
		return t.roman.value();
	}
	default:
		std::cout << t.kind << "\n";
		throw std::runtime_error("primary expected");
	}
}

double term(token_stream::Token_stream &ts, Symbol_table &st) { // deals with multiplication and division
	double left = primary(ts, st);
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary(ts, st);
			t = ts.get();
			break;
		case '/':
		{
			double d = primary(ts, st);
			if (d == 0) {
				throw std::runtime_error("cannot divide or mod by zero");
				left = 0;
			}
			else {
				left /= d;
			}
			t = ts.get();
			break;
		}
		case '%':
		{
			/*
			code for floating point modulus
			double d = primary();
			if (d == 0) {
			error("cannot divide or mod by zero");
			left = 0;
			}
			else{
			left = fmod(left, d);
			}*/
			int i1 = narrow_cast<int>(left);
			int i2 = narrow_cast<int>(primary(ts, st));
			if (i2 == 0) {
				throw std::runtime_error("cannot divide or mod by zero");
				left = 0;
			}
			else {
				left = i1%i2;
			}

			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression(token_stream::Token_stream &ts, Symbol_table &st) { // deals with addition and subtraction
	double left = term(ts, st);
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '+':
			left += term(ts, st);
			t = ts.get();
			break;
		case '-':
			left -= term(ts, st);
			t = ts.get();
			break;
		default:
			if (t.kind != token_stream::print && t.kind != ')' && t.kind != '}' && t.kind != ',') {
				throw std::runtime_error("syntax error"); //checks that you don't return an invalid statement
			}
			ts.putback(t);
			return left;
		}
	}
}

void clean_up_mess(token_stream::Token_stream &ts) {
	ts.ignore(token_stream::print);
}

void manual(std::ostream &os, token_stream::Token_stream &ts) {
	std::ifstream ifs{manualPath};
	if (!ifs)throw std::runtime_error("can't find manual");
	std::vector<std::string>manual;
	for(std::string s; getline(ifs,s);){
		manual.push_back(s);
	}
	
	for (const std::string &i : manual) {
		os << token_stream::result << "~ " << i << "\n";
	}
	os << token_stream::prompt;
	ts.ignore('\n');
}

void calculate(std::ostream &os, token_stream::Token_stream &ts, Symbol_table &st);
void from_x(std::ostream &os) {
	Token t = ts.get();
	if (t.kind != token_stream::name) {
		throw std::runtime_error("expected file name");
	}

	std::ifstream ifs{ t.name };
	if (!ifs)throw std::runtime_error("can't open input stream \"" + t.name + "\"");
	token_stream::Token_stream ts2{ ifs };
	calculate(os, ts2, st);
}

void to_y() {
	Token t = ts.get();
	if (t.kind != token_stream::name) {
		throw std::runtime_error("expected file name");
	}

	std::ofstream ofs{ t.name };
	if (!ofs)throw std::runtime_error("can't open output stream \"" + t.name + "\"");
	calculate(ofs, ts, st);
}


/// calculation loop 
void calculate(std::ostream &os, token_stream::Token_stream &ts, Symbol_table &st) {
	while (os) {
		try {
			if (&ts == &::ts) {
				std::cout << token_stream::prompt;
			}
			Token t = ts.get();
			while (t.kind == token_stream::print) {
				t = ts.get();
			}
			if (t.kind == token_stream::help) {
				manual(os, ts);
				t = ts.get();
			}
			if (t.kind == token_stream::exiting) { // quit condition
				return;
			}
			ts.putback(t);
			statement(os, ts, st);// start calculation
		}
		catch (std::exception &e) {
			os << token_stream::result << e.what() << '\n';
			clean_up_mess(ts);
		}
	}
}

void wait() {
	char ch(0);
	while (ch != '\n') {
		std::cout << "Press Enter to continue...";
		std::cin.clear();
		std::cin.ignore(120, '\n');
		std::cin.get(ch);
	}
}

int main(int argc, char **argv) {
	try {
		if (argc > 1) {
			if (!strcmp(argv[1], "help")) {
				ts.putback(Token(token_stream::help));
			}
		}

		st.declare("pi", 3.141592653, true);
		st.declare("e", 2.7182818284, true);
		st.declare("k", 1000, true);

		calculate(std::cout, ts, st);
		return 0;
	}
	catch (std::runtime_error &e) {
		std::cerr << e.what() << '\n';
		wait();
		return 1;
	}
	catch (...) {
		std::cerr << "unknown exception thrown\n";
		wait();
		return 2;
	}
}