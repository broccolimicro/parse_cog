#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include "expression.h"
#include <parse_ucs/declaration.h>

#include <memory>

namespace parse_cog {

struct composition;
struct control;

struct branch {
	using declaration=parse_ucs::declaration_t<expression>;

	branch();
	branch(composition sub);
	branch(control ctrl);
	branch(assignment assign);
	branch(declaration decl);
	~branch();

	std::shared_ptr<composition> sub;
	std::shared_ptr<control> ctrl;
	assignment assign;
	declaration decl;

	string to_string(int level, string tab) const;
};

}
