#ifndef __WEBRPC_METHOD_SYSTEMLISTMETHODS_H__
#define __WEBRPC_METHOD_SYSTEMLISTMETHODS_H__

#include <webrpc/AbstractMethod.h>
#include <webrpc/Registry.h>

namespace method
{
struct SystemListMethods : public AbstractMethod
{
	SystemListMethods() = delete;
	SystemListMethods(const Registry& registry)
	:	AbstractMethod("system.list_methods"),
		_registry(registry)
	{};

	boost::optional<value_t> execute(const boost::optional<value_t> /*arg*/) override
	{
		array_t res;
		for (const auto& item : _registry)
		{
			res.push_back(item.first);
		}
		return value_t{res};
	}

	const Registry& _registry;
};
}

#endif