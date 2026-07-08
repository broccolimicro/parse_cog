#pragma once

#include <parse/schema.h>

namespace parse_cog {

struct adapter {
	parse::schema type_name;
	void *type_name_data;

	adapter();
	~adapter();
};

}

