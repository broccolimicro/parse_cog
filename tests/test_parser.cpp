#include <gtest/gtest.h>
#include <parse/default/line_comment.h>
#include <parse/default/block_comment.h>
#include <parse_cog/composition.h>
#include <parse_cog/control.h>
#include <parse_cog/declaration.h>
#include <parse_cog/factory.h>
#include <sstream>
#include <string>

using namespace std;
using namespace parse_cog;

namespace {

composition load_cog_string(const string &input) {
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	factory.register_syntax(tokens);

	tokens.insert("string_input", input);

	composition dut(tokens);
	EXPECT_TRUE(tokens.is_clean());
	return dut;
}

template <typename T>
const T *unwrap(const parse::syntax *s) {
	while (s != nullptr
		and s->is_a<composition>()
		and s->get<composition>().branches.size() == 1u) {
		s = s->get<composition>().branches[0].get();
	}
	if (s == nullptr) {
		return nullptr;
	}
	if (not s->is_a<T>()) {
		string target = T().debug_name;
		printf("error: trying to unwrap %s into a %s\n", s->debug_name.c_str(), target.c_str());
		return nullptr;
	}
	return &s->get<T>();
}

} // namespace

TEST(CogParser, BasicWhileLoop) {
	string cog_code = R"(
while {
	L.recv()
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);
	EXPECT_EQ(ctrl->action.branches.size(), 1u);

	const assignment *assign = unwrap<assignment>(ctrl->action.branches[0].get());
	ASSERT_NE(assign, nullptr);
	EXPECT_TRUE(assign->valid);
	EXPECT_EQ(assign->to_string(), "L.recv()");
}

TEST(CogParser, Source) {
	string cog_code = R"(
while {
	L.send(rand())
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);
	EXPECT_EQ(ctrl->action.branches.size(), 1u);

	const assignment *assign = unwrap<assignment>(ctrl->action.branches[0].get());
	ASSERT_NE(assign, nullptr);
	EXPECT_TRUE(assign->valid);
	EXPECT_EQ(assign->to_string(), "L.send(rand())");
}

TEST(CogParser, Buffer) {
	string cog_code = R"(
while {
	R.send(L.recv())
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);
	EXPECT_EQ(ctrl->action.branches.size(), 1u);

	const assignment *assign = unwrap<assignment>(ctrl->action.branches[0].get());
	ASSERT_NE(assign, nullptr);
	EXPECT_TRUE(assign->valid);
	EXPECT_EQ(assign->to_string(), "R.send(L.recv())");
}

TEST(CogParser, Copy) {
	string cog_code = R"(
while {
	var l = L.recv()
	R0.send(l) and R1.send(l)
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);

	const composition *seq = unwrap<composition>(&ctrl->action);
	ASSERT_NE(seq, nullptr);
	ASSERT_TRUE(seq->branches.size() == 2u);
	EXPECT_TRUE(seq->branches[0].get() != nullptr);
	ASSERT_TRUE(seq->branches[1].get() != nullptr and seq->branches[1]->is_a<composition>());

	const composition *para = unwrap<composition>(seq->branches[1].get());
	ASSERT_NE(para, nullptr);
	EXPECT_EQ(para->level, composition::PARALLEL);
	EXPECT_EQ(para->comp.size(), 1u);
	EXPECT_EQ(para->comp[0], "and");
	EXPECT_EQ(para->branches.size(), 2u);
}

TEST(CogParser, Split) {
	string cog_code = R"(
while {
	if Cc.probe() == 0 {
		R0.send(L.recv()) and Cc.recv()
	} or if Cc.probe() == 1 {
		R1.send(L.recv()) and Cc.recv()
	}
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);
	EXPECT_EQ(ctrl->action.branches.size(), 1u);

	const composition *choice = unwrap<composition>(&ctrl->action);
	ASSERT_NE(choice, nullptr);
	EXPECT_EQ(choice->level, composition::CONDITION);
	EXPECT_EQ(choice->branches.size(), 2u);
	EXPECT_EQ(choice->comp.size(), 1u);
	EXPECT_EQ(choice->comp[0], "or");

	for (auto &branch : choice->branches) {
		const control *if_ctrl = unwrap<control>(branch.get());
		ASSERT_NE(if_ctrl, nullptr);
		EXPECT_EQ(if_ctrl->kind, "if");
		EXPECT_TRUE(if_ctrl->guard.valid);
		EXPECT_TRUE(if_ctrl->action.valid);
		EXPECT_EQ(if_ctrl->action.branches.size(), 1u);

		const composition *para = unwrap<composition>(&if_ctrl->action);
		ASSERT_NE(para, nullptr);
		EXPECT_EQ(para->level, composition::PARALLEL);
		EXPECT_EQ(para->branches.size(), 2u);
	}
}

