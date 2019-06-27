#include <webrpc/AbstractMethod.h>
#include <webrpc/Client.h>
#include <webrpc/Server.h>

#include <gtest/gtest.h>

namespace detail
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
}

using Testcase = std::pair<std::string, std::string>;
using Testcases = std::vector<Testcase>;

static const auto testcases = Testcases{
	{"http://127.0.0.1:8080/system.list_methods", "[\"Echo\",\"system.list_methods\"]"}
	, {"http://localhost:8080/Echo/[{foo:bar},[a,b,c,1,2,3,true,false],<11,12,ab>]", "[{foo:\"bar\"},[\"a\",\"b\",\"c\",1,2,3,true,false],<0x11,0x12,0xAB>]"}
	, {"http://127.0.0.1:8080/Echo/{syntax-error}", "null_t"}
	, {"http://127.0.0.1:8080/Echo/%7Bkey:val%7D", "{key:\"val\"}"}
	, {"http://127.0.0.1:8080/favicon.ico", "Invalid webrpc request 'favicon.ico'\r"}
};

void test_server_async_call(const Testcases& args)
{
	auto ioc = boost::asio::io_context{};

	const unsigned short port = 8080;
	const boost::asio::ip::address host = boost::asio::ip::make_address("127.0.0.1");

	auto server = Server{ioc, {host, port}, 3};
	server.register_method(std::make_unique<detail::Echo>());
	server.set_verbose(true);
	server.start();

	auto reply_expected = 0;
	auto reply_cnt = 0;
	auto client = Client{ioc, false};

	for (const auto& item : args)
	{
		const auto& request = item.first;
		const auto& expected = item.second;
		std::cout << request << " => " << expected << std::endl;
		++reply_expected;
	client.async_call(request, [&](const std::string& res) {
		ASSERT_EQ(expected, res);
		if (reply_expected == ++reply_cnt) {ioc.stop();}
	});
	}

	ioc.run_for(boost::asio::chrono::seconds(3));
	ASSERT_EQ(reply_expected, reply_cnt);
}

TEST(ClientServer, AsyncSingleCalls)
{
	for (const auto& testcase : testcases)
	{
		test_server_async_call({testcase});
	}
}

TEST(ClientServer, AsyncMultipleCalls)
{
	test_server_async_call(testcases);
}

