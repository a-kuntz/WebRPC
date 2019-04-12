#ifndef __PARSER_H__
#define __PARSER_H__

#include <webrpc/Uri.h>
#include <webrpc/Value.h>

#include <boost/optional.hpp>

boost::optional<Uri> parse_uri(const std::string& str);

boost::optional<value_t> parse_value(const std::string& str);

#endif
