#ifndef __METHOD_H__
#define __METHOD_H__

#include <webrpc/IMethod.h>

class Method : public IMethod
{
	public:
	Method();
	std::string execute(const std::string& args) override;
};

#endif