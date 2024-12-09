#include "control.h"
#include "composition.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/new_line.h>

namespace parse_cog
{

control::control()
{
	debug_name = "control";
}

control::control(tokenizer &tokens, void *data)
{
	debug_name = "control";
	parse(tokens, data);
}

control::~control()
{

}

void control::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect("while");
	tokens.expect("await");
	tokens.expect("assume");
	tokens.expect("region");

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		kind = tokens.next();
	}

	if (kind == "while"
		or kind == "region") {
		tokens.increment(true);
		tokens.expect("{");
	} if (kind == "await") {
		tokens.increment(false);
		tokens.expect("{");
	}

	if (kind == "region") {
		tokens.increment(false);
		tokens.expect<parse::number>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			region = tokens.next();
		}
	} else {
		tokens.increment(false);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			guard.parse(tokens, data);
		}
	}

	if (kind == "while"
		or kind == "await"
		or kind == "region") {
		if (tokens.decrement(__FILE__, __LINE__, data)) {
			tokens.next();

			tokens.increment(true);
			tokens.expect("}");

			tokens.increment(true);
			tokens.expect<composition>();

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				action.parse(tokens, data);
			}

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				tokens.next();
			}
		}
	}

	tokens.syntax_end(this);
}

bool control::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("while", i)
		or tokens.is_next("when", i)
		or tokens.is_next("await", i)
		or tokens.is_next("assume", i)
		or tokens.is_next("region", i);
}

void control::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<control>())
	{
		tokens.register_syntax<control>();
		expression::register_syntax(tokens);
		composition::register_syntax(tokens);
		tokens.register_token<parse::number>();
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(false);
	}
}

string control::to_string(string tab) const
{
	if (!valid)
		return tab+"skip";

	string result = tab+kind;
	if (guard.valid) {
		result += " " + guard.to_string(tab);
	} else if (region != "") {
		result += " " + region;
	}

	if (action.valid) {
		result += " {\n" + action.to_string(tab+"\t") + "\n" + tab + "}";
	}

	return result;
}

parse::syntax *control::clone() const
{
	return new control(*this);
}
}
