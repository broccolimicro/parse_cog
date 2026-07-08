#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

#include "expression.h"

#include <memory>

namespace parse_cog {

struct declaration : parse::syntax {
	std::shared_ptr<syntax> type;
	assignment expr;

	declaration();
	declaration(tokenizer &tokens, void *data=nullptr);
	~declaration();
	
	void parse(tokenizer &tokens, void *data=nullptr);
	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;
	parse::syntax *clone() const;
};

}
