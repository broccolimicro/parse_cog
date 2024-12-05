#pragma once

#include "composition.h"
#include "control.h"

namespace parse_cog
{
	parse::syntax *produce(tokenizer &tokens, void *data)
	{
		return new composition(tokens, data);
	};

	string factory_type()
	{
		return "[" + composition().debug_name + "]";
	}
}

