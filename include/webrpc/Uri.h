#ifndef __URI_H__
#define __URI_H__

#include <string>
#include <iostream>
#include <optional>

struct Uri
{
    Uri() = default;

    Uri(const std::string& host, const std::optional<unsigned> port, const std::string& request)
	: host{host}, port{std::to_string(port?*port:8080)}, request{request}
    {}

    std::string host;
    std::string port;
    std::string request;
};

#endif // __URI_H__
