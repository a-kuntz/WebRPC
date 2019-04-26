#ifndef __PARSER_H__
#define __PARSER_H__

#include <webrpc/Target.h>
#include <webrpc/Uri.h>
#include <webrpc/Value.h>

#include <boost/optional.hpp>

namespace Parser
{

boost::optional<Uri> parse_uri(const std::string& str);

boost::optional<Target> parse_target(const std::string& str);

boost::optional<value_t> parse_value(const std::string& str);

} // ns Parser
#endif
