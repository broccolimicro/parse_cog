#include "factory.h"

#include "composition.h"
#include <parse_expression/precedence.h>

namespace parse_cog {

parse::syntax *produce(tokenizer &tokens, void *data) {
	return new composition(tokens, 0, data);
}

void expect(tokenizer &tokens) {
	tokens.expect<composition>();
}

void setup_expressions() {
	parse_expression::precedence_set result;
	result.push(parse_expression::operation_set::GROUP);
	result.push_back("[", "", ",", "]");

	result.push(parse_expression::operation_set::TERNARY);
	result.push_back("", "?", ":", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "|", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "&", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "^", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "||", "");
	
	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "&&", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "^^", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "==", "");
	result.push_back("", "", "~=", "");
	result.push_back("", "", "<", "");
	result.push_back("", "", ">", "");
	result.push_back("", "", "<=", "");
	result.push_back("", "", ">=", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "<<", "");
	result.push_back("", "", ">>", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "+", "");
	result.push_back("", "", "-", "");

	result.push(parse_expression::operation_set::BINARY);
	result.push_back("", "", "*", "");
	result.push_back("", "", "/", "");
	result.push_back("", "", "%", "");

	result.push(parse_expression::operation_set::UNARY);
	result.push_back("!", "", "", "");
	result.push_back("~", "", "", "");
	result.push_back("+", "", "", "");
	result.push_back("-", "", "", "");
	result.push_back("?", "", "", "");

	result.push(parse_expression::operation_set::MODIFIER);
	result.push_back("", "'", "", "");

	result.push(parse_expression::operation_set::MODIFIER);
	result.push_back("", "{", ",", "}");
	result.push_back("", "(", ",", ")");
	result.push_back("", ".", "", "");
	result.push_back("", "[", ":", "]");
	
	result.push(parse_expression::operation_set::MODIFIER);
	result.push_back("", "::", "", "");
	parse_expression::expression::register_precedence(result);
	parse_expression::assignment::lvalueLevel = 13;
}

void register_syntax(tokenizer &tokens) {
	setup_expressions();
	composition::register_syntax(tokens);
}

}

