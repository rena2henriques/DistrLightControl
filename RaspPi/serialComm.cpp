#include "serialComm.h"

// compile with
// g++ -g -O -Wall -pedantic -std=c++11 mainSerial.cpp serialComm.cpp -lpthread -lboost_system -o serialArduino

// Constructor
SerialComm::SerialComm(io_service& io_serv,std::string port_name)
	: io(io_serv), sp(io), input_(io, ::dup(STDIN_FILENO)), input_buffer_(1024){

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

std::string SerialComm::getCommand(char message[]) { // <---------- TODO

  std::cout << "Get command " << message << std::endl;

  return "Work in progress";
}


std::string SerialComm::setCommand(char message[]) { // <---------- TODO

  char desk[4] = "";
  char flag[2] = ""; // 0 non-ocuppied, 1 occupied

  /* get the first token, should be 's' in this case*/
  char * token = strtok(message, " ");

  // second parameter   
  token = strtok(NULL, " ");
  if (token != NULL) {
    strncpy(desk, token, 3); // watch out
  }
  
  // third parameter - state   
  token = strtok(NULL, " ");
  if (token != NULL){
    strncpy(flag, token, 1); // watch out
  }

  //printf("%s %s\n", desk, flag); // test

  //sendSerialMsg(desk, flag); <--------------- TODO

  return "Work in progress";
}

std::string SerialComm::restartCommand() { // <---------- TODO

  //sends a restart flag to the main arduino 

  //sendSerialMsg("r");

  return "ack";
}

std::string SerialComm::streamCommand() { // <---------- TODO

  return "Work in progress";
}