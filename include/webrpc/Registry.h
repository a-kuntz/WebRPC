#ifndef __WEBRPC_REGISTRY_H__
#define __WEBRPC_REGISTRY_H__

#include <webrpc/IMethod.h>

using Registry = std::map<std::string, IMethodUP>;

#endif