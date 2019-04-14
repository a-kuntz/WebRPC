#include <webrpc/Parser.h>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

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
		quote_rule			= lit('"') >> lexeme[*( ~chr('"') )] >> lit('"');
		array_rule			= lit("[") >> -(value_rule % ',') >> lit("]");
		struct_rule			= lit('{') >> -(struct_member_rule % ',') >> lit('}');
		struct_member_rule	= string_rule >> ':' >> value_rule;
		bytestring_rule		= lit('<') >> -((-lit("0x") >> hex) % ',') >> lit('>');

//		BOOST_SPIRIT_DEBUG_NODE(value_rule);
//		BOOST_SPIRIT_DEBUG_NODE(string_rule);
//		BOOST_SPIRIT_DEBUG_NODE(array_rule);
//		BOOST_SPIRIT_DEBUG_NODE(struct_rule);
//		BOOST_SPIRIT_DEBUG_NODE(struct_member_rule);
//		BOOST_SPIRIT_DEBUG_NODE(bytestring_rule);
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
		return std::move(value);
	}
	else
	{
		return boost::none;
	}
}

using uri_t = std::tuple<std::string, boost::optional<unsigned>, std::string>;

template <typename Iterator, typename Skipper>
struct uri_grammar : qi::grammar<Iterator, uri_t(), Skipper>
{
	uri_grammar() : uri_grammar::base_type{uri_rule}
	{
		ascii::char_type         chr;
		qi::lexeme_type          lexeme;

		// rules initialization
		uri_rule	= "http://" >> host_rule >> -(':' >> port_rule) >> '/' >> target_rule;
		host_rule	= lexeme[+chr("a-zA-Z0-9_-.")];
//		host_rule	= lexeme[+qi::char_ - ':'];
		port_rule	= qi::uint_;
		target_rule	= lexeme[*qi::char_];

//		BOOST_SPIRIT_DEBUG_NODE(uri_rule);
//		BOOST_SPIRIT_DEBUG_NODE(host_rule);
//		BOOST_SPIRIT_DEBUG_NODE(port_rule);
//		BOOST_SPIRIT_DEBUG_NODE(args_rule);

//		qi::debug(uri_rule);
//		qi::debug(host_rule);
//		qi::debug(port_rule);
//		qi::debug(args_rule);
	}

	qi::rule<Iterator, uri_t(), Skipper> uri_rule;
	qi::rule<Iterator, std::string(), Skipper> host_rule;
	qi::rule<Iterator, boost::optional<unsigned>(), Skipper> port_rule;
	qi::rule<Iterator, std::string(), Skipper> target_rule;
};

boost::optional<Uri> parse_uri(const std::string& str)
{
	std::string input(str);
	auto itr = input.begin();
	uri_grammar<std::string::iterator, ascii::space_type> grammar;
	uri_t uri;
	if (qi::phrase_parse(itr, input.end(), grammar, ascii::space, uri))
	{
		return Uri{std::get<0>(uri), std::get<1>(uri), std::get<2>(uri)};
	}
	else
	{
		return boost::none;
	}
}

using target_t = std::tuple<std::string, boost::optional<std::string>>;

template <typename Iterator, typename Skipper>
struct target_grammar : qi::grammar<Iterator, target_t(), Skipper>
{
	target_grammar() : target_grammar::base_type{target_rule}
	{
		ascii::char_type         chr;
		qi::lexeme_type          lexeme;

		// rules initialization
		target_rule	= method_rule >> -('/' >> args_rule);
		method_rule	= lexeme[+chr("a-zA-Z0-9_.-")];
//		method_rule	= lexeme[+chr >> !chr("/")];
		args_rule	= lexeme[*qi::char_];

//		BOOST_SPIRIT_DEBUG_NODE(target_rule);
//		BOOST_SPIRIT_DEBUG_NODE(method_rule);
//		BOOST_SPIRIT_DEBUG_NODE(args_rule);

//		qi::debug(target_rule);
//		qi::debug(method_rule);
//		qi::debug(args_rule);
	}

	qi::rule<Iterator, target_t(), Skipper> target_rule;
	qi::rule<Iterator, std::string(), Skipper> method_rule;
	qi::rule<Iterator, boost::optional<std::string>(), Skipper> args_rule;
};

boost::optional<Target> parse_target(const std::string& str)
{
	std::string input(str);
	auto itr = input.begin();
	target_grammar<std::string::iterator, ascii::space_type> grammar;
	target_t target;
	if (qi::phrase_parse(itr, input.end(), grammar, ascii::space, target))
	{
		return Target{std::get<0>(target), std::get<1>(target)};
	}
	else
	{
		return boost::none;
	}
}
