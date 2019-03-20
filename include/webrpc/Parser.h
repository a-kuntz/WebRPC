#ifndef __PARSER_H__
#define __PARSER_H__

#include <boost/container/stable_vector.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>
#include <boost/variant.hpp>

#include <iostream>
#include <map>
#include <string>
#include <sstream>
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
using struct_t	= std::map<string_t, value_t>;
using member_t	= std::pair<string_t, value_t>;

class value_t : boost::spirit::extended_variant<null_t, bool_t, int_t, string_t, double_t, array_t>
{
	public:
	value_t(const null_t& val = null_t()) : base_type(val) {}
	value_t(const bool_t& val) : base_type(val) {}
	value_t(const int_t& val) : base_type(val) {}
	value_t(const double_t& val) : base_type(val) {}
	value_t(const string_t& val) : base_type(val) {}
	value_t(char const * val) : base_type((string_t(val))) {}
	value_t(const array_t& val) : base_type(val) {}
	// value_t(const struct_t& val) : base_type(val) {}
	// value_t(const value_t& rhs) : base_type(rhs.get_ast()) {}

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
		std::ostream& _out;
	};

	std::ostream& serialize(std::ostream& out) const {
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

// struct request_type
// {
// 	string_t name;
// 	value_t value;
// };

// BOOST_FUSION_ADAPT_STRUCT(
// 	request_type,
// 	(string_t, name)
// 	(value_t, value)
// )

template <typename Iterator, typename Skipper>
struct grammar : qi::grammar<Iterator, value_t(), Skipper>
{
	grammar() : grammar::base_type{value_rule}
	{
		qi::lit_type             lit;
		ascii::char_type         chr;
		ascii::digit_type        digit;
		// qi::double_type          double_;
		// qi::int_parser<int64_t>  int_;
		// qi::bool_type            bool_value;
		// qi::attr_type            attr;
		qi::lexeme_type          lexeme;

		// rules initialization
		value_rule   =
			  qi::bool_
			| lexeme[!('+' | (-lit('-') >> '0' >> digit)) >> qi::int_ >> !chr(".eE")]
			| lexeme[!('+' | (-lit('-') >> '0' >> digit)) >> qi::double_]
			| string_rule
			| array_rule;

		string_rule		= qi::alpha >> *qi::alnum;
		// string_rule		= lexeme[+chr("a-zA-Z0-9")];
		// auto quote = '"' >> lexeme[*( ~char_('"') )] >> '"';

		array_rule		= lit("[") >> -(value_rule % ',') >> lit("]");

		// struct_rule		= lit('{') >> -(member_rule % ',') >> lit('}');
		// member_rule		= string_rule >> ':' >> value_rule;
		// todo: bytestring_rule

		BOOST_SPIRIT_DEBUG_NODE(value_rule);
		BOOST_SPIRIT_DEBUG_NODE(string_rule);
		BOOST_SPIRIT_DEBUG_NODE(array_rule);
		BOOST_SPIRIT_DEBUG_NODE(struct_rule);
		BOOST_SPIRIT_DEBUG_NODE(member_rule);
	}

	qi::rule<Iterator, value_t(), Skipper> value_rule;
	qi::rule<Iterator, string_t(), Skipper> string_rule;
	qi::rule<Iterator, array_t(), Skipper> array_rule;
	qi::rule<Iterator, struct_t(), Skipper> struct_rule;
	qi::rule<Iterator, member_t(), Skipper> member_rule;
};

#endif