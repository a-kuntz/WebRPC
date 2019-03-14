#ifndef __METHOD_H__
#define __METHOD_H__

#include <webrpc/IMethod.h>

class Method : public IMethod
{
	public:
	Method() = delete;
	Method(const std::string& name) : _name(name) {}

	std::string get_name() const override {return _name;}
	std::string execute(const std::string& args) override;

	private:
	const std::string _name;
};

#endif