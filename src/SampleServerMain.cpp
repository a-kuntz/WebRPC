#include <webrpc/Server.h>

#include <boost/asio.hpp>

#include <iostream>

int main()
{
	try
	{
		const boost::asio::ip::address host(boost::asio::ip::make_address("127.0.0.1"));
		const unsigned short port(80);

		Server server({host, port});
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
