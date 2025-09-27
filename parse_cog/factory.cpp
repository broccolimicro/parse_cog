#include "factory.h"

#include "expression.h"
#include "composition.h"

namespace parse_cog {

parse::syntax *produce(tokenizer &tokens, void *data) {
	return new composition(tokens, 0, data);
}

void expect(tokenizer &tokens) {
	tokens.expect<composition>();
}

void register_syntax(tokenizer &tokens) {
	setup_expressions();
	composition::register_syntax(tokens);
}

}

