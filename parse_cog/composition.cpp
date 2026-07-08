#include "composition.h"

#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/new_line.h>

#include "control.h"
#include "expression.h"
#include "declaration.h"

namespace parse_cog {

composition::composition() {
	debug_name = "cog_composition";
	level = 0;
}

composition::composition(tokenizer &tokens, int level, void *data) {
	debug_name = "cog_composition";
	this->level = level;
	parse(tokens, data);
}

composition::~composition() {
}

void composition::parse(tokenizer &tokens, void *data) {
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
			comp.push_back(tokens.next());
		}

		tokens.increment(false);
		tokens.expect<parse::new_line>();

		while (tokens.decrement(__FILE__, __LINE__)) {
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
			tokens.expect<declaration>();
			tokens.expect("{");
			tokens.expect("skip");
		}

		// TODO(edward.bingham) move the decrement data value to the expect
		// function and store different data pointers for each expected token
		if (tokens.decrement(__FILE__, __LINE__)) {
			if (tokens.found<composition>()) {
				branches.push_back(std::make_shared<composition>(tokens, level+1, data));
			} else if (tokens.found<control>()) {
				branches.push_back(std::make_shared<control>(tokens, data));
			} else if (tokens.found<assignment>()) {
				branches.push_back(std::make_shared<assignment>(tokens, nullptr));
			} else if (tokens.found<declaration>()) {
				branches.push_back(std::make_shared<declaration>(tokens, data));
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

				while (tokens.decrement(__FILE__, __LINE__)) {
					tokens.next();

					tokens.increment(false);
					tokens.expect<parse::new_line>();
				}

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					branches.push_back(std::make_shared<composition>(tokens, 0, data));
				}

				if (tokens.decrement(__FILE__, __LINE__)) {
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
				tokens.expect("else");
			} else if (level == CHOICE) {
				tokens.expect("xor");
			}
		} else {
			break;
		}
	} while (tokens.decrement(__FILE__, __LINE__));

	tokens.syntax_end(this);
}

bool composition::is_next(tokenizer &tokens, int i, void *data) {
	while (tokens.is_next<parse::new_line>(i)) {
		i++;
	}

	return tokens.is_next("skip", i)
		or tokens.is_next("{", i)
		or declaration::is_next(tokens, i, data)
		or control::is_next(tokens, i, data)
		or assignment::is_next(tokens, i, nullptr);
}

void composition::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<composition>()) {
		setup_expressions();
		tokens.register_syntax<composition>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(true);
		control::register_syntax(tokens);
		assignment::register_syntax(tokens);
		declaration::register_syntax(tokens);
	}
}

string composition::to_string(string tab) const {
	if (!valid || branches.empty())
		return tab+"skip";

	string result = "";
	size_t j = 0;
	for (auto i = branches.begin(); i != branches.end(); i++) {
		if (i != branches.begin()) {
			result += comp[j];
			++j;
		}

		if ((*i)->is_a<composition>() and (*i)->get<composition>().level < level) {
			result += tab + "{\n" + (*i)->to_string(tab+"\t") + "\n" + tab + "}";
		} else {
			result += (*i)->to_string(tab);
		}
	}

	return result;
}

parse::syntax *composition::clone() const {
	composition *result = new composition();
	result->level = level;
	for (auto i = branches.begin(); i != branches.end(); i++) {
		result->branches.push_back(std::shared_ptr<syntax>((*i)->clone()));
	}
	return result;
}

}
