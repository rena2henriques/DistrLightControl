//Compile as:  g++ -std=c++11 async_tcp_echo_server.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "tcpServer.h"
#include "command.h"

class Command Cmd;

  session::session(boost::asio::io_service& io_service)
    : socket_(io_service), response_("") {
  }

  tcp::socket& session::socket() {

    return socket_;
  }

  void session::start() {
    socket_.async_read_some(boost::asio::buffer(request_, max_length),
        boost::bind(&session::handle_read, this, _1, _2 )); 
  }

  void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {

      std::cout << "received: " << request_ << endl;

      if (request_[0] == 'g') {
        // takes care of get requests
        response_ = Cmd.getCommand(request_);

        boost::asio::async_write(socket_, boost::asio::buffer(response_, response_.length()),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
        
      } else if (request_[0] == 'r'){

        response_ = Cmd.restartCommand();

        // sends the info to the client
        boost::asio::async_write(socket_, boost::asio::buffer(response_, response_.length()),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

      } else if (request_[0] == 's') {

        // gets the response requested
        response_ = Cmd.setCommand(request_);

        // sends the info to the client
        boost::asio::async_write(socket_, boost::asio::buffer(response_, response_.length()),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

      } else if (request_[0] == 'b' || request_[0] == 'c' || request_[0] == 'd') {

        response_ = Cmd.streamCommand();

        // sends the info to the client
        boost::asio::async_write(socket_, boost::asio::buffer(response_, response_.length()),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

      } else if (request_[0] == '\n') {

        // do nothing
        boost::asio::async_write(socket_, boost::asio::buffer("Ping received."),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

      } else {
        // not a recognizable command 
        boost::asio::async_write(socket_, boost::asio::buffer("Request not found."),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
      }

      memset(request_, 0, sizeof(request_));
    }
    else
    {
      delete this;
    }
  }

  void session::handle_write(const boost::system::error_code& error) {
    if (!error) {

      socket_.async_read_some(boost::asio::buffer(request_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

    } else {
      delete this;
    }
  }

  Tcp_server::Tcp_server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      input_(io_service, ::dup(STDIN_FILENO)),
      input_buffer_(1024),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
    start_read_input();
  }

  void Tcp_server::start_accept() {

    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&Tcp_server::handle_accept, this, new_session, _1));
  }

  void Tcp_server::start_read_input() {

    // Read a line of input entered by the user.
    boost::asio::async_read_until(input_, input_buffer_, '\n', 
        boost::bind(&Tcp_server::handle_read_input, this, _1, _2));
  }

  void Tcp_server::handle_read_input(const boost::system::error_code& error,
      std::size_t length) {
    if (!error)
    {
       std::cout << &input_buffer_ << std::endl;
    }
    start_read_input();
  }

  void Tcp_server::handle_accept(session* new_session,
      const boost::system::error_code& error) {
    if (!error)
    {
      new_session->start();
    }
    else
    {
      delete new_session;
    }

    start_accept();
  }
