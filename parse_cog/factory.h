#pragma once

#include "composition.h"
#include "control.h"

namespace parse_cog {

parse::syntax *produce(tokenizer &tokens, void *data=nullptr);
void expect(tokenizer &tokens);
void register_syntax(tokenizer &tokens);

}

