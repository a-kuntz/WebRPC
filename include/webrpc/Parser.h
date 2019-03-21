#ifndef __PARSER_H__
#define __PARSER_H__

#include <boost/container/stable_vector.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>
#include <boost/variant.hpp>

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace boost::spirit;

// primitive types
struct null_t {};
using bool_t	= bool;
using int_t		= int;
using double_t	= double;
using string_t	= std::string;

class value_t;

// composite types
using array_t	= std::vector<value_t>;
// using array_t	= boost::container::stable_vector<value_t>;
// using struct_t	= std::map<string_t, value_t>;
using struct_t	= std::vector<std::pair<string_t, value_t>>;
using struct_member_t	= struct_t::value_type;
using bytestring_t = std::vector<int>;

class value_t : boost::spirit::extended_variant<null_t, bool_t, int_t, string_t, double_t, array_t,struct_t,bytestring_t>
{
	public:
	value_t(const null_t& val = null_t()) : base_type(val) {}
	value_t(const bool_t& val) : base_type(val) {}
	value_t(const int_t& val) : base_type(val) {}
	value_t(const double_t& val) : base_type(val) {}
	value_t(const string_t& val) : base_type(val) {}
	value_t(char const * val) : base_type((string_t(val))) {}
	value_t(const array_t& val) : base_type(val) {}
	value_t(const struct_t& val) : base_type(val) {}
	value_t(const bytestring_t& val) : base_type(val) {}

	struct serializer : public boost::static_visitor<>
	{
		serializer(std::ostream& out) : _out(out) {}
		void operator()(const null_t& /**/) const {_out << "null_t";}
		void operator()(const bool_t& v) const {_out << std::boolalpha << v;}
		void operator()(const int_t& v) const {_out << v;}
		void operator()(const double_t& v) const {_out << v;}
		void operator()(const string_t& v) const {_out << v;}
		void operator()(const value_t& v) const {v.serialize(_out);}
		void operator()(const array_t& v) const
		{
			_out << "[";
			for (auto itr = std::begin(v); itr != std::end(v); ++itr)
			{
				this->operator()(*itr);
				if (std::next(itr) != std::end(v)) {_out << ",";}
			}
			_out << "]";
		}
		void operator()(const struct_t& v) const
		{
			_out << "{";
			for (auto itr = std::begin(v); itr != std::end(v); ++itr)
			{
				this->operator()(itr->first);
				_out << ":";
				this->operator()(itr->second);
				if (std::next(itr) != std::end(v)) {_out << ",";}
			}
			_out << "}";
		}
		void operator()(const bytestring_t& v) const
		{
			_out << "<";
			for (auto itr = std::begin(v); itr != std::end(v); ++itr)
			{
				_out << std::setfill('0') << std::setw(2) << std::hex << *itr;
				if (std::next(itr) != std::end(v)) {_out << ",";}
			}
			_out << ">";
		}
		std::ostream& _out;
	};

	std::ostream& serialize(std::ostream& out) const
	{
		boost::apply_visitor(serializer{out}, base_type::get());
		return out;
	}

	std::string to_string() const
	{
		std::stringstream ss;
		serialize(ss);
		return ss.str();
	}
};

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

#endif