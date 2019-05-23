#ifndef __SERVER__
#define __SERVER__

#include <webrpc/AbstractMethod.h>
#include <webrpc/Registry.h>
#include <webrpc/Value.h>

#include <boost/asio.hpp>

#include <map>

class Server
{
  public:
	Server() = delete;

	Server(const boost::asio::ip::tcp::endpoint endpoint, int num_workers = 10);

	void register_method(IMethodUP&& method);

	void run();

	void set_verbose(bool verbose);

  private:
	const boost::asio::ip::tcp::endpoint _endpoint;
	const int							 _num_workers;
	Registry							 _registry;
	bool								 _verbose = false;
};

#endif