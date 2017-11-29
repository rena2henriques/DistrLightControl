#ifndef TCP_SERVER
#define TCP_SERVER

#include <unistd.h>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/share_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
using boost::asio;
using ip::tcp;

class tcp_connection: public boost::enable_shared_from_this<tcp_connection> {

private:
	tcp::socket sock_;
	std::string msg_;
	tcp_connection(io_service &io)


};

class TcpServer {






};

#endif