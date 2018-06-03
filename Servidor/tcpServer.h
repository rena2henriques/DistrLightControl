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

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <boost/bind.hpp>
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
#include <chrono>
#include <boost/asio/steady_timer.hpp>
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

	void deadline_handler(const boost::system::error_code & ec);


	// aux variables <----------
	// to stop the timer
	int stop = 1;
	char order = 'z'; // <-- useless
	int address = -1;
	char type = 'z';

	tcp::socket socket_;
  	enum { max_length = 1024 };
  	char request_[max_length];
  	std::string response_;

  	std::shared_ptr <SerialComm> arduino;
  	boost::asio::steady_timer tim;
};

//----------------------------------------------------------------------------

class Tcp_server {

public:
	Tcp_server(boost::asio::io_service& io_service, short port, std::shared_ptr <SerialComm> arduino_);

private:
	void start_accept();

	void handle_accept(session* new_session,
      const boost::system::error_code& error);

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;

	std::shared_ptr <SerialComm> arduino;
};

#endif
