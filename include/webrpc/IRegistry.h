#ifndef __I_REGISTRY__
#define __I_REGISTRY__

#include <webrpc/Method.h>

#include <string>

class IRegistry
{
	public:
	virtual void register_method(const Method& method) = 0;
	virtual void execute_method(const std::string& name) = 0;
};

#endif