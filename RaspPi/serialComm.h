#ifndef SERIAL_COMM
#define SERIAL_COMM

//Compile as:  g++ -std=c++11 async_tcp_echo_server.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>
#include <string.h>
#include "database.h"
#include "i2cComm.h"
using namespace boost::system;
using namespace boost::asio;

// Asynchronous Serial Communications

class SerialComm {

public:

	SerialComm(io_service& io_serv,std::string port_name , shared_ptr <Database> db_, shared_ptr <I2Comm> i2c_slave_);

	~SerialComm();

	void sendMessage(std::string message);

	// for commands of the type 'g'
	std::string getCommand(char message[]);

	// for commands of the type 's'
	std::string setCommand(char message[]);

	// for commands of the type 'r'
	std::string restartCommand();

	// other commands
	std::string lastMinCommand(char message[]);

	// turn on/off consensus
	std::string consensusCommand();

	std::string streaming(int address, char type);

private:

	void sendMessageHandler(const boost::system::error_code& ec);

	boost::asio::io_service& io;
 	boost::asio::serial_port sp;

  	boost::asio::posix::stream_descriptor input_;
	boost::asio::streambuf input_buffer_;

	// creats the object where we handle the data coming from arduino
	shared_ptr <Database> db;

	// creates I2C object
	shared_ptr <I2Comm> i2c_slave;
};

#endif