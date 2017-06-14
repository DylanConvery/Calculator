#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "std_lib_facilities.h"
#include "Variable.h"

class Symbol_table {
public:
	double get(const std::string &var);
	void set(const std::string &var, const double &d);
	bool is_declared(const std::string &var);
	double declare(const std::string &var, const double &val, const bool &type);

private:
	vector<Variable>var_table;
};

#endif // !SYMBOL_TABLE_H
