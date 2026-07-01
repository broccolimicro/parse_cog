#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

#include <memory>

#include "expression.h"

namespace parse_cog {

struct composition : parse::syntax {
	composition();
	composition(tokenizer &tokens, int level = 0, void *data = NULL);
	~composition();

	enum {
		SEQUENCE = 0,
		INTERNAL_SEQUENCE = 1,
		PARALLEL = 2,
		CONDITION = 3, // deterministic
		CHOICE = 4 // non-deterministic
	};

	vector<std::shared_ptr<syntax> > branches;
	int level;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};

}

