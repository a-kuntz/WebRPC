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

TEST(ClientServer, Async)
{
	auto ioc = boost::asio::io_context{};

	const unsigned short port = 8080;
	const boost::asio::ip::address host = boost::asio::ip::make_address("127.0.0.1");

	auto server = Server{ioc, {host, port}, 3};
	server.register_method(std::make_unique<detail::Echo>());
	server.set_verbose(true);
	server.start();

	const auto reply_expected = 4;
	auto reply_cnt = 0;
	auto client = Client{ioc, false};
	client.async_call("http://127.0.0.1:8080/system.list_methods", [&](const std::string& res) {
		ASSERT_EQ("[\"Echo\",\"system.list_methods\"]", res);
		if (reply_expected == ++reply_cnt) {ioc.stop();}
	});

	client.async_call("http://localhost:8080/Echo/[{foo:bar},[a,b,c,1,2,3,true,false],<11,12,ab>]", [&](const std::string& res) {
		ASSERT_EQ("[{foo:\"bar\"},[\"a\",\"b\",\"c\",1,2,3,true,false],<0x11,0x12,0xAB>]", res);
		if (reply_expected == ++reply_cnt) {ioc.stop();}
	});

	client.async_call("http://127.0.0.1:8080/Echo/{syntax-error}", [&](const std::string& res) {
		ASSERT_EQ("null_t", res);
		if (reply_expected == ++reply_cnt) {ioc.stop();}
	});

	client.async_call("http://127.0.0.1:8080/Echo/%7Bkey:val%7D", [&](const std::string& res) {
		ASSERT_EQ("{key:\"val\"}", res);
		if (reply_expected == ++reply_cnt) {ioc.stop();}
	});

	ioc.run_for(boost::asio::chrono::seconds(3));
	ASSERT_EQ(reply_expected, reply_cnt);
}
