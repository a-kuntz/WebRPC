#ifndef __SERVER__
#define __SERVER__

#include <webrpc/AbstractMethod.h>
#include <webrpc/Value.h>

#include <boost/asio.hpp>

#include <map>

using Registry = std::map<std::string, IMethodUP>;

class Server
{
	public:
	Server() = delete;

	// todo: add introspection method: system.list_methods
	Server(const boost::asio::ip::tcp::endpoint endpoint, int num_workers=10)
	:	_num_workers{num_workers},
		_endpoint{endpoint}
	{};

	void register_method(IMethodUP&& method);

	void run();

	private:
	const int _num_workers;
	const boost::asio::ip::tcp::endpoint _endpoint;
	Registry _registry;
};

#endif