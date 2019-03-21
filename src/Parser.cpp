#include <webrpc/Parser.h>

boost::optional<value_t> parse_value(const std::string& str)
{
	std::string input(str);
	auto itr = input.begin();
	grammar<std::string::iterator, ascii::space_type> grammar;
	value_t value;
	if (qi::phrase_parse(itr, input.end(), grammar, ascii::space, value))
	{
		return value;
	}
	else
	{
		return boost::none;
	}
}
