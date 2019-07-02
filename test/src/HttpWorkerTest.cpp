#include <webrpctest/Testcase.h>
#include <webrpctest/method/Echo.h>

#include <webrpc/AbstractMethod.h>
#include <webrpc/Client.h>
#include <webrpc/detail/HttpWorker.h>
#include <webrpc/Registry.h>
#include <webrpc/method/SystemListMethods.h>

#include <gtest/gtest.h>

namespace webrpctest
{

void test_async_call(const Testcases& args)
{
	auto ioc = boost::asio::io_context{};

	const unsigned short port = 8080;
	const boost::asio::ip::address host = boost::asio::ip::make_address("127.0.0.1");

	auto registry = Registry{};
	{
		auto method = std::make_unique<method::Echo>();
		registry.emplace(std::make_pair(method->get_name(), std::move(method)));
	}
	{
		auto method = std::make_unique<::method::SystemListMethods>(registry);
		registry.emplace(std::make_pair(method->get_name(), std::move(method)));
	}

	auto acceptor = boost::asio::ip::tcp::acceptor{ioc, {host, port}};
	auto worker = detail::HttpWorker{acceptor, registry};
	worker.start();

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

TEST(HttpWorker, AsyncSingleCalls)
{
	for (const auto& testcase : testcases)
	{
		test_async_call({testcase});
	}
}

TEST(HttpWorker, AsyncMultipleCalls)
{
	test_async_call(testcases);
}

}