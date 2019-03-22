#include <webrpc/detail/fields_alloc.h>
#include <webrpc/Parser.h>
#include <webrpc/Server.h>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/filesystem.hpp>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace ip = boost::asio::ip;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

std::vector<boost::string_view> split_string_view(boost::string_view strv, boost::string_view delims = "/")
{
    std::vector<boost::string_view> output;
    size_t first = 0;

    while (first < strv.size())
    {
        const auto second = strv.find_first_of(delims, first);

        if (first != second)
            output.emplace_back(strv.substr(first, second-first));

        if (second == boost::string_view::npos)
            break;

        first = second + 1;
    }

    return output;
}

class HttpWorker
{
public:
	HttpWorker(HttpWorker const&) = delete;
	HttpWorker& operator=(HttpWorker const&) = delete;

	HttpWorker(tcp::acceptor& acceptor, Registry& registry) :
		_acceptor(acceptor),
		_registry(registry)
	{
	}

	void start()
	{
		accept();
		check_deadline();
	}

private:
	using alloc_t = fields_alloc<char>;
	//using request_body_t = http::basic_dynamic_body<boost::beast::flat_static_buffer<1024 * 1024>>;
	using request_body_t = http::string_body;

	// The acceptor used to listen for incoming connections.
	tcp::acceptor& _acceptor;

	// The socket for the currently connected client.
	tcp::socket _socket{_acceptor.get_executor().context()};

	// The buffer for performing reads
	boost::beast::flat_static_buffer<8192> _buffer;

	// The allocator used for the fields in the request and reply.
	alloc_t _alloc{8192};

	// The parser for reading the requests
	boost::optional<http::request_parser<request_body_t, alloc_t>> _parser;

	// The timer putting a time limit on requests.
	boost::asio::basic_waitable_timer<std::chrono::steady_clock> request_deadline_{
		_acceptor.get_executor().context(), (std::chrono::steady_clock::time_point::max)()};

	// The string-based response message.
	boost::optional<http::response<http::string_body, http::basic_fields<alloc_t>>> _string_response;

	// The string-based response serializer.
	boost::optional<http::response_serializer<http::string_body, http::basic_fields<alloc_t>>> _string_serializer;

	Registry& _registry;

	void accept()
	{
		// Clean up any previous connection.
		boost::beast::error_code ec;
		_socket.close(ec);
		_buffer.consume(_buffer.size());

		_acceptor.async_accept(
			_socket,
			[this](boost::beast::error_code ec)
			{
				if (ec)
				{
					accept();
				}
				else
				{
					// Request must be fully processed within 60 seconds.
					request_deadline_.expires_after(
						std::chrono::seconds(60));

					read_request();
				}
			});
	}

	void read_request()
	{
		// On each read the parser needs to be destroyed and
		// recreated. We store it in a boost::optional to
		// achieve that.
		//
		// Arguments passed to the parser constructor are
		// forwarded to the message object. A single argument
		// is forwarded to the body constructor.
		//
		// We construct the dynamic body with a 1MB limit
		// to prevent vulnerability to buffer attacks.
		//
		_parser.emplace(
			std::piecewise_construct,
			std::make_tuple(),
			std::make_tuple(_alloc));

		http::async_read(
			_socket,
			_buffer,
			*_parser,
			[this](boost::beast::error_code ec, std::size_t)
			{
				if (ec)
					accept();
				else
					process_request(_parser->get());
			});
	}

	void process_request(http::request<request_body_t, http::basic_fields<alloc_t>> const& req)
	{
		switch (req.method())
		{
		case http::verb::get:
			process_webrpc_request(req.target());
			break;

		default:
			// We return responses indicating an error if
			// we do not recognize the request method.
			send_bad_response(
				http::status::bad_request,
				"Invalid request-method '" + req.method_string().to_string() + "'\r\n");
			break;
		}
	}

	void process_webrpc_request(const boost::beast::string_view request)
	{
		// todo: cleanup usage of string_view / std::string
		auto parts = split_string_view(request, "/");
		const std::string name = parts.size() > 0 ? parts.at(0).to_string() : "no method";
		const std::string args = parts.size() > 1 ? parts.at(1).to_string() : "no arguments";

		const auto method = _registry.find(name);
		std::cout << "-" << request << "-" << name << "-" << args << "-";
		if (method != _registry.end())
		{
			try
			{
				const auto oval = parse_value(args);
				const auto ores = method->second->execute(oval);
				const auto result = ores.value_or(null_t{}).to_string();
				std::cout << " exec=" << method->first << "(" << args << ")" << "=>" << result;
				send_message(result);
			}
			catch (const std::exception& e)
			{
				std::cout << "caught exception: " << e.what() << std::endl;
				send_bad_response(
					http::status::bad_request,
					"Invalid webrpc request '" + std::string(e.what()) + "'\r\n"
				);
			}
		}
		else
		{
			send_bad_response(
				http::status::bad_request,
				"Invalid webrpc request '" + request.to_string() + "'\r\n"
			);
		}
		std::cout << std::endl;
	}

	void send_bad_response(
		http::status status,
		std::string const& error)
	{
		_string_response.emplace(
			std::piecewise_construct,
			std::make_tuple(),
			std::make_tuple(_alloc));

		_string_response->result(status);
		_string_response->keep_alive(false);
		_string_response->set(http::field::server, "Beast");
		_string_response->set(http::field::content_type, "text/plain");
		_string_response->body() = error;
		_string_response->prepare_payload();

		_string_serializer.emplace(*_string_response);

		http::async_write(
			_socket,
			*_string_serializer,
			[this](boost::beast::error_code ec, std::size_t)
			{
				_socket.shutdown(tcp::socket::shutdown_send, ec);
				_string_serializer.reset();
				_string_response.reset();
				accept();
			});
	}

	void send_message(boost::beast::string_view message)
	{
		_string_response.emplace(
			std::piecewise_construct,
			std::make_tuple(),
			std::make_tuple(_alloc));

		_string_response->result(http::status::ok);
		_string_response->keep_alive(false);
		_string_response->set(http::field::server, "Beast");
		_string_response->set(http::field::content_type, "text/plain");
		_string_response->body() = message.to_string() + "\n";
		_string_response->prepare_payload();

		_string_serializer.emplace(*_string_response);

		http::async_write(
			_socket,
			*_string_serializer,
			[this](boost::beast::error_code ec, std::size_t)
			{
				_socket.shutdown(tcp::socket::shutdown_send, ec);
				_string_serializer.reset();
				_string_response.reset();
				accept();
			});
	}

	void check_deadline()
	{
		// The deadline may have moved, so check it has really passed.
		if (request_deadline_.expiry() <= std::chrono::steady_clock::now())
		{
			// Close socket to cancel any outstanding operation.
			// boost::beast::error_code ec;
			_socket.close();

			// Sleep indefinitely until we're given a new deadline.
			request_deadline_.expires_at(
				std::chrono::steady_clock::time_point::max());
		}

		request_deadline_.async_wait(
			[this](boost::beast::error_code)
			{
				check_deadline();
			});
	}
};

void Server::register_method(IMethodUP&& method)
{
	_registry.emplace(std::make_pair(method->get_name(), std::move(method)));
}

void Server::run()
{
	// todo: check how to handle io_context in boost asio application
	boost::asio::io_context ioc{1};
	tcp::acceptor acceptor{ioc, _endpoint};

	std::list<HttpWorker> workers;
	for (int i = 0; i < _num_workers; ++i)
	{
		workers.emplace_back(acceptor, _registry);
		workers.back().start();
	}

	ioc.run();
}
