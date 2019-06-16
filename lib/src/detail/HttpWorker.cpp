#include <webrpc/detail/HttpWorker.h>
#include <webrpc/Parser.h>

#include <boost/optional/optional_io.hpp>

namespace detail
{
void replace_all(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
	{
		return;
	}
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

std::string decode_url(const std::string& url)
{
	std::string result(url);
	replace_all(result, "%7B", "{");
	replace_all(result, "%7D", "}");
	return result;
}

HttpWorker::HttpWorker(tcp::acceptor& acceptor, Registry& registry)
	: _acceptor(acceptor)
	, _registry(registry)
{}

void HttpWorker::start()
{
	accept();
	check_deadline();
}

void HttpWorker::set_verbose(bool verbose)
{
	_verbose = verbose;
}

void HttpWorker::accept()
{
	// Clean up any previous connection.
	boost::beast::error_code ec;
	_socket.close(ec);
	_buffer.consume(_buffer.size());

	_acceptor.async_accept(_socket, [this](boost::beast::error_code ec) {
		if (ec)
		{
			accept();
		}
		else
		{
			// Request must be fully processed within 60 seconds.
			request_deadline_.expires_after(std::chrono::seconds(60));

			read_request();
		}
	});
}

void HttpWorker::read_request()
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
	_parser.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));

	http::async_read(_socket, _buffer, *_parser, [this](boost::beast::error_code ec, std::size_t) {
		if (ec)
			accept();
		else
			process_request(_parser->get());
	});
}

void HttpWorker::process_request(http::request<request_body_t, http::basic_fields<alloc_t>> const& req)
{
	if (_verbose)
	{
		std::cout << req /* << std::endl*/;
	}

	switch (req.method())
	{
	case http::verb::get:
		process_target(req.target());
		break;

	default:
		// We return responses indicating an error if
		// we do not recognize the request method.
		send_bad_response(
			http::status::bad_request, "Invalid request-method '" + req.method_string().to_string() + "'\r\n");
		break;
	}
}

void HttpWorker::process_target(const boost::beast::string_view trg)
{
	// remove leading '/' if present
	const auto trg_str = (trg[0] == '/' ? std::string(std::string{trg}, 1) : std::string{trg});
	const auto target = Parser::parse_target(decode_url(trg_str));

	if (!target)
	{
		send_bad_response(http::status::bad_request, "Invalid webrpc request '" + trg.to_string() + "'\r\n");
	}
	else
	{
		const auto method = _registry.find(target->method);

		if (method == _registry.end())
		{
			send_bad_response(http::status::bad_request, "Invalid webrpc request '" + trg.to_string() + "'\r\n");
		}

		try
		{
			const auto oval = target->args ? Parser::parse_value(*target->args) : boost::none;
			//			const auto oval = Parser::parse_value(target->args.value_or("")); // todo: enable when parsing empty
			// strings is supported
			const auto ores   = method->second->execute(oval);
			const auto result = ores.value_or(null_t{}).to_string();

			if (!_verbose)
			{
				static auto idx = 0;
				std::cout << idx << "> " << trg << std::endl << idx << "< " << result << std::endl;
				++idx;
			}

			send_message(result);
		}
		catch (const std::exception& e)
		{
			std::cout << "caught exception: " << e.what() << std::endl;
			send_bad_response(http::status::bad_request, "Invalid webrpc request '" + std::string(e.what()) + "'\r\n");
		}
	}
}

void HttpWorker::send_bad_response(http::status status, std::string const& error)
{
	_string_response.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));

	_string_response->result(status);
	_string_response->keep_alive(false);
	_string_response->set(http::field::server, "Beast");
	_string_response->set(http::field::content_type, "text/plain");
	_string_response->body() = error;
	_string_response->prepare_payload();

	if (_verbose)
	{
		std::cout << _string_response /* << std::endl*/;
	}

	_string_serializer.emplace(*_string_response);

	http::async_write(_socket, *_string_serializer, [this](boost::beast::error_code ec, std::size_t) {
		_socket.shutdown(tcp::socket::shutdown_send, ec);
		_string_serializer.reset();
		_string_response.reset();
		accept();
	});
}

void HttpWorker::send_message(boost::beast::string_view message)
{
	_string_response.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));

	_string_response->result(http::status::ok);
	_string_response->keep_alive(false);
	_string_response->set(http::field::server, "Beast");
	_string_response->set(http::field::content_type, "text/plain");
	_string_response->body() = message.to_string() + "\n";
	_string_response->prepare_payload();

	if (_verbose)
	{
		std::cout << _string_response << std::endl;
	}

	_string_serializer.emplace(*_string_response);

	http::async_write(_socket, *_string_serializer, [this](boost::beast::error_code ec, std::size_t) {
		_socket.shutdown(tcp::socket::shutdown_send, ec);
		_string_serializer.reset();
		_string_response.reset();
		accept();
	});
}

void HttpWorker::check_deadline()
{
	// The deadline may have moved, so check it has really passed.
	if (request_deadline_.expiry() <= std::chrono::steady_clock::now())
	{
		// Close socket to cancel any outstanding operation.
		// boost::beast::error_code ec;
		_socket.close();

		// Sleep indefinitely until we're given a new deadline.
		request_deadline_.expires_at(std::chrono::steady_clock::time_point::max());
	}

	request_deadline_.async_wait([this](boost::beast::error_code) { check_deadline(); });
}

} // namespace detail
