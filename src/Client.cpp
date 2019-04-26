#include <webrpc/Client.h>

#include <webrpc/Uri.h>
#include <webrpc/Parser.h>

#include <boost/asio/connect.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/version.hpp>

#include <functional>
#include <iostream>
#include <string>

// Report a failure
void fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

namespace detail
{
// Performs an HTTP GET and prints the response
class Session : public std::enable_shared_from_this<Session>
{
public:
	// Resolver and socket require an io_context
	explicit Session(boost::asio::io_context& ioc);

	// todo: add lambda or string return value
	void async_call(const std::string& uri_string);

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

// Resolver and socket require an io_context
Session::Session(boost::asio::io_context& ioc)
	: _resolver(ioc)
	, _socket(ioc)
	, _verbose(false)
{}

void Session::async_call(const std::string& uri_string)
{
	const auto uri = Parser::parse_uri(uri_string);

	// Set up an HTTP GET request message
	_request.version(10);
	_request.method(http::verb::get);
	_request.target(uri->target);
	_request.set(http::field::host, uri->host);
	_request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	// Look up the domain name
	_resolver.async_resolve(
		uri->host,
		uri->port,
		std::bind(
			&Session::on_resolve,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Session::set_verbose(bool verbose)
{
	_verbose = verbose;
}

void Session::on_resolve(boost::system::error_code ec, tcp::resolver::results_type results)
{
	if (ec)
		return fail(ec, "resolve");

	// Make the connection on the IP address we get from a lookup
	boost::asio::async_connect(
		_socket,
		results.begin(),
		results.end(),
		std::bind(
			&Session::on_connect,
			shared_from_this(),
			std::placeholders::_1));
}

void Session::on_connect(boost::system::error_code ec)
{
	if (ec)
		return fail(ec, "connect");

	if (_verbose)
	{
		// Write the request to standard out
		std::cout << _request;
	}

	// Send the HTTP request to the remote host
	http::async_write(_socket, _request,
		std::bind(
			&Session::on_write,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Session::on_write(boost::system::error_code ec, std::size_t /*bytes_transferred*/)
{
	if (ec)
		return fail(ec, "write");

	// Receive the HTTP response
	http::async_read(_socket, _buffer, _response,
		std::bind(
			&Session::on_read,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Session::on_read(boost::system::error_code ec, std::size_t /*bytes_transferred*/)
{
	if (ec)
		return fail(ec, "read");

	if (_verbose)
	{
		std::cout << _response.base();
	}

	// Write the message to standard out
	std::cout << _response.body(); // body ends with newline

	// Gracefully close the socket
	_socket.shutdown(tcp::socket::shutdown_both, ec);

	// not_connected happens sometimes so don't bother reporting it.
	if (ec && ec != boost::system::errc::not_connected)
		return fail(ec, "shutdown");

	// If we get here then the connection is closed gracefully
}
} // ns detail

Client::Client(bool verbose)
	: _verbose(verbose)
{}

void Client::async_call(const std::string& uri)
{
	// Launch the asynchronous operation
	boost::asio::io_context ioc;

	auto session = std::make_shared<detail::Session>(ioc);
	session->set_verbose(_verbose);
	session->async_call(uri);
//	session->call(uri);
//	session->call(uri);

	ioc.run();
}

