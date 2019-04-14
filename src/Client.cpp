#include <webrpc/Client.h>

#include <webrpc/Uri.h>
#include <webrpc/Parser.h>

#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>

#include <functional>
#include <iostream>
#include <string>

// Report a failure
void fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

// Resolver and socket require an io_context
Client::Client(boost::asio::io_context& ioc)
	: _resolver(ioc)
	, _socket(ioc)
	, _verbose(false)
{}

void Client::call(const std::string& uri_string)
{
	const auto uri = parse_uri(uri_string);

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
			&Client::on_resolve,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Client::set_verbose(bool verbose)
{
	_verbose = verbose;
}

void Client::on_resolve(boost::system::error_code ec, tcp::resolver::results_type results)
{
	if(ec)
		return fail(ec, "resolve");

	// Make the connection on the IP address we get from a lookup
	boost::asio::async_connect(
		_socket,
		results.begin(),
		results.end(),
		std::bind(
			&Client::on_connect,
			shared_from_this(),
			std::placeholders::_1));
}

void Client::on_connect(boost::system::error_code ec)
{
	if(ec)
		return fail(ec, "connect");

	if (_verbose)
	{
		// Write the request to standard out
		std::cout << _request;
	}

	// Send the HTTP request to the remote host
	http::async_write(_socket, _request,
		std::bind(
			&Client::on_write,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Client::on_write(boost::system::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if(ec)
		return fail(ec, "write");

	// Receive the HTTP response
	http::async_read(_socket, _buffer, _response,
		std::bind(
			&Client::on_read,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Client::on_read(boost::system::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if(ec)
		return fail(ec, "read");

	if (_verbose)
	{
		std::cout << _response.base();
	}

	// Write the message to standard out
	std::cout << _response.body() << std::endl;

	// Gracefully close the socket
	_socket.shutdown(tcp::socket::shutdown_both, ec);

	// not_connected happens sometimes so don't bother reporting it.
	if(ec && ec != boost::system::errc::not_connected)
		return fail(ec, "shutdown");

	// If we get here then the connection is closed gracefully
}

