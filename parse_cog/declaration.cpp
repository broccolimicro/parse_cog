#include "declaration.h"

namespace parse_cog {

declaration::declaration() {
	debug_name = "cog_declaration";
}

declaration::declaration(tokenizer &tokens, void *data) {
	debug_name = "cog_declaration";
	parse(tokens, data);
}

declaration::~declaration() {
}

void declaration::parse(tokenizer &tokens, void *data) {
	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect<assignment>();

	if (not type_name.empty()) {
		tokens.increment(true);
		tokens.expect(type_name.label);
	}

	tokens.increment(true);
	tokens.expect("var");

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		tokens.next();
	}

	// type name
	if (not type_name.empty() and tokens.decrement(__FILE__, __LINE__, data)) {
		type = type_name.factory(tokens, data);
	}

	// expr
	if (tokens.decrement(__FILE__, __LINE__, data)) {
		expr.parse(tokens, data);
	}

	tokens.syntax_end(this);
}

bool declaration::is_next(tokenizer &tokens, int i, void *data) {
	return tokens.is_next("var");
}

void declaration::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<declaration>()) {
		tokens.register_syntax<declaration>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(true);
		if (not type_name.empty()) {
			type_name.register_syntax(tokens);
		}
		assignment::register_syntax(tokens);
	}
}

string declaration::to_string(string tab) const {
	return "var " + type->to_string(tab) + " " + expr.to_string(tab);
}

parse::syntax *declaration::clone() const {
	return new declaration(*this);
}

}
