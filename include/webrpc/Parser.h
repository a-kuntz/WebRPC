#ifndef __PARSER_H__
#define __PARSER_H__

#include <webrpc/Value.h>

boost::optional<value_t> parse_value(const std::string& str);

#endif