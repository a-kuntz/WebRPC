#ifndef __URI_H__
#define __URI_H__

#include <string>
#include <optional>

struct Uri
{
    Uri() = default;

    Uri(const std::string& host, const std::optional<unsigned> port, const std::string& target)
	: host{host}, port{std::to_string(port?*port:8080)}, target{target}
    {}

    std::string host;
    std::string port;
    std::string target;
};

#endif // __URI_H__
