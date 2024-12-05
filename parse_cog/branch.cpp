#include "branch.h"

#include "composition.h"
#include "control.h"

namespace parse_cog {

branch::branch()
{

}

branch::branch(composition sub)
{
	this->sub = std::shared_ptr<composition>(new composition(sub));
}

branch::branch(control ctrl)
{
	this->ctrl = std::shared_ptr<control>(new control(ctrl));
}

branch::branch(assignment assign)
{
	this->assign = assign;
}

branch::~branch()
{

}

string branch::to_string(int level, string tab) const
{
	if (sub and sub->valid) {
		return sub->to_string(level, tab);
	} else if (ctrl and ctrl->valid) {
		return ctrl->to_string(tab);
	} else if (assign.valid) {
		return assign.to_string(tab);
	}
	return "skip";
}

}
