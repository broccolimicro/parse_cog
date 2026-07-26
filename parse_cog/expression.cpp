#include "expression.h"
#include <parse_expression/precedence.h>

namespace parse_cog {

std::shared_ptr<parse_expression::config> expression_config::cfg = 
	std::make_shared<parse_expression::config>(parse_cog::makeExprConfig());

expression_config::expression_config() {
	debug_name = "cog";
}

expression_config::~expression_config() {
}

std::shared_ptr<parse_expression::config> composition_config::cfg = 
	std::make_shared<parse_expression::config>(parse_cog::makeCompConfig());

composition_config::composition_config() {
	debug_name = "cog";
}

composition_config::~composition_config() {
}

parse_expression::config makeExprConfig() {
	parse_expression::config cfg;
	int CONSTANT = cfg.push<constant>("constant");
	int LITERAL = cfg.push<literal>("literal");
	/*int TYPE = */cfg.push<type_name>("type");
	int TERM = cfg.push<term_name>("term");
	int LABEL = cfg.push<label>("label");

	cfg.base = {LITERAL, CONSTANT};

	using operation_set=parse_expression::operation_set;

	// TODO(edward.bingham) we don't have arithmetic support for conditionals inside expressions
	/*cfg.order.push(operation_set::TERNARY);
	cfg.order.push_back("", "?", ":", "");*/

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "|", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "&", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "^", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "||", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "&&", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "^^", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "==", "");
	cfg.order.push_back("", "", "~=", "");
	cfg.order.push_back("", "", "<", "");
	cfg.order.push_back("", "", ">", "");
	cfg.order.push_back("", "", "<=", "");
	cfg.order.push_back("", "", ">=", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "<<", "");
	cfg.order.push_back("", "", ">>", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "+", "");
	cfg.order.push_back("", "", "-", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "*", "");
	cfg.order.push_back("", "", "/", "");
	cfg.order.push_back("", "", "%", "");

	cfg.order.push(operation_set::UNARY);
	cfg.order.push_back("!", "", "", "");
	cfg.order.push_back("~", "", "", "");
	cfg.order.push_back("+", "", "", "");
	cfg.order.push_back("-", "", "", "");
	cfg.order.push_back("?", "", "", "");

	cfg.set_lvalue();

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "'", "", "", {LITERAL}, {LABEL});

	cfg.order.push(operation_set::MODIFIER);
	//cfg.order.push_back("", "{", ",", "}");
	cfg.order.push_back("", "(", ",", ")", {TERM});
	cfg.order.push_back("", ".", "", "", {LITERAL}, {LITERAL});
	cfg.order.push_back("", "[", ":", "]");

	// TODO(edward.bingham) subtyping needs a type system
	/*cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "::", "", "", {TYPE}, {LABEL});*/

	cfg.order.push(operation_set::GROUP);
	cfg.order.push_back("[", "", ",", "]");

	return cfg;
}

parse_expression::config makeCompConfig() {
	parse_expression::config cfg;

	int GUARD = cfg.push<guard>("guard");
	int ASSIGN = cfg.push<assignment>("assign");

	cfg.base = {GUARD, ASSIGN};

	using operation_set=parse_expression::operation_set;

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", ":", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", ",", "");

	cfg.set_lvalue();

	return cfg;
}

}
