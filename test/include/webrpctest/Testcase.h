#ifndef __WEBRPCTEST_TESTCASE_H__
#define __WEBRPCTEST_TESTCASE_H__

#include <string>
#include <vector>

namespace webrpctest
{

using Testcase = std::pair<std::string, std::string>;
using Testcases = std::vector<Testcase>;

extern const Testcases testcases;

}
#endif