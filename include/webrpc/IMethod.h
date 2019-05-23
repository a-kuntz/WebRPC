#ifndef __I_METHOD_H__
#define __I_METHOD_H__

#include <webrpc/Value.h>

#include <boost/optional.hpp>

#include <memory>
#include <string>

struct IMethod;

using IMethodUP = std::unique_ptr<IMethod>;

struct IMethod
{
  public:
	virtual ~IMethod()															 = default;
	virtual std::string				 get_name() const							 = 0;
	virtual boost::optional<value_t> execute(const boost::optional<value_t> arg) = 0;
};

#endif