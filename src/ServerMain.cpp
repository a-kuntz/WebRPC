//
// Copyright (c) 2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, fast
//
//------------------------------------------------------------------------------

#include <webrpc/Method.h>

#include <fields_alloc.h>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <string>

namespace ip = boost::asio::ip;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class http_worker
{
public:
    http_worker(http_worker const&) = delete;
    http_worker& operator=(http_worker const&) = delete;

    http_worker(tcp::acceptor& acceptor, const std::string& doc_root) :
        _acceptor(acceptor),
        _doc_root(doc_root)
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

    // The path to the root of the document directory.
    std::string _doc_root;

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
            std::cout << req.target() << std::endl;
            send_echo(req.target());
            break;

        default:
            std::cout
                << "invalid request-method: " << req.method_string()
                << " request: " << req.target()
                << std::endl;
            // We return responses indicating an error if
            // we do not recognize the request method.
            send_bad_response(
                http::status::bad_request,
                "Invalid request-method '" + req.method_string().to_string() + "'\r\n");
            break;
        }
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

    void send_echo(boost::beast::string_view target)
    {
        _string_response.emplace(
            std::piecewise_construct,
            std::make_tuple(),
            std::make_tuple(_alloc));

        _string_response->result(http::status::ok);
        _string_response->keep_alive(false);
        _string_response->set(http::field::server, "Beast");
        _string_response->set(http::field::content_type, "text/plain");
        _string_response->body() = target.to_string() + "\n";
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
            boost::beast::error_code ec;
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

int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
        if (argc != 6)
        {
            std::cerr << "Usage: server <address> <port> <doc_root> <num_workers> {spin|block}\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    server 0.0.0.0 80 . 100 block\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    server 0::0 80 . 100 block\n";
            return EXIT_FAILURE;
        }

        const auto address = boost::asio::ip::make_address(argv[1]);
        const unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));
        const std::string doc_root = argv[3];
        const int num_workers = std::atoi(argv[4]);
        const bool spin = (std::strcmp(argv[5], "spin") == 0);

        boost::asio::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};

        std::list<http_worker> workers;
        for (int i = 0; i < num_workers; ++i)
        {
            workers.emplace_back(acceptor, doc_root);
            workers.back().start();
        }

        if (spin)
          for (;;) ioc.poll();
        else
          ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
