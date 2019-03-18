#ifndef __PARSER_H__
#define __PARSER_H__

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>
#include <vector>
#include <iostream>

using namespace boost::spirit;

using value_type = boost::variant<double, int, bool>;
using string_type = std::string;

struct request_type
{
	string_type name;
	value_type value;
};

BOOST_FUSION_ADAPT_STRUCT(
	request_type,
	(string_type, name)
	(value_type, value)
)

using grammar_type = request_type;

template <typename Iterator, typename Skipper>
struct request_grammar : qi::grammar<Iterator, grammar_type(), Skipper>
{
	request_grammar() : request_grammar::base_type{request_rule}
	{
		// rules initialization
		name_rule    = qi::lexeme[+qi::char_("a-zA-Z0-9")] >> '/';
		value_rule   = qi::double_ | qi::int_ | qi::bool_;
		request_rule = name_rule >> value_rule;
	}

	qi::rule<Iterator, std::string(), Skipper> name_rule;
	qi::rule<Iterator, value_type(), Skipper> value_rule;
	qi::rule<Iterator, request_type(), Skipper> request_rule;
};

struct print : public boost::static_visitor<>
{
	template <typename T>
	void operator()(T t) const
	{
		std::cout << std::boolalpha << t << ';';
	}
};


#endif