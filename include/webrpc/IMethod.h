#ifndef __I_METHOD_H__
#define __I_METHOD_H__

#include <string>

struct IMethod
{
	public:
	virtual std::string execute(const std::string& args) = 0;
};

#endif