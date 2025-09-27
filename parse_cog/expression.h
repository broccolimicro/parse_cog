#pragma once

#include <parse_expression/expression.h>
#include <parse_expression/assignment.h>

namespace parse_cog {

using expression=parse_expression::expression_t<0>;
using assignment=parse_expression::assignment_t<0>;

void setup_expressions();

}
