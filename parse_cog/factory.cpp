#include "factory.h"

#include "composition.h"

namespace parse_cog {

const parse::factory factory(parse::schema::from<composition>());

}

