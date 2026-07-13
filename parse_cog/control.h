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
	control(tokenizer &tokens, std::any data=std::any());
	~control();

	void parse(tokenizer &tokens, std::any data=std::any());
	static bool is_next(tokenizer &tokens, int i = 1, std::any data=std::any());
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};

}

