#include "Token_stream.h"
#include "Symbol_table.h"

Token_stream ts;
Symbol_table st;

double expression(Token_stream &ts, Symbol_table &st);
double declaration(bool type, Token_stream &ts, Symbol_table &st) {
	Token t;
	if (type) {
		t = ts.get();
		if (t.kind != var) {
			error("declaration expected");
		}
	}
	t = ts.get();
	if (t.kind != name) {
		error("name expected in declaration");
	}
	std::string var_name = t.name;
	t = ts.get();
	if (t.kind != assignment) {
		error("'=' sign missing in delcaration of ", var_name);
	}
	double d = expression(ts, st);
	t = ts.get();
	if (t.kind != print) {
		error("Bad input...");
	}
	ts.putback(t);
	st.declare(var_name, d, type);
	return d;
}

void to_y();
void from_x(std::ostream &);
void statement(ostream &os, Token_stream &ts, Symbol_table &st) {
	Token t = ts.get();
	switch (t.kind) {
	case var:
		os << result << declaration(false, ts, st) << "\n";
		break;
	case constant:
		os << result << declaration(true, ts, st) << "\n";
		break;
	case roman_numeral:
		ts.putback(t);
		os << result << roman::Roman_int(static_cast<int>(expression(ts, st))) << "\n";
		break;
	case from:
		from_x(os);
		break;
	case to:
		to_y();
		break;
	default:
		ts.putback(t);
		os << result << expression(ts, st) << "\n";
		break;
	}
}

double primary(Token_stream &ts, Symbol_table &st) { // deals with numbers and parentheses
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ')') {
			error("')' expected");
		}
		return d;
	}
	case '{':
	{
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != '}') {
			error("'}' expected");
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
	case number:
	{
		return t.value;
	}
	case name:
	{
		std::string var_name = t.name;
		if (st.is_declared(var_name)) {
			t = ts.get();
			if (t.kind == assignment) {
				double d = expression(ts, st);
				t = ts.get();
				if (t.kind != print) {
					error("Bad input...");
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
	case square:
	{
		t = ts.get();
		if (t.kind != '(') {
			error("'(' expected");
		}
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ')') {
			error("')' expected");
		}
		if (d <= 0) {
			error("Cannot get square root of negative number");
		}
		d = sqrt(d);
		return d;
	}
	case power:
	{
		t = ts.get();
		if (t.kind != '(') {
			error("'(' expected");
		}
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ',') {
			error("',' expected");
		}
		int times = narrow_cast<int>(expression(ts, st));
		t = ts.get();
		if (t.kind != ')') {
			error("')' expected");
		}
		d = pow(d, times);
		return d;
	}
	case roman_numeral:
	{
		return t.roman.value();
	}
	default:
		std::cout << t.kind << "\n";
		error("primary expected");
	}
}

double term(Token_stream &ts, Symbol_table &st) { // deals with multiplication and division
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
				error("cannot divide or mod by zero");
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
				error("cannot divide or mod by zero");
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

double expression(Token_stream &ts, Symbol_table &st) { // deals with addition and subtraction
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
			if (t.kind != print && t.kind != ')' && t.kind != '}' && t.kind != ',') {
				error("syntax error"); //checks that you don't return an invalid statement
			}
			ts.putback(t);
			return left;
		}
	}
}

void clean_up_mess(Token_stream &ts) {
	ts.ignore(print);
}

void manual(std::ostream &os, Token_stream &ts) {
	std::ifstream ifs{"manual"};
	if (!ifs)error("can't open input stream manual");
	vector<std::string>_manual;
	for(std::string s; getline(ifs,s);){
		_manual.push_back(s);
	}
	
	for (const std::string &i : _manual) {
		os << result << "~ " << i << "\n";
	}
	os << prompt;
	ts.ignore('\n');
}

void calculate(std::ostream &os, Token_stream &ts, Symbol_table &st);
void from_x(std::ostream &os) {
	Token t = ts.get();
	if (t.kind != name) {
		error("expected file name");
	}

	std::ifstream ifs{ t.name };
	if (!ifs)error("can't open input stream", t.name);
	Token_stream ts2{ ifs };
	calculate(os, ts2, st);
}

void to_y() {
	Token t = ts.get();
	if (t.kind != name) {
		error("expected file name");
	}

	std::ofstream ofs{ t.name };
	if (!ofs)error("can't open output stream", t.name);
	calculate(ofs, ts, st);
}


/// calculation loop 
void calculate(std::ostream &os, Token_stream &ts, Symbol_table &st) {
	while (os) {
		try {
			if (&ts == &::ts) {
				std::cout << prompt;
			}
			Token t = ts.get();
			while (t.kind == print) {
				t = ts.get();
			}
			if (t.kind == help) {
				manual(os, ts);
				t = ts.get();
			}
			if (t.kind == exiting) { // quit condition
				return;
			}
			ts.putback(t);
			statement(os, ts, st);// start calculation
		}
		catch (std::exception &e) {
			os << result << e.what() << '\n';
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
				ts.putback(Token(help));
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