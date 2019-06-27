#include <webrpc/Client.h>
#include <webrpc/Version.h>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
	std::vector<std::string> uris;
	auto desc = po::options_description{"Options"};
	desc.add_options()
		("async,a", "make asynchronous calls")
		("uri,u", po::value(&uris), "WebRPC uri")
		("verbose,v", "make verbose output")
		("help,h", "print help message and exit");

	auto pos_desc = po::positional_options_description{};
	pos_desc.add("uri", -1);

	// parse and compare for required options
	auto vm = po::variables_map{};
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << "sampleclient [options] URI\n"
				  << "\n"
				  << desc << "\n"
				  << "URI example:\n"
				  << "    http://localhost:8080/?\n"
				  << "\n"
				  << "WebRPC Version: " << WEBRPC_VERSION_STRING << std::endl;
		return EXIT_SUCCESS;
	}

	const auto verbose = vm.count("verbose") > 0;

	try
	{
		// create and start io context
		auto ioc = boost::asio::io_context{};
		auto c = Client{ioc, verbose};

		if (vm.count("async"))
		{ // make asynchronous call(s)
			auto guard = boost::asio::make_work_guard(ioc);

			std::thread t([&]() { ioc.run(); });

			auto idx = 0;
			for (const auto& uri: uris)
			{
				std::cout <<idx << "< " << uri << std::endl;
				c.async_call(uri, [&, idx=idx](const std::string& res) {
					std::cout << idx << "> " << res << "\n";
					guard.reset();
				});
				++idx;
			}
			t.join();
		}
		else
		{ // make synchronous call(s)

			auto idx = 0;
			for (const auto& uri: uris)
			{
				std::cout << idx << "< "<< uri << std::endl;
				std::cout << idx << "> "<< c.call(uri) << std::endl;
				++idx;
			}
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
