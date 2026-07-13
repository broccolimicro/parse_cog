#pragma once

#include <parse/factory.h>

namespace parse_cog {

struct adapter {
	parse::factory type_name;

	adapter();
	~adapter();
};

}

