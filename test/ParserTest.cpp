#include <boost/spirit/include/qi.hpp>
#include <gtest/gtest.h>

using namespace boost::spirit;

TEST(Parser, basic){
	std::string input("abc/4");
	for (std::string input : {"abc/4", "abc/5"})
	{
		auto it = input.begin();
		auto match = qi::phrase_parse(it, input.end(), +qi::char_("A-Za-z") >> '/' >> +qi::char_("a-zA-Z0-9"), ascii::space);
		ASSERT_EQ(true, match);
	}
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
