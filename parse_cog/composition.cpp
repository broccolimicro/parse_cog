#include "composition.h"

#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/new_line.h>

#include "branch.h"
#include "control.h"

namespace parse_cog
{

composition::composition()
{
	debug_name = "composition";
	level = 0;
}

composition::composition(tokenizer &tokens, int level, void *data)
{
	debug_name = "composition";
	this->level = level;
	parse(tokens, data);
}

composition::~composition()
{
}

void composition::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	bool first = true;
	do {
		if (first) {
			first = false;
		} else {
			/*if (tokens.found<parse::new_line>() and not control::is_next(tokens, 1) and not assignment::is_next(tokens, 1) and not tokens.is_next("{") and not tokens.is_next("skip")) {
				tokens.next();
				break;
			}*/
			tokens.next();
		}

		tokens.increment(false);
		tokens.expect<parse::new_line>();

		while (tokens.decrement(__FILE__, __LINE__, data)) {
			tokens.next();

			tokens.increment(false);
			tokens.expect<parse::new_line>();
		}

		tokens.increment(level != SEQUENCE);
		if (level < CHOICE) {
			tokens.expect<composition>();
		} else {
			tokens.expect<control>();
			tokens.expect<assignment>();
			tokens.expect("{");
			tokens.expect("skip");
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			if (tokens.found<composition>()) {
				branches.push_back(branch(composition(tokens, level+1, data)));
			} else if (tokens.found<control>()) {
				branches.push_back(branch(control(tokens, data)));
			} else if (tokens.found<assignment>()) {
				branches.push_back(branch(assignment(tokens, data)));
			} else if (tokens.found("skip")) {
				tokens.next();
			} else if (tokens.found("{")) {
				tokens.next();

				tokens.increment(true);
				tokens.expect("}");

				tokens.increment(true);
				tokens.expect<composition>();

				tokens.increment(false);
				tokens.expect<parse::new_line>();

				while (tokens.decrement(__FILE__, __LINE__, data)) {
					tokens.next();

					tokens.increment(false);
					tokens.expect<parse::new_line>();
				}

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					branches.push_back(branch(composition(tokens, 0, data)));
				}

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					tokens.next();
				}
			}

			tokens.increment(false);
			if (level == SEQUENCE) {
				tokens.expect<parse::new_line>();
			} else if (level == INTERNAL_SEQUENCE) {
				tokens.expect(";");
			} else if (level == PARALLEL) {
				tokens.expect("and");
			} else if (level == CONDITION) {
				tokens.expect("or");
			} else if (level == CHOICE) {
				tokens.expect("xor");
			}
		} else {
			break;
		}
	} while (tokens.decrement(__FILE__, __LINE__, data));

	tokens.syntax_end(this);
}

bool composition::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("skip")
		or tokens.is_next("{", i)
		or control::is_next(tokens, i, data)
		or assignment::is_next(tokens, i, data);
}

void composition::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<composition>())
	{
		tokens.register_syntax<composition>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(true);
		control::register_syntax(tokens);
		assignment::register_syntax(tokens);
	}
}

string composition::to_string(string tab) const
{
	return to_string(-1, tab);
}

string composition::to_string(int prev_level, string tab) const
{
	if (!valid || branches.empty())
		return tab+"skip";

	string result = "";
	string subtab = tab;
	if (prev_level > level) {
		result += tab + "{\n";
		subtab += "\t";
	}

	for (auto branch = branches.begin(); branch != branches.end(); branch++) {
		if (branch != branches.begin()) {
			if (level == SEQUENCE) {
				result += "\n";
			} else if (level == CONDITION) {
				result += " or ";
			} else if (level == CHOICE) {
				result += " xor ";
			} else if (level == PARALLEL) {
				result += " and ";
			} else if (level == INTERNAL_SEQUENCE) {
				result += "; ";
			}
		}

		result += branch->to_string(level, subtab);
	}

	if (prev_level > level)
		result += "\n" + tab + "}";

	return result;
}

parse::syntax *composition::clone() const
{
	return new composition(*this);
}

}
