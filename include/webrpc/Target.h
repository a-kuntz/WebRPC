#ifndef __TARGET_H__
#define __TARGET_H__

#include <boost/optional.hpp>

#include <string>

struct Target
{
	Target() = default;

	Target(const std::string& method, const boost::optional<std::string>& args)
		: method{method}
		, args{args}
	{}

	std::string					 method;
	boost::optional<std::string> args;
};

#endif // __TARGET_H__
