#ifndef SERIAL_COMM
#define SERIAL_COMM

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

	void sendMessage(std::string message);

private:

	void sendMessageHandler(const boost::system::error_code& ec);

	boost::asio::io_service& io;
 	boost::asio::serial_port sp;

  	boost::asio::posix::stream_descriptor input_;
	boost::asio::streambuf input_buffer_;
};

#endif