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
	request_grammar<std::string::iterator, ascii::space_type> grammar;
	// const auto match = qi::parse(itr, input.end(), grammar, ascii::space);
	grammar_type grammar_value;
	const auto match = qi::phrase_parse(itr, input.end(), grammar, ascii::space, grammar_value);
	ASSERT_EQ(true, match);
}

TEST(Parser, Grammar)
{
	check("name/3.14,42,true,false");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
