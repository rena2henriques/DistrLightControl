#ifndef TCP_SERVER
#define TCP_SERVER

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

// NOT USED YET

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
//#include "command.h"
#include "serialComm.h"
using boost::asio::ip::tcp;

// Asynchronous TCP server

class session {

public:
	session(boost::asio::io_service& io_service, std::shared_ptr <SerialComm> arduino_);

	tcp::socket& socket();

	void start();

private:
	void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

	void handle_write(const boost::system::error_code& error);

	tcp::socket socket_;
  	enum { max_length = 1024 };
  	char request_[max_length];
  	std::string response_;

  	std::shared_ptr <SerialComm> arduino;
};

//----------------------------------------------------------------------------

class Tcp_server {

public:
	Tcp_server(boost::asio::io_service& io_service, short port, std::shared_ptr <SerialComm> arduino_);

private:
	void start_accept();

	void start_read_input();

	void handle_read_input(const boost::system::error_code& error,
      std::size_t length);

	void handle_accept(session* new_session,
      const boost::system::error_code& error);

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	boost::asio::posix::stream_descriptor input_;
	boost::asio::streambuf input_buffer_;

	std::shared_ptr <SerialComm> arduino;
};

#endif
