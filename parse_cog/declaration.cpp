#include "declaration.h"
#include "adapter.h"

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
	const adapter *cfg = (const adapter*)data;

	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect<assignment>();

	if (cfg != nullptr and not cfg->type_name.empty()) {
		tokens.increment(true);
		tokens.expect(cfg->type_name.label);
	}

	tokens.increment(true);
	tokens.expect("var");

	if (tokens.decrement(__FILE__, __LINE__)) {
		tokens.next();
	}

	// type name
	if (cfg != nullptr and not cfg->type_name.empty() and tokens.decrement(__FILE__, __LINE__, cfg->type_name_data)) {
		type = cfg->type_name.factory(tokens, cfg->type_name_data);
	}

	// expr
	// TODO(edward.bingham) use the data here instead of static precedence listings
	if (tokens.decrement(__FILE__, __LINE__, nullptr)) {
		expr.parse(tokens, nullptr);
	}

	tokens.syntax_end(this);
}

bool declaration::is_next(tokenizer &tokens, int i, void *data) {
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
	return "var " + type->to_string(tab) + " " + expr.to_string(tab);
}

parse::syntax *declaration::clone() const {
	return new declaration(*this);
}

}
