#pragma once

#include <webrpc/detail/fields_alloc.h>
#include <webrpc/Registry.h>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace detail
{

using tcp	  = boost::asio::ip::tcp; // from <boost/asio.hpp>
namespace http = boost::beast::http;   // from <boost/beast/http.hpp>

class HttpWorker
{
	public:
	HttpWorker(HttpWorker const&) = delete;
	HttpWorker& operator=(HttpWorker const&) = delete;

	HttpWorker(tcp::acceptor& acceptor, Registry& registry);

	~HttpWorker();

	void start();

	void set_verbose(bool verbose);

	private:
	using alloc_t = fields_alloc<char>;

	// using request_body_t = http::basic_dynamic_body<boost::beast::flat_static_buffer<1024 * 1024>>;
	using request_body_t = http::string_body;

	void accept();

	void read_request();

	void process_request(http::request<request_body_t, http::basic_fields<alloc_t>> const& req);

	void process_target(const boost::beast::string_view trg);

	void send_bad_response(http::status status, std::string const& error);

	void send_message(boost::beast::string_view message);

	void check_deadline();

	// The acceptor used to listen for incoming connections.
	tcp::acceptor& _acceptor;

	// The socket for the currently connected client.
	tcp::socket _socket{_acceptor.get_executor()};

	// The buffer for performing reads
	boost::beast::flat_static_buffer<8192> _buffer;

	// The allocator used for the fields in the request and reply.
	alloc_t _alloc{8*8192};

	// The parser for reading the requests
	boost::optional<http::request_parser<request_body_t, alloc_t>> _parser;

	// The timer putting a time limit on requests.
	boost::asio::basic_waitable_timer<std::chrono::steady_clock> request_deadline_{
		_acceptor.get_executor(), (std::chrono::steady_clock::time_point::max)()};

	// The string-based response message.
	boost::optional<http::response<http::string_body, http::basic_fields<alloc_t>>> _string_response;

	// The string-based response serializer.
	boost::optional<http::response_serializer<http::string_body, http::basic_fields<alloc_t>>> _string_serializer;

	Registry& _registry;

	bool _verbose = false;
};

}
