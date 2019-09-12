#include <webrpctest/Testcase.h>
#include <webrpctest/method/Echo.h>

#include <webrpc/AbstractMethod.h>
#include <webrpc/Client.h>
#include <webrpc/Server.h>
#include <webrpc/Util.h>

#include <ApprovalTests.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <iterator>

namespace webrpctest
{
namespace approval
{

class ClientServerFixture : public ::testing::Test
{
public:
	ClientServerFixture()
	{
		_server.register_method(std::make_unique<method::Echo>());
		_server.start();
	}
protected:
	boost::asio::io_context _ioc{};
	const unsigned short _port = 8888;
	const std::string _hostname = "127.0.0.1";
	const boost::asio::ip::address _host = boost::asio::ip::make_address(_hostname);

	Server _server{_ioc, {_host, _port}, 3};
	Client _client{_ioc, false};
};

TEST_F(ClientServerFixture, Echo)
{
	std::vector<std::string> observations;
	const std::vector<std::string> args = {"some string","[{foo:bar},[a,b,c,1,2,3,true,false],<11,12,ab>]","%7Bkey:val%7D"};

	for (const auto& arg : args)
	{
		const auto request = std::string{"http://"} + _hostname + ":" + std::to_string(_port) + "/Echo/" + arg;
		_client.async_call(request, [&](const std::string observed) {
			std::cout << observed << std::endl;
			observations.push_back(observed);
			if (args.size() == observations.size()) {_ioc.stop();}
		});
	}

	_ioc.run_for(boost::asio::chrono::seconds(1));
	Approvals::verify(observations);
}

}
}