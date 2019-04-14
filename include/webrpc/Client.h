#ifndef CLIENT_H
#define CLIENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

// Performs an HTTP GET and prints the response
class Client : public std::enable_shared_from_this<Client>
{
public:
	// Resolver and socket require an io_context
	explicit Client(boost::asio::io_context& ioc);

	void call(const std::string& uri_string);

	void set_verbose(bool verbose);

private:
	void on_resolve(boost::system::error_code ec, tcp::resolver::results_type results);

	void on_connect(boost::system::error_code ec);

	void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

	void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

private:
	tcp::resolver _resolver;
	tcp::socket _socket;
	boost::beast::flat_buffer _buffer;
	http::request<http::empty_body> _request;
	http::response<http::string_body> _response;
	bool _verbose;
};

#endif // CLIENT_H
