#include <webrpc/AbstractMethod.h>
#include <webrpc/Parser.h>	// to parse reversed value
#include <webrpc/Server.h>
#include <webrpc/Version.h>

#include <boost/asio.hpp>

#include <memory>
#include <iostream>
#include <algorithm>

class Echo : public AbstractMethod
{
	public:
	Echo() : AbstractMethod("Echo") {};

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		return arg;
	}
};

class Revert : public AbstractMethod
{
	public:
	Revert() : AbstractMethod("Revert") {};

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		std::string result(arg.value().to_string());
		std::reverse(std::begin(result),std::end(result));
		return parse_value(result);
	}
};

class Add : public AbstractMethod
{
	public:
	Add() : AbstractMethod("Add") {};

	boost::optional<value_t> execute(const boost::optional<value_t> /*arg*/) override
	{
		return value_t{42.0};
	}
};

int main()
{
	std::cout << "Version: " << WEBRPC_VERSION_MAJOR << "." << WEBRPC_VERSION_MINOR << "." << WEBRPC_VERSION_PATCH << std::endl;

	try
	{
		const boost::asio::ip::address host(boost::asio::ip::make_address("127.0.0.1"));
		const unsigned short port(80);

		Server server({host, port});
		server.register_method(std::make_unique<Echo>());
		server.register_method(std::make_unique<Revert>());
		server.register_method(std::make_unique<Add>());
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
