#include <webrpc/Value.h>

#include <boost/algorithm/string.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

namespace detail
{
struct get_type : public boost::static_visitor<value_t::type_info>
{
	value_t::type_info operator()(const null_t&) const
	{
		return value_t::null_type;
	}
	value_t::type_info operator()(const bool_t&) const
	{
		return value_t::bool_type;
	}
	value_t::type_info operator()(const int_t&) const
	{
		return value_t::int_type;
	}
	value_t::type_info operator()(const double_t&) const
	{
		return value_t::double_type;
	}
	value_t::type_info operator()(const string_t&) const
	{
		return value_t::string_type;
	}
	value_t::type_info operator()(const array_t&) const
	{
		return value_t::array_type;
	}
	value_t::type_info operator()(const struct_t&) const
	{
		return value_t::struct_type;
	}
	value_t::type_info operator()(const bytestring_t&) const
	{
		return value_t::bytestring_type;
	}
};

std::string serialize(double_t dbl)
{
	std::string str = std::to_string(dbl);
	str.erase(str.find_last_not_of('0') + 1, std::string::npos);
	return str;
}
struct serializer : public boost::static_visitor<>
{
	serializer(std::ostream& out)
		: _out(out)
	{}
	void operator()(const null_t&) const
	{
		_out << "null_t";
	}
	void operator()(const bool_t& v) const
	{
		_out << std::boolalpha << v;
	}
	void operator()(const int_t& v) const
	{
		_out << v;
	}
	void operator()(const double_t& v) const
	{
		_out << serialize(v);
	}
	void operator()(const string_t& v) const
	{
		auto str{v};
		boost::replace_all(str, "\\", "\\\\");
		boost::replace_all(str, "\"", "\\\"");
		_out << '\"' << str << '\"';
	}
	void operator()(const value_t& v) const
	{
		v.serialize(_out);
	}
	void operator()(const array_t& v) const
	{
		_out << "[";
		for (auto itr = std::begin(v); itr != std::end(v); ++itr)
		{
			this->operator()(*itr);
			if (std::next(itr) != std::end(v))
			{
				_out << ",";
			}
		}
		_out << "]";
	}
	void operator()(const struct_t& v) const
	{
		_out << "{";
		for (auto itr = std::begin(v); itr != std::end(v); ++itr)
		{
			_out << itr->first;
			_out << ":";
			this->operator()(itr->second);
			if (std::next(itr) != std::end(v))
			{
				_out << ",";
			}
		}
		_out << "}";
	}
	void operator()(const bytestring_t& v) const
	{
		_out << "<";
		for (auto itr = std::begin(v); itr != std::end(v); ++itr)
		{
			_out << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << *itr;
			if (std::next(itr) != std::end(v))
			{
				_out << ",";
			}
		}
		_out << ">";
	}
	std::ostream& _out;
};
} // namespace detail

value_t::type_info value_t::type() const
{
	return boost::apply_visitor(detail::get_type{}, *this);
}

std::ostream& value_t::serialize(std::ostream& out) const
{
	boost::apply_visitor(detail::serializer{out}, base_type::get());
	return out;
}

std::string value_t::to_string() const
{
	std::stringstream ss;
	serialize(ss);
	return ss.str();
}