TEST(CogParser, Merge) {
	string cog_code = R"(
while {
	if Cc.probe() == 0 {
		R.send(L0.recv()) and Cc.recv()
	} or if Cc.probe() == 1 {
		R.send(L1.recv()) and Cc.recv()
	}
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);
	EXPECT_EQ(ctrl->action.branches.size(), 1u);

	const composition *choice = unwrap<composition>(&ctrl->action);
	ASSERT_NE(choice, nullptr);
	EXPECT_EQ(choice->level, composition::CONDITION);
	EXPECT_EQ(choice->branches.size(), 2u);
	EXPECT_EQ(choice->comp.size(), 1u);
	EXPECT_EQ(choice->comp[0], "or");

	for (auto &branch : choice->branches) {
		const control *if_ctrl = unwrap<control>(branch.get());
		ASSERT_NE(if_ctrl, nullptr);
		EXPECT_EQ(if_ctrl->kind, "if");
		EXPECT_TRUE(if_ctrl->guard.valid);
		EXPECT_TRUE(if_ctrl->action.valid);
		EXPECT_EQ(if_ctrl->action.branches.size(), 1u);

		const composition *para = unwrap<composition>(&if_ctrl->action);
		ASSERT_NE(para, nullptr);
		EXPECT_EQ(para->level, composition::PARALLEL);
		EXPECT_EQ(para->branches.size(), 2u);
	}
}

TEST(CogParser, Add) {
	string cog_code = R"(
while {
	S.send(A.recv() + B.recv())
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "while");
	EXPECT_TRUE(ctrl->action.valid);

	const assignment *assign = unwrap<assignment>(ctrl->action.branches[0].get());
	ASSERT_NE(assign, nullptr);
	EXPECT_TRUE(assign->valid);
	EXPECT_EQ(assign->to_string(), "S.send(A.recv()+B.recv())");
}

TEST(CogParser, ParallelComposition) {
	string cog_code = R"(
{
	A.send(x)
} and {
	B.send(x)
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const composition *para = unwrap<composition>(&dut);
	ASSERT_NE(para, nullptr);
	EXPECT_EQ(para->level, composition::PARALLEL);
	EXPECT_EQ(para->branches.size(), 2u);
	EXPECT_EQ(para->comp.size(), 1u);
	EXPECT_EQ(para->comp[0], "and");
}

TEST(CogParser, ConditionalChoice) {
	string cog_code = R"(
if c == 0 {
	A.send(x)
} or if c == 1 {
	B.send(x)
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const composition *choice = unwrap<composition>(&dut);
	ASSERT_NE(choice, nullptr);
	EXPECT_EQ(choice->branches.size(), 2u);
	EXPECT_EQ(choice->comp.size(), 1u);
	EXPECT_EQ(choice->comp[0], "or");

	const control *first = unwrap<control>(choice->branches[0].get());
	const control *second = unwrap<control>(choice->branches[1].get());
	ASSERT_NE(first, nullptr);
	ASSERT_NE(second, nullptr);
	EXPECT_EQ(first->kind, "if");
	EXPECT_EQ(second->kind, "if");
	EXPECT_TRUE(first->guard.valid);
	EXPECT_TRUE(second->guard.valid);
}

TEST(CogParser, VariableDeclaration) {
	string cog_code = R"(
{
	var l
	l = L.recv()
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const composition *seq = unwrap<composition>(&dut);
	ASSERT_NE(seq, nullptr);
	EXPECT_EQ(seq->branches.size(), 2u);

	const declaration *decl = unwrap<declaration>(seq->branches[0].get());
	ASSERT_NE(decl, nullptr);
	EXPECT_TRUE(decl->expr.valid);

	const assignment *assign = unwrap<assignment>(seq->branches[1].get());
	ASSERT_NE(assign, nullptr);
	EXPECT_TRUE(assign->valid);
	EXPECT_EQ(assign->to_string(), "l=L.recv()");
}

TEST(CogParser, AwaitGuard) {
	string cog_code = R"(
await b {
	L.recv()
}
)";

	composition dut = load_cog_string(cog_code);
	EXPECT_TRUE(dut.valid);

	const control *ctrl = unwrap<control>(&dut);
	ASSERT_NE(ctrl, nullptr);
	EXPECT_EQ(ctrl->kind, "await");
	EXPECT_TRUE(ctrl->guard.valid);
	EXPECT_EQ(ctrl->guard.to_string(), "b");
	EXPECT_TRUE(ctrl->action.valid);
}

TEST(CogParser, ParseAndRegenerate) {
	vector<string> test_cases = {
		R"(skip)",

		R"(while {
	L.recv()
})",

		R"(while {
	R.send(L.recv())
})",

		R"(while {
	var l
	l=L.recv()
	R0.send(l) and R1.send(l)
})",

		R"(while {
	if Cc.probe()==0 {
		R0.send(L.recv()) and Cc.recv()
	} or if Cc.probe()==1 {
		R1.send(L.recv()) and Cc.recv()
	}
})",

		R"(while {
	S.send(A.recv()+B.recv())
})",
	};

	for (const auto &test_case : test_cases) {
		tokenizer tokens;
		tokens.register_token<parse::block_comment>(false);
		tokens.register_token<parse::line_comment>(false);
		factory.register_syntax(tokens);
		tokens.insert("test_case", test_case);

		composition dut(tokens);
		EXPECT_TRUE(dut.valid);
		EXPECT_TRUE(tokens.is_clean());

		EXPECT_EQ(test_case, dut.to_string());
	}
}
