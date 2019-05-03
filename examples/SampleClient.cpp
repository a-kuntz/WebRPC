#include <webrpc/Version.h>
#include <webrpc/Client.h>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace po = boost::program_options;

class ThreadGuard
{
public:
	ThreadGuard(boost::asio::io_context& ioc)
	: _ioc(ioc)
	, _work(boost::asio::make_work_guard(ioc))
	, _thread([&](){ioc.run();})
	{}

	~ThreadGuard()
	{
		_ioc.stop();
		_thread.join();
	}

private:
	boost::asio::io_context& _ioc;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work;
	std::thread _thread;
};


int main(int argc, char** argv)
{
	std::string uri;
	bool verbose = false;

	po::options_description desc("Options");
	desc.add_options()
		("uri,u",     po::value<std::string>(&uri)->value_name("URI"), "WebRPC uri")
		("verbose,v", po::value<bool>(&verbose)->implicit_value(true), "verbose")
		("help,h",    "print help message and exit");

	po::positional_options_description pos_desc;
	pos_desc.add("uri", -1);

	// parse and compare for required options
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout
			<< "sampleclient [options] URI\n"
			<< "\n"
			<< desc << "\n"
			<< "URI example:\n"
			<< "    http://localhost:8080/system.list_methods\n"
			<< "\n"
			<< "WebRPC Version: " << WEBRPC_VERSION_MAJOR << "." << WEBRPC_VERSION_MINOR << "." << WEBRPC_VERSION_PATCH
			<< " (" << GIT_BRANCH << " @ " << GIT_COMMIT_HASH << " " << (GIT_WORKING_COPY_MODIFIED ? "+" : "") << ")"
			<< std::endl;
		return EXIT_SUCCESS;
	}

	try {
		// create and start io context
		boost::asio::io_context ioc;
		Client c{ioc, verbose};

//		c.async_call(uri, [](const std::string& res){std::cout << "--" << res << "--\n";});
//		ioc.run();

		c.async_call(uri, [](const std::string& res){std::cout << "--" << res << "--\n";});
		std::thread t([&](){ioc.run();});
		t.join();

//		auto g = ThreadGuard{ioc};
//		std::cout << "==" << c.call(uri) << "==\n";

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
