#ifndef __REGISTRY__
#define __REGISTRY__

#include <webrpc/IRegistry.h>

class Registry : public IRegistry
{
	public:
	Registry();

	void register_method(const Method& method) override;
	void execute_method(const std::string& name) override;
};

#endif