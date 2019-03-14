#include <webrpc/Method.h>

Method::Method(){};

std::string Method::execute(const std::string& args)
{
	return args;
};