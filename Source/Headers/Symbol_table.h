#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <stdexcept>
#include "Variable.h"

class Symbol_table {
public:
	double get(const std::string &var);
	void set(const std::string &var, const double &d);
	bool is_declared(const std::string &var);
	double declare(const std::string &var, const double &val, const bool &type);

private:
	std::vector<Variable>var_table;
};

#endif // !SYMBOL_TABLE_H
