#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

namespace parse_cog {

parse::syntax *produce(tokenizer &tokens, void *data=nullptr);
void expect(tokenizer &tokens);
void setup_expressions();
void register_syntax(tokenizer &tokens);

}

