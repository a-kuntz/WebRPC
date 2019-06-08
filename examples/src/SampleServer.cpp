#include <webrpc/AbstractMethod.h>
#include <webrpc/Parser.h> // to parse reversed value
#include <webrpc/Server.h>
#include <webrpc/Version.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>

namespace po = boost::program_options;

namespace method
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

struct Revert : public AbstractMethod
{
	Revert()
		: AbstractMethod("Revert")
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		std::string result(arg.value().as<string_t>());
		std::reverse(std::begin(result), std::end(result));
		return value_t(result);
	}
};

struct Shuffle : public AbstractMethod
{
	Shuffle()
		: AbstractMethod("Shuffle")
		, _rng{static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())}
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		std::string result(arg.value().as<string_t>());

		std::shuffle(result.begin(), result.end(), _rng);
		return value_t(result);
	}

	std::default_random_engine _rng;
};

struct Sum : public AbstractMethod
{
	Sum()
		: AbstractMethod("Sum")
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		if (!arg || arg.value().type() != value_t::type_info::array_type)
		{
			throw std::runtime_error("invalid argument: expected array of double or int like [1,2,3,4e-5,6.7]");
		}

		const auto a   = arg.value().as<array_t>();
		double	 sum = 0;
		for (const auto& item : a)
		{
			if (item.type() == value_t::type_info::int_type)
			{
				sum += item.as<int_t>();
			}
			else if (item.type() == value_t::type_info::double_type)
			{
				sum += item.as<double_t>();
			}
		}

		return value_t{sum};
	}
};

struct DateTime : public AbstractMethod
{
	DateTime()
		: AbstractMethod("DateTime")
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> /*arg*/) override
	{
		const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream date_time;
		date_time << std::put_time(std::localtime(&now), "%F %T");
		return value_t(date_time.str());
	}
};

struct SomeStruct
{
	SomeStruct() = default;
	SomeStruct(const boost::optional<value_t> arg)
	{
		assert(arg.value().type() == value_t::type_info::struct_type);
		const auto s = arg.value().as<struct_t>();
		_double		 = s.at("double").as<double_t>();
		_int		 = s.at("int").as<int_t>();
		_string		 = s.at("string").as<string_t>();
	}
	explicit operator value_t() const
	{
		struct_t result;
		result["double"] = double_t(_double);
		result["int"]	= int_t(_int);
		result["string"] = string_t(_string);
		return result;
	}
	double		_double = 0.0;
	int			_int	= 0;
	std::string _string;
};

struct GetSetValue : public AbstractMethod
{
	GetSetValue(SomeStruct& val)
		: AbstractMethod("GetSetValue")
		, _val(val)
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		_val = arg && arg.value().type() == value_t::type_info::struct_type ? SomeStruct(arg) : _val;
		return static_cast<value_t>(_val);
	}

	SomeStruct& _val;
};

} // namespace method

int main(int argc, char** argv)
{
	std::string    host_name;
	unsigned short port;

	po::options_description desc("Options");
	desc.add_options()
		("host,h", po::value<std::string>(&host_name)->value_name("HOST")->default_value("127.0.0.1"), "host name or ip address to bind to")
		("port,p", po::value<unsigned short>(&port)->value_name("PORT")->default_value(8080), "port number")
		("verbose,v", "verbose")
		("help", "print help message and exit");

	// parse and compare for required options
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << "sampleserver [options]\n"
				  << "\n"
				  << desc << "\n"
				  << "WebRPC Version: " << WEBRPC_VERSION_STRING << std::endl;
		return EXIT_SUCCESS;
	}

	bool verbose = vm.count("verbose") >0;

	try
	{
		const boost::asio::ip::address host = boost::asio::ip::make_address(host_name);

		std::cout << "SampleServer listening on " << host << " " << port << std::endl;
		std::cout << " - http://" << host << ":" << port << "/system.list_methods" << std::endl;

		method::SomeStruct globalValue;

		Server server({host, port});
		server.set_verbose(verbose);
		server.register_method(std::make_unique<method::Echo>());
		server.register_method(std::make_unique<method::Revert>());
		server.register_method(std::make_unique<method::Shuffle>());
		server.register_method(std::make_unique<method::Sum>());
		server.register_method(std::make_unique<method::DateTime>());
		server.register_method(std::make_unique<method::GetSetValue>(globalValue));
		server.run();

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
