#include <webrpc/Parser.h>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/qi.hpp>

#include <iostream>

using namespace boost::spirit;

template <typename Iterator, typename Skipper>
struct grammar : qi::grammar<Iterator, value_t(), Skipper>
{
	grammar() : grammar::base_type{value_rule}
	{
		qi::lit_type             lit;
		ascii::char_type         chr;
		ascii::digit_type        digit;
		qi::lexeme_type          lexeme;
		qi::int_parser<int, 16>  hex;

		// rules initialization
		value_rule =
			  qi::bool_
			| lexeme[!('+' | (-lit('-') >> '0' >> digit)) >> qi::int_ >> !chr(".eE")]
			| lexeme[!('+' | (-lit('-') >> '0' >> digit)) >> qi::double_]
			| array_rule
			| struct_rule
			| string_rule
			| quote_rule
			| bytestring_rule
			;

		string_rule			= lexeme[+chr("a-zA-Z_") >> *chr("a-zA-Z0-9_")];
		quote_rule			= chr('"') >> lexeme[*( ~chr('"') )] >> chr('"');
		array_rule			= lit("[") >> -(value_rule % ',') >> lit("]");
		struct_rule			= lit('{') >> -(struct_member_rule % ',') >> lit('}');
		struct_member_rule	= string_rule >> ':' >> value_rule;
		bytestring_rule		= lit('<') >> -(hex % ',') >> lit('>');

		BOOST_SPIRIT_DEBUG_NODE(value_rule);
		BOOST_SPIRIT_DEBUG_NODE(string_rule);
		BOOST_SPIRIT_DEBUG_NODE(array_rule);
		BOOST_SPIRIT_DEBUG_NODE(struct_rule);
		BOOST_SPIRIT_DEBUG_NODE(struct_member_rule);
		BOOST_SPIRIT_DEBUG_NODE(bytestring_rule);
	}

	qi::rule<Iterator, value_t(), Skipper> value_rule;
	qi::rule<Iterator, string_t(), Skipper> string_rule;
	qi::rule<Iterator, string_t(), Skipper> quote_rule;
	qi::rule<Iterator, array_t(), Skipper> array_rule;
	qi::rule<Iterator, struct_t(), Skipper> struct_rule;
	qi::rule<Iterator, struct_member_t(), Skipper> struct_member_rule;
	qi::rule<Iterator, bytestring_t(), Skipper> bytestring_rule;
};

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
