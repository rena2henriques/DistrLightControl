#include "serialComm.h"

// compile with
// g++ -g -O -Wall -pedantic -std=c++11 mainSerial.cpp serialComm.cpp -lpthread -lboost_system -o serialArduino

// Constructor
SerialComm::SerialComm(io_service& io_serv,std::string port_name , shared_ptr <Database> db_, shared_ptr <I2Comm> i2c_slave_)
	: io(io_serv), sp(io), input_(io, ::dup(STDIN_FILENO)), input_buffer_(1024), db(db_), i2c_slave(i2c_slave_) {

	try{
    // opens port
		sp.open(port_name);
    // sets baudrate
	  sp.set_option(serial_port_base::baud_rate(115200));

		//start_read_input();
		
	} catch(boost::system::system_error& error) {
	    boost::system::error_code ec =	error.code();
	    std::cerr<<"ERROR:initialization of serial port "<<ec.message()<<std::endl;
	}
}

// Destructor
SerialComm::~SerialComm() {
  sp.close();
}

// Sends the string message to the arduino
void SerialComm::sendMessage(std::string message) {

    std::ostringstream os;
    os << message << '\n';

    // Read a line of input entered by the user.
    async_write(sp, buffer(os.str()), boost::bind(&SerialComm::sendMessageHandler, this, 
      boost::asio::placeholders::error));
}

void SerialComm::sendMessageHandler(const boost::system::error_code& ec){

  // do nothing
  std::cout << "Order sent to Arduino Serial" << std::endl; // teste

}

std::string SerialComm::getCommand(char message[]) {

  std::cout << "Get command " << message << std::endl;

  std::string s(message);

  // sends message to arduino master
  sendMessage(s);

  std::string response = i2c_slave->receiveGet();
  /*db->printBuffers(1);
  db->printBuffers(2);*/

  // the messages need to have \n in the end for the client to work
  response += '\n';

  return response;
}


std::string SerialComm::setCommand(char message[]) {

  std::string s(message);  

  cout << "Sending: " << s;

  // sends request to arduino
  sendMessage(s);

  return "ack\n";
}

std::string SerialComm::restartCommand() {

  //sends a restart flag to the main arduino 
  sendMessage("r");

  return "ack\n";
}

std::string SerialComm::streamCommand() {

  return "Work in progress\n";
}