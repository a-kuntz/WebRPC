#include <webrpc/Server.h>
#include <webrpc/Method.h>

#include <boost/asio.hpp>

#include <memory>
#include <iostream>
#include <algorithm>

class Echo : public Method
{
	public:
	Echo() : Method("Echo") {};

	std::string execute(const std::string& args) override
	{
		return args;
	}
};

class Revert : public Method
{
	public:
	Revert() : Method("Revert") {};

	std::string execute(const std::string& args) override
	{
		std::string result(args);
		std::reverse(std::begin(result),std::end(result));
		return result;
	}
};

int main()
{
	try
	{
		const boost::asio::ip::address host(boost::asio::ip::make_address("127.0.0.1"));
		const unsigned short port(80);

		Server server({host, port});
		server.register_method(std::make_unique<Echo>());
		server.register_method(std::make_unique<Revert>());
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
