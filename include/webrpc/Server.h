#ifndef __SERVER__
#define __SERVER__

#include <boost/asio.hpp>

class Server
{
	public:
	Server() = delete;

	Server(const boost::asio::ip::tcp::endpoint endpoint, int num_workers=10)
	:	_num_workers{num_workers},
		_endpoint{endpoint}
	{};

	void run();

	private:
	const int _num_workers;
	const boost::asio::ip::tcp::endpoint _endpoint;
};

#endif