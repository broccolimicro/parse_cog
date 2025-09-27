#include "expression.h"
#include <parse_expression/precedence.h>

namespace parse_cog {

using parse_expression::precedence_set;
using parse_expression::operation_set;

void setup_expressions() {
	precedence_set result;
	result.push(operation_set::GROUP);
	result.push_back("[", "", ",", "]");

	result.push(operation_set::TERNARY);
	result.push_back("", "?", ":", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "|", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "&", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "^", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "||", "");
	
	result.push(operation_set::BINARY);
	result.push_back("", "", "&&", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "^^", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "==", "");
	result.push_back("", "", "~=", "");
	result.push_back("", "", "<", "");
	result.push_back("", "", ">", "");
	result.push_back("", "", "<=", "");
	result.push_back("", "", ">=", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "<<", "");
	result.push_back("", "", ">>", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "+", "");
	result.push_back("", "", "-", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "*", "");
	result.push_back("", "", "/", "");
	result.push_back("", "", "%", "");

	result.push(operation_set::UNARY);
	result.push_back("!", "", "", "");
	result.push_back("~", "", "", "");
	result.push_back("+", "", "", "");
	result.push_back("-", "", "", "");
	result.push_back("?", "", "", "");

	result.push(operation_set::MODIFIER);
	result.push_back("", "'", "", "");

	result.push(operation_set::MODIFIER);
	//result.push_back("", "{", ",", "}");
	result.push_back("", "(", ",", ")");
	result.push_back("", ".", "", "");
	result.push_back("", "[", ":", "]");
	
	result.push(operation_set::MODIFIER);
	result.push_back("", "::", "", "");
	
	expression::register_precedence(result);
	assignment::lvalueLevel = 13;
}

}
