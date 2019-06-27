#include <webrpc/AbstractMethod.h>
#include <webrpc/Client.h>
#include <webrpc/detail/HttpWorker.h>
#include <webrpc/Registry.h>

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

void test_async_call(const std::string& request, const std::string& expected)
{
	auto ioc = boost::asio::io_context{};

	const unsigned short port = 8080;
	const boost::asio::ip::address host = boost::asio::ip::make_address("127.0.0.1");

	auto registry = Registry{};
	auto method = std::make_unique<detail::Echo>();
	registry.emplace(std::make_pair(method->get_name(), std::move(method)));

	auto acceptor = boost::asio::ip::tcp::acceptor{ioc, {host, port}};
	auto worker = detail::HttpWorker{acceptor, registry};
	worker.start();

	auto reply_cnt = 0;
	auto client = Client{ioc, false};
	client.async_call(request, [&](const std::string& res) {
		ASSERT_EQ(expected, res);
		++reply_cnt;
		ioc.stop();
	});

	ioc.run_for(boost::asio::chrono::seconds(3));
	ASSERT_EQ(1, reply_cnt);
}

TEST(HttpWorker, AsyncCallValid)
{
	test_async_call("http://127.0.0.1:8080/Echo/Echo", "\"Echo\"");
}

TEST(HttpWorker, AsyncCallInvalid)
{
	test_async_call("http://127.0.0.1:8080/favicon.ico", "Invalid webrpc request 'favicon.ico'\r");
}