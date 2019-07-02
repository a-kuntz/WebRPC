#include <webrpctest/Testcase.h>
#include <webrpctest/method/Echo.h>

#include <webrpc/AbstractMethod.h>
#include <webrpc/Client.h>
#include <webrpc/Server.h>

#include <gtest/gtest.h>

namespace webrpctest
	{

void test_server_async_call(const Testcases& args)
{
	auto ioc = boost::asio::io_context{};

	const unsigned short port = 8080;
	const boost::asio::ip::address host = boost::asio::ip::make_address("127.0.0.1");

	auto server = Server{ioc, {host, port}, 3};
	server.register_method(std::make_unique<method::Echo>());
	server.set_verbose(true);
	server.start();

	auto reply_expected = 0;
	auto reply_cnt = 0;
	auto client = Client{ioc, false};

	for (const auto& item : args)
	{
		const auto& request = item.first;
		const auto& expected = item.second;
		std::cout << "request=" << request << " => expected=" << expected << std::endl;
		++reply_expected;
	client.async_call(request, [&](const std::string& res) {
		ASSERT_EQ(expected, res);
		if (reply_expected == ++reply_cnt) {ioc.stop();}
	});
	}

	ioc.run_for(boost::asio::chrono::seconds(1));
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


}