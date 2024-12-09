#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

#include "branch.h"

namespace parse_cog
{

struct composition : parse::syntax
{
	composition();
	composition(tokenizer &tokens, int level = 0, void *data = NULL);
	~composition();

	enum {
		SEQUENCE = 0,
		PARALLEL = 1,
		CONDITION = 2, // deterministic
		CHOICE = 3 // non-deterministic
	};

	vector<parse_cog::branch> branches;
	int level;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int prev_level, string tab = "") const;
	parse::syntax *clone() const;
};

}

