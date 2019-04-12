//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP client, asynchronous
//
//------------------------------------------------------------------------------

#include <webrpc/Version.h>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace po = boost::program_options;

//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

// Performs an HTTP GET and prints the response
class session : public std::enable_shared_from_this<session>
{
	tcp::resolver _resolver;
	tcp::socket _socket;
	boost::beast::flat_buffer _buffer; // (Must persist between reads)
	http::request<http::empty_body> _request;
	http::response<http::string_body> _response;

public:
	// Resolver and socket require an io_context
	explicit
	session(boost::asio::io_context& ioc)
		: _resolver(ioc)
		, _socket(ioc)
	{
	}

	// Start the asynchronous operation
	void
	run(
		const std::string& host,
		const std::string& port,
		const std::string& target)
	{
		// Set up an HTTP GET request message
		_request.version(10);
		_request.method(http::verb::get);
		_request.target(target);
		_request.set(http::field::host, host);
		_request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		// Look up the domain name
		_resolver.async_resolve(
			host,
			port,
			std::bind(
				&session::on_resolve,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
	on_resolve(
		boost::system::error_code ec,
		tcp::resolver::results_type results)
	{
		if(ec)
			return fail(ec, "resolve");

		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(
			_socket,
			results.begin(),
			results.end(),
			std::bind(
				&session::on_connect,
				shared_from_this(),
				std::placeholders::_1));
	}

	void
	on_connect(boost::system::error_code ec)
	{
		if(ec)
			return fail(ec, "connect");

		// Write the request to standard out
		std::cout << _request << std::endl;

		// Send the HTTP request to the remote host
		http::async_write(_socket, _request,
			std::bind(
				&session::on_write,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
	on_write(
		boost::system::error_code ec,
		std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if(ec)
			return fail(ec, "write");

		// Receive the HTTP response
		http::async_read(_socket, _buffer, _response,
			std::bind(
				&session::on_read,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
	on_read(
		boost::system::error_code ec,
		std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if(ec)
			return fail(ec, "read");

		// Write the message to standard out
		std::cout << _response << std::endl;

		// Gracefully close the socket
		_socket.shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes so don't bother reporting it.
		if(ec && ec != boost::system::errc::not_connected)
			return fail(ec, "shutdown");

		// If we get here then the connection is closed gracefully
	}
};

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	std::string host;
	std::string port;
	std::string request;

	po::options_description desc("Options");
	desc.add_options()
		("ip,i",      po::value<std::string>(&host)->value_name("HOST"), "host address")
		("port,p",    po::value<std::string>(&port)->value_name("PORT")->default_value("8080")->implicit_value("8080"), "port number")
		("request,r", po::value<std::string>(&request)->value_name("REQUEST"), "request")
		("help,h", "print help message and exit");

	// parse and compare for required options
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout
			<< "sampleclient [options]\n"
			<< "\n"
			<< desc << "\n"
			<< "Example\n"
//			<< "    http://localhost:8080/system.list_methods\n"
			<< "    -i localhost -r system.list_methods\n"
			<< "\n"
			<< "WebRPC Version: " << WEBRPC_VERSION_MAJOR << "." << WEBRPC_VERSION_MINOR << "." << WEBRPC_VERSION_PATCH
			<< " (" << GIT_BRANCH << " @ " << GIT_COMMIT_HASH << " " << (GIT_WORKING_COPY_MODIFIED ? "+" : "") << ")"
			<< std::endl;
		return EXIT_SUCCESS;
	}

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	// Launch the asynchronous operation
	std::make_shared<session>(ioc)->run(host, port, request);

	// Run the I/O service. The call will return when
	// the get operation is complete.
	ioc.run();

	return EXIT_SUCCESS;
}
