#include <webrpc/Server.h>
#include <webrpc/method/SystemListMethods.h>

Server::Server(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint endpoint, int num_workers)
	: _acceptor{ioc, endpoint}
	, _num_workers{num_workers}
{
	register_method(std::make_unique<method::SystemListMethods>(_registry));
}

void Server::register_method(IMethodUP&& method)
{
	_registry.emplace(std::make_pair(method->get_name(), std::move(method)));
}

void Server::start()
{
	// todo: make HttpWorker : public std::enable_shared_from_this<HttpWorker>
	// and create workers via std::make_shared<detail::HttpWorker>
	for (int i = 0; i < _num_workers; ++i)
	{
		_workers.emplace_back(_acceptor, _registry);
		_workers.back().set_verbose(_verbose);
		_workers.back().start();
	}
}

void Server::set_verbose(bool verbose)
{
	_verbose = verbose;
}
