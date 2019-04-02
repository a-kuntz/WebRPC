#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

class Client
{
public:
	Client(const std::string& host, const unsigned port, const std::string& request)
		: _host{host}, _port{std::to_string(port)}, _request{request}
	{}

	void read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
	{
		if (!ec)
		{
			std::cout.write(_buffer.data(), bytes_transferred);
			_tcp_socket.async_read_some(buffer(_buffer), [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
			{
				read_handler(ec, bytes_transferred);
			});
		}
	}

	void connect_handler(const boost::system::error_code &ec)
	{
		if (!ec)
		{
			// todo: create HTTP GET request
			std::string http_request = "GET " + _request + " HTTP/1.1\r\nHost: " + _host + "\r\n\r\n";
			write(_tcp_socket, buffer(http_request));
			_tcp_socket.async_read_some(buffer(_buffer), [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
			{
				read_handler(ec, bytes_transferred);
			});
		}
	}

	void resolve_handler(const boost::system::error_code &ec, tcp::resolver::iterator it)
	{
		if (!ec)
		{
			_tcp_socket.async_connect(*it, [this](const boost::system::error_code &ec)
			{
				connect_handler(ec);
			});
		}
	}

	void run()
	{
		tcp::resolver resolver{_ioservice};
		tcp::resolver::query query{_host, _port};
		resolver.async_resolve(query, [this](const boost::system::error_code &ec, tcp::resolver::iterator it)
		{
			resolve_handler(ec,it);
		});
		_ioservice.run();
	}

private:
	io_service _ioservice;
	tcp::socket _tcp_socket{_ioservice};
	std::array<char, 4096> _buffer;
	const std::string _host;
	const std::string _port;
	const std::string _request;
};

int main()
{
	// todo: read host, port and request from commandline
	Client client{"localhost", 8080, "system.list_methods"};
	client.run();
}
