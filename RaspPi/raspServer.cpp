
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

// creating io services for tcp and arduino
io_service io;

shared_ptr <SerialComm> arduino (new SerialComm(io, "/dev/ttyACM0", db, i2c_slave));


void tcp_serial_thread(int port){

  try {

    Tcp_server s(io, port, arduino);

    io.run();

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

// used for exiting the program because ctrl-c doesn't terminate GPIO library
void read_keyboard(){
  while (1) {
    string command;
    getline(std::cin,command);
    if(command=="quit"){
      i2c_slave->~I2Comm();
      arduino->~SerialComm();
      exit(0);
    }
  }
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
      
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
  }

  thread threadkeyboard(read_keyboard);// ----> for exiting the program

  // starts the server and serial comm thread
  thread threadTcp(tcp_serial_thread, atoi(argv[1]) );

  thread threadI2C(i2c_thread);

  threadTcp.join();
  threadI2C.join();

  cout<<"Goodbye!"<<endl;
  return 0;
}