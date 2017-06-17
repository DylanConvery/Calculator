#include "Symbol_table.h"

double Symbol_table::get(const std::string &var) {
	for (Variable &v : var_table) {
		if (v.name == var) {
			return v.value;
		}
	}
	throw std::runtime_error("undefined variable " + var);
}

void Symbol_table::set(const std::string &var, const double &d) {
	for (Variable &v : var_table) {
		if (v.name == var) {
			if (v.type) {
				throw std::runtime_error(var + " is a constant value");
			}
			v.value = d;
			return;
		}
	}
	throw std::runtime_error("undefined variable " + var);
}

bool Symbol_table::is_declared(const std::string &var) {
	for (const Variable &v : var_table) {
		if (v.name == var) {
			return true;
		}
	}
	return false;
}

double Symbol_table::declare(const std::string &var, const double &val, const bool &type) {
	if (is_declared(var)) {
		throw std::runtime_error(var + " is already defined");
	}
	var_table.push_back(Variable{ var, val, type });
	return val;
}