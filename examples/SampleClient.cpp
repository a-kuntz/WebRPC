#include <webrpc/Version.h>
#include <webrpc/Client.h>

#include <boost/asio/connect.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
	std::string uri;

	po::options_description desc("Options");
	desc.add_options()
		("uri,u",     po::value<std::string>(&uri)->value_name("URI"), "WebRPC uri")
		("help,h", "print help message and exit");

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
		// Launch the asynchronous operation
		boost::asio::io_context ioc;

		std::make_shared<Client>(ioc)->call(uri);

		ioc.run();

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
