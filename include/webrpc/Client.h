#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <functional>
#include <string>

using tcp	  = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;   // from <boost/beast/http.hpp>

class Client
{
	public:
	using Completion = std::function<void(const std::string&)>;

	Client(boost::asio::io_context& ioc, bool verbose);

	void async_call(const std::string& uri, Completion completion);

	std::string call(const std::string& uri);

	private:
	boost::asio::io_context& _ioc;
	const bool				 _verbose;
};

#endif // CLIENT_H
