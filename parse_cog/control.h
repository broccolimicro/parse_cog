#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include "expression.h"

#include "composition.h"

namespace parse_cog {

struct control : parse::syntax {
	string kind;
	string region;
	expression guard;
	composition action;

	control();
	control(tokenizer &tokens, void *data = NULL);
	~control();

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};

}

