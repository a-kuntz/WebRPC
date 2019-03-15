#ifndef __SERVER__
#define __SERVER__

#include <webrpc/Method.h>

#include <boost/asio.hpp>

#include <map>
#include <memory>

using Registry = std::map<std::string, std::unique_ptr<Method>>;

class Server
{
	public:
	Server() = delete;

	Server(const boost::asio::ip::tcp::endpoint endpoint, int num_workers=10)
	:	_num_workers{num_workers},
		_endpoint{endpoint}
	{};

	void register_method(std::unique_ptr<Method>&& method);

	void run();

	private:
	const int _num_workers;
	const boost::asio::ip::tcp::endpoint _endpoint;
	Registry _registry;
};

#endif