#ifndef __SERVER__
#define __SERVER__

#include <webrpc/AbstractMethod.h>
#include <webrpc/Value.h>
#include <webrpc/Registry.h>

#include <boost/asio.hpp>

#include <map>

class Server
{
public:
	Server() = delete;

	Server(const boost::asio::ip::tcp::endpoint endpoint, int num_workers=10);

	void register_method(IMethodUP&& method);

	void run();

private:
	const int _num_workers;
	const boost::asio::ip::tcp::endpoint _endpoint;
	Registry _registry;
};

#endif