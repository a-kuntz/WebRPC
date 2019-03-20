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
		ASSERT_EQ(true, match);
	}
}

void check(const std::string& in)
{
	std::string input(in);
	auto itr = input.begin();
	grammar<std::string::iterator, ascii::space_type> grammar;
	value_t value;
	const auto match = qi::phrase_parse(itr, input.end(), grammar, ascii::space, value);
	ASSERT_EQ(true, match) << "testcase input=" << in;
	ASSERT_EQ(in, value.to_string());
}

TEST(Parser, Grammar)
{
	check("true");
	check("42");
	check("3.14");
	check("abc");
	// todo: check quoted string

	check("[true,false,true]");
	check("[42,43,44]");
	check("[3.14,2.7,1.4]");
	check("[abc,def,ghi]");
	check("[true,42,3.14,abc]");
	check("[true,42,3.14,abc,[true,42,3.14,abc,[true,42,3.14,abc]]]");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
