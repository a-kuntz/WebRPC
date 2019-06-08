#include <webrpc/Client.h>
#include <webrpc/Version.h>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace po = boost::program_options;

class IocThreadGuard
{
	public:
	IocThreadGuard(boost::asio::io_context& ioc)
		: _work(boost::asio::make_work_guard(ioc))
		, _thread([&]() { ioc.run(); })
	{}

	~IocThreadGuard()
	{
		_thread.join();
	}

	void reset()
	{
		_work.reset();
	}

	private:
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work;
	std::thread																 _thread;
};

int main(int argc, char** argv)
{
	std::string uri;

	po::options_description desc("Options");
	desc.add_options()
		("uri,u", po::value<std::string>(&uri)->value_name("URI"), "WebRPC uri")
		("verbose,v", "verbose")
		("help,h", "print help message and exit");

	po::positional_options_description pos_desc;
	pos_desc.add("uri", -1);

	// parse and compare for required options
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << "sampleclient [options] URI\n"
				  << "\n"
				  << desc << "\n"
				  << "URI example:\n"
				  << "    http://localhost:8080/system.list_methods\n"
				  << "\n"
				  << "WebRPC Version: " << WEBRPC_VERSION_STRING << std::endl;
		return EXIT_SUCCESS;
	}

	bool verbose = vm.count("verbose") > 0;

	try
	{
		// create and start io context
		boost::asio::io_context ioc;
		Client					c{ioc, verbose};

		// async_call(.)
		//		if (false)
		{
			c.async_call(uri, [](const std::string& res) { std::cout << "--" << res << "--\n"; });
			// c.async_call(uri, [](const std::string& res) { std::cout << "--" << res << "--\n"; });
			ioc.run();
		}

		if (false)
		{
			c.async_call(uri, [](const std::string& res) { std::cout << "--" << res << "--\n"; });
			// c.async_call(uri, [](const std::string& res) { std::cout << "--" << res << "--\n"; });
			std::thread t([&]() { ioc.run(); });
			t.join();
		}

		if (false)
		{
			auto		guard = boost::asio::make_work_guard(ioc);
			std::thread t([&]() { ioc.run(); });
			c.async_call(uri, [&](const std::string& res) {
				std::cout << "--" << res << "--\n";
				guard.reset();
			});
			// c.async_call(uri, [&](const std::string& res) {
			// 	std::cout << "--" << res << "--\n";
			// 	guard.reset();
			// });
			t.join();
		}

		if (false)
		{
			auto guard = IocThreadGuard{ioc};
			c.async_call(uri, [&](const std::string& res) {
				std::cout << "--" << res << "--\n";
				guard.reset();
			});
			// c.async_call(uri, [&](const std::string& res) {
			// 	std::cout << "--" << res << "--\n";
			// 	guard.reset();
			// });
		}

		// call(.)
		if (false)
		{
			std::cout << "==" << c.call(uri) << "==\n";
			// std::cout << "==" << c.call(uri) << "==\n";
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
