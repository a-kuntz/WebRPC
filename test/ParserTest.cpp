#include <webrpc/Parser.h>

#include <boost/spirit/include/qi.hpp>

#include <gtest/gtest.h>

using namespace boost::spirit;

TEST(BoostSpirit, basic)
{
	std::string input("abc/4");
	for (std::string input : {"abc/4", "abc/5"})
	{
		auto it = input.begin();
		auto match = qi::phrase_parse(it, input.end(), +qi::char_("A-Za-z") >> '/' >> +qi::char_("a-zA-Z0-9"), ascii::space);
		EXPECT_EQ(true, match);
	}
}

void check(const std::string& in)
{
	boost::optional<value_t> opt = parse_value(in);
	ASSERT_EQ(true, static_cast<bool>(opt)) << "testcase input=" << in;
	EXPECT_EQ(in, opt.value().to_string());
}

TEST(Parser, PrimitiveValues)
{
	// primitive values
	check("true");
	check("42");
	check("3.14");
	check("abc");
	check("abc_3");
	check("\"abc\"");
	check("\"a b c?\"");
}

TEST(Parser, ArrayValues)
{
	// arrays
	check("[]");
	check("[[],[],[]]");
	check("[true,false,true]");
	check("[42,43,44]");
	check("[3.14,2.7,1.4]");
	check("[abc,d_e_f,ghi]");
	check("[\"abc\",\"def\",\"ghi.,;.-_?!§$%&/()=\"]");
	check("[true,42,3.14,abc]");
	check("[true,42,3.14,abc,[true,42,3.14,abc,[true,42,3.14,abc]]]");
	check("[{key:val},{key:val},{key:val}]");
	check("[<>,<00>,<00,01,0a,0f,de,ad,01,23,45,67,89,ab,cd,ef>]");
	check("[true,42,3.14,abc,{key:value},[true,42,3.14,abc,{key:value},[true,42,3.14,abc,{key:value}]]]");
}

TEST(Parser, StructValues)
{
	// structs
	check("{}");
	check("{key:value,key2:value2,key3:value}");
	check("{key:value,key2:[1,2,3],key3:{k:v,k:v,k:v},key4:<ab,cd,ef>}");
}

TEST(Parser, BytestringValues)
{
	// bytestrings
	check("<>");
	check("<00,11,22,33,44,55,66,aa,bb,cc>");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
