#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

#include "expression.h"

#include <memory>
#include <any>

namespace parse_cog {

struct declaration : parse::syntax {
	std::shared_ptr<syntax> type;
	assignment expr;

	declaration();
	declaration(tokenizer &tokens, std::any data={});
	~declaration();
	
	void parse(tokenizer &tokens, std::any data={});
	static bool is_next(tokenizer &tokens, int i=1, std::any data={});
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;
	parse::syntax *clone() const;
};

}
