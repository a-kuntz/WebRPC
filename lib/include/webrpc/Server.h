#ifndef __SERVER__
#define __SERVER__

#include <webrpc/detail/HttpWorker.h>
#include <webrpc/IMethod.h>
#include <webrpc/Registry.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <list>

class Server
{
	public:
	Server() = delete;

	Server(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint endpoint, int num_workers = 10);

	void register_method(IMethodUP&& method);

	void start();

	void set_verbose(bool verbose);

	private:
	boost::asio::ip::tcp::acceptor 	_acceptor;
	const int						_num_workers;
	Registry						_registry;
	bool							_verbose = false;
	std::list<detail::HttpWorker> 	_workers;

};

#endif