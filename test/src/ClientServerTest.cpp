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

TEST(ClientServer, Echo)
{
	auto ioc = boost::asio::io_context{};

	const unsigned short port = 8080;
	const boost::asio::ip::address host = boost::asio::ip::make_address("127.0.0.1");

	auto server = Server{ioc, {host, port}, 1};
	server.register_method(std::make_unique<detail::Echo>());
	server.start();

	auto received_reply = false;
	auto client = Client{ioc, false};
	client.async_call("http://127.0.0.1:8080/system.list_methods", [&](const std::string& res) {
		received_reply = true;
		// std::cout << res << std::endl;
		ASSERT_EQ("[\"Echo\",\"system.list_methods\"]", res);
		ioc.stop();
	});

	client.async_call("http://127.0.0.1:8080/Echo/Echo", [&](const std::string& res) {
		received_reply = true;
		ASSERT_EQ("\"Echo\"", res);
		ioc.stop();
	});

	ioc.run_for(boost::asio::chrono::seconds(3));
	ASSERT_TRUE(received_reply);
}
