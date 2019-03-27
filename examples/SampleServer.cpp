#include <webrpc/AbstractMethod.h>
#include <webrpc/Parser.h>	// to parse reversed value
#include <webrpc/Server.h>
#include <webrpc/Version.h>

#include <boost/asio.hpp>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

struct Echo : public AbstractMethod
{
	Echo() : AbstractMethod("Echo") {};

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		return arg;
	}
};

struct Revert : public AbstractMethod
{
	Revert() : AbstractMethod("Revert") {};

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		std::string result(arg.value().to_string());
		std::reverse(std::begin(result),std::end(result));
		return parse_value(result);
	}
};

struct Sum : public AbstractMethod
{
	Sum() : AbstractMethod("Sum") {};

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		if (!arg || arg.value().type() != value_t::type_info::array_type)
		{
			throw std::runtime_error("invalid argument: expected array of double or int like [1,2,3,4e-5,6.7]");
		}

		const auto a = boost::get<array_t>(arg.value());
		double sum = 0;
		for (const auto& item : a)
		{
			if (item.type() == value_t::type_info::int_type)
			{
				sum += boost::get<int_t>(item);
			}
			else if (item.type() == value_t::type_info::double_type)
			{
				sum += boost::get<double_t>(item);
			}
		}

		return value_t{sum};
	}
};

struct DateTime : public AbstractMethod
{
	DateTime() : AbstractMethod("DateTime") {};


	boost::optional<value_t> execute(const boost::optional<value_t> /*arg*/) override
	{
		const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream date_time;
		date_time << std::put_time(std::localtime(&now), "%F %T");
		return value_t(date_time.str());
	}
};

int main()
{
	std::cout << "WebRPC Version: " << WEBRPC_VERSION_MAJOR << "." << WEBRPC_VERSION_MINOR << "." << WEBRPC_VERSION_PATCH << std::endl;
	try
	{
		const boost::asio::ip::address host(boost::asio::ip::make_address("127.0.0.1"));
		const unsigned short port(8080);

		std::cout << "SampleServer listening on " << host << " " << port << std::endl;
		std::cout << " - http://" << host << ":" << port << "/system.list_methods" << std::endl;

		Server server({host, port});
		server.register_method(std::make_unique<Echo>());
		server.register_method(std::make_unique<Revert>());
		server.register_method(std::make_unique<Sum>());
		server.register_method(std::make_unique<DateTime>());
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
