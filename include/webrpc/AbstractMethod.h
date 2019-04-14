#ifndef __METHOD_H__
#define __METHOD_H__

#include <webrpc/IMethod.h>

class AbstractMethod : public IMethod
{
	AbstractMethod() = delete;

	public:
	AbstractMethod(const std::string& name);
	virtual ~AbstractMethod() = default;

	std::string get_name() const override;

	private:
	const std::string _name;
};

#endif
