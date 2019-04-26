#ifndef CLIENT_H
#define CLIENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class Client
{
public:
	Client(bool verbose);

	// todo: add lambda or string return value
	void async_call(const std::string& uri);

	// todo: implement
	//std::string call(const std::string& uri);

private:
	const bool _verbose;
};

#endif // CLIENT_H
