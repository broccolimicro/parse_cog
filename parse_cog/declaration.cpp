#include "declaration.h"
#include "adapter.h"

namespace parse_cog {

declaration::declaration() {
	debug_name = "cog_declaration";
}

declaration::declaration(tokenizer &tokens, std::any data) {
	debug_name = "cog_declaration";
	parse(tokens, data);
}

declaration::~declaration() {
}

void declaration::parse(tokenizer &tokens, std::any data) {
	adapter cfg;
	if (data.has_value()) {
		cfg = std::any_cast<adapter>(data);
	}

	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect<assignment>();

	if (not cfg.type_name.empty()) {
		tokens.increment(true);
		cfg.type_name.expect(tokens);
	}

	tokens.increment(true);
	tokens.expect("var");

	if (tokens.decrement(__FILE__, __LINE__)) {
		tokens.next();
	}

	// type name
	if (not cfg.type_name.empty() and tokens.decrement(__FILE__, __LINE__)) {
		type = std::shared_ptr<syntax>(cfg.type_name.produce(tokens));
	}

	// expr
	// TODO(edward.bingham) use the data here instead of static precedence listings
	if (tokens.decrement(__FILE__, __LINE__)) {
		expr.parse(tokens);
	}

	tokens.syntax_end(this);
}

bool declaration::is_next(tokenizer &tokens, int i, std::any data) {
	return tokens.is_next("var", i);
}

void declaration::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<declaration>()) {
		tokens.register_syntax<declaration>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(true);
		assignment::register_syntax(tokens);
	}
}

string declaration::to_string(string tab) const {
	string result = "var ";
	if (type != nullptr) {
		result += type->to_string(tab) + " ";
	}
	result += expr.to_string(tab);
	return result;
}

parse::syntax *declaration::clone() const {
	return new declaration(*this);
}

}
