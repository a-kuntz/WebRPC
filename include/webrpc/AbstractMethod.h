#ifndef __METHOD_H__
#define __METHOD_H__

#include <webrpc/IMethod.h>

class AbstractMethod : public IMethod
{
	public:
	AbstractMethod() = delete;
	AbstractMethod(const std::string& name) : _name(name) {}
	virtual ~AbstractMethod() {};

	std::string get_name() const override {return _name;}

	private:
	const std::string _name;
};

#endif