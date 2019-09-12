#include <webrpctest/Testcase.h>

namespace webrpctest
{

const Testcases testcases {
	{"http://127.0.0.1:8080/?", "[\"?\",\"Echo\"]"}
	, {"http://localhost:8080/Echo/[{foo:bar},[a,b,c,1,2,3,true,false],<11,12,ab>]", "[{foo:\"bar\"},[\"a\",\"b\",\"c\",1,2,3,true,false],<0x11,0x12,0xAB>]"}
	, {"http://127.0.0.1:8080/Echo/{syntax-error}", "null_t"}
	, {"http://127.0.0.1:8080/Echo/%7Bkey:val%7D", "{key:\"val\"}"}
	, {"http://127.0.0.1:8080/favicon.ico", "Invalid webrpc request 'favicon.ico'\r"}
};

}
