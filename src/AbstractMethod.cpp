#include <webrpc/AbstractMethod.h>

AbstractMethod::AbstractMethod(const std::string& name)
: _name(name)
{}

std::string AbstractMethod::get_name() const
{
	return _name;
}
