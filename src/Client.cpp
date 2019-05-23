#include <webrpc/Client.h>

#include <webrpc/Parser.h>
#include <webrpc/Uri.h>

#include <boost/asio/connect.hpp>
#include <boost/beast/version.hpp>

#include <functional>
#include <future>
#include <iostream>
#include <string>

// Report a failure
void fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

namespace detail
{
class Session : public std::enable_shared_from_this<Session>
{
  public:
	// Resolver and socket require an io_context
	explicit Session(boost::asio::io_context& ioc, bool verbose);

	void async_call(const std::string& uri_string, Client::Completion completion);

  private:
	void on_resolve(boost::system::error_code ec, tcp::resolver::results_type results);

	void on_connect(boost::system::error_code ec);

	void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

	void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

  private:
	tcp::resolver					  _resolver;
	tcp::socket						  _socket;
	boost::beast::flat_buffer		  _buffer;
	http::request<http::empty_body>   _request;
	http::response<http::string_body> _response;
	bool							  _verbose;
	Client::Completion				  _completion = nullptr;
};

Session::Session(boost::asio::io_context& ioc, bool verbose)
	: _resolver(ioc)
	, _socket(ioc)
	, _verbose(verbose)
{}

void Session::async_call(const std::string& uri_string, Client::Completion completion)
{
	_completion = completion;

	const auto uri = Parser::parse_uri(uri_string);

	// Set up an HTTP GET request message
	_request.version(10);
	_request.method(http::verb::get);
	_request.target(uri->target);
	_request.set(http::field::host, uri->host);
	_request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	// Look up the domain name
	_resolver.async_resolve(uri->host, uri->port,
		std::bind(&Session::on_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::on_resolve(boost::system::error_code ec, tcp::resolver::results_type results)
{
	if (ec)
		return fail(ec, "resolve");

	// Make the connection on the IP address we get from a lookup
	boost::asio::async_connect(_socket, results.begin(), results.end(),
		std::bind(&Session::on_connect, shared_from_this(), std::placeholders::_1));
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
		std::bind(&Session::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::on_write(boost::system::error_code ec, std::size_t /*bytes_transferred*/)
{
	if (ec)
		return fail(ec, "write");

	// Receive the HTTP response
	http::async_read(_socket, _buffer, _response,
		std::bind(&Session::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::on_read(boost::system::error_code ec, std::size_t /*bytes_transferred*/)
{
	if (ec)
		return fail(ec, "read");

	if (_verbose)
	{
		std::cout << _response;
	}

	// remove trailing newline
	const std::string result = [&]() {
		auto s = _response.body();
		s.pop_back();
		return s;
	}();
	_completion(result);

	// Gracefully close the socket
	_socket.shutdown(tcp::socket::shutdown_both, ec);

	// not_connected happens sometimes so don't bother reporting it.
	if (ec && ec != boost::system::errc::not_connected)
		return fail(ec, "shutdown");

	// If we get here then the connection is closed gracefully
}
} // namespace detail

Client::Client(boost::asio::io_context& ioc, bool verbose)
	: _ioc(ioc)
	, _verbose(verbose)
{}

void Client::async_call(const std::string& uri, Completion completion)
{
	std::make_shared<detail::Session>(_ioc, _verbose)->async_call(uri, completion);
}

std::string Client::call(const std::string& uri)
{
	std::promise<std::string> promise;
	auto					  future = promise.get_future();
	std::make_shared<detail::Session>(_ioc, _verbose)->async_call(uri, [&](const std::string& res) {
		promise.set_value(res);
	});
	std::thread t([&]() { _ioc.run(); });
	future.wait();
	t.join();
	_ioc.reset();
	return future.get();
}
