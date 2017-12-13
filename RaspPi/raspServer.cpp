
//Compile as:  // g++ -g -O -Wall -pedantic -std=c++11 tcpServer.cpp
// serialComm.cpp raspServer.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include "tcpServer.h"
#include "serialComm.h"
#include "i2cComm.h"
#include "database.h"

using namespace std;
using namespace boost::asio;

// creats the object where we handle the data coming from arduino
shared_ptr <Database> db (new Database(6000));

// creates I2C object
shared_ptr <I2Comm> i2c_slave (new I2Comm(db));


void tcp_serial_thread(int port){

  try {

    // creating io services for tcp and arduino
    boost::asio::io_service io_service;

    shared_ptr <SerialComm> arduino (new SerialComm(io_service, "/dev/ttyACM0", db, i2c_slave));

    Tcp_server s(io_service, port, arduino);

    io_service.run();

  } catch (std::exception& e) {

    std::cerr << "Exception: " << e.what() << "\n";
  }

  return;
}


void i2c_thread() {

  // creates object
  i2c_slave->sniffer();

  return;
}


int main(int argc, char* argv[])
{
  if (argc != 2) {
      
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
  }

  // std::thread t1{read_keyboard}; ----> for exiting the program

  // starts the server and serial comm thread
  thread threadTcp(tcp_serial_thread, atoi(argv[1]) );

  thread threadI2C(i2c_thread);

  threadTcp.join();
  threadI2C.join();

  cout<<"Goodbye!"<<endl;
  return 0;
}