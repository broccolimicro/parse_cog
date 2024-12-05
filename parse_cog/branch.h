#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse_expression/assignment.h>

#include <memory>

namespace parse_cog
{
using parse_expression::assignment;

struct composition;
struct control;

struct branch
{
	branch();
	branch(composition sub);
	branch(control ctrl);
	branch(assignment assign);
	~branch();

	std::shared_ptr<composition> sub;
	std::shared_ptr<control> ctrl;
	assignment assign;

	string to_string(int level, string tab) const;
};

}
