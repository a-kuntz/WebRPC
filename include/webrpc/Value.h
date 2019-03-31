#ifndef __VALUE_H__
#define __VALUE_H__

#include <boost/spirit/include/support_extended_variant.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>

struct value_t;

// primitive types
struct null_t 			{};
using bool_t			= bool;
using int_t				= int;
using double_t			= double;
using string_t			= std::string;

// composite types
using array_t			= std::vector<value_t>;
using struct_member_t	= std::pair<string_t, value_t>;
using struct_t			= std::map<string_t, value_t>;

using bytestring_t		= std::vector<int>;

struct value_t : public boost::spirit::extended_variant<null_t, bool_t, int_t, string_t, double_t, array_t,struct_t,bytestring_t>
{
	value_t(const null_t& val = null_t())	: base_type(val) {}
	value_t(const bool_t& val)				: base_type(val) {}
	value_t(const int_t& val)				: base_type(val) {}
	value_t(const double_t& val)			: base_type(val) {}
	value_t(const string_t& val)			: base_type(val) {}
	value_t(char const * val)				: base_type(string_t(val)) {}
	value_t(const array_t& val)				: base_type(val) {}
	value_t(const struct_t& val)			: base_type(val) {}
	value_t(const bytestring_t& val)		: base_type(val) {}

	enum type_info {
		null_type,
		bool_type,
		int_type,
		double_type,
		string_type,
		array_type,
		struct_type,
		bytestring_type,
	};

	type_info type() const;

	std::ostream& serialize(std::ostream& out) const;

	std::string to_string() const;

	template <typename T>
	T get() const
	{
		return boost::get<T>(*this);
	}
};

#endif