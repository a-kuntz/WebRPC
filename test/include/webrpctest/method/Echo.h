#ifndef __WEBRPC_METHOD_ECHO_H__
#define __WEBRPC_METHOD_ECHO_H__

#include <webrpc/AbstractMethod.h>

namespace webrpctest
{
namespace method
{

struct Echo : public AbstractMethod
{
	Echo()
		: AbstractMethod("Echo")
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		return arg;
	}
};

} // namespace method
} // namespace webrpctest

#endif