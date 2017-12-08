
//Compile as:  // g++ -g -O -Wall -pedantic -std=c++11 tcpServer.cpp
// serialComm.cpp raspServer.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include "tcpServer.h"
#include "serialComm.h"

using namespace std;
using namespace boost::asio;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2) {
      
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    // creating io services for tcp and arduino
    boost::asio::io_service io_service;

    shared_ptr <SerialComm> arduino (new SerialComm(io_service, "/dev/ttyACM0"));

    Tcp_server s(io_service, std::atoi(argv[1]), arduino);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}