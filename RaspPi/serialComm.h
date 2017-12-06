#ifndef TCP_SERVER
#define TCP_SERVER

//Compile as:  g++ -std=c++11 async_tcp_echo_server.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>
using namespace boost::system;
using namespace boost::asio;

// Asynchronous Serial Communications

class SerialComm {

public:

	SerialComm(io_service& io_, std::string port_name);

	~SerialComm();

	void write_handler(const error_code &ec);

	void timer_handler(const error_code &ec);

	void read_handler(const error_code &ec);

private:
	void start_read_input();
	void handle_read_input(const boost::system::error_code& error, std::size_t length);

	int counter = 0;

	boost::asio::io_service& io;
 	boost::asio::serial_port sp;
  	boost::asio::streambuf read_buf;
  	steady_timer tim;

  	boost::asio::posix::stream_descriptor input_;
	boost::asio::streambuf input_buffer_;
};

#endif