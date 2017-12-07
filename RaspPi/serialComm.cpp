#include "serialComm.h"

// compile with
// g++ -g -O -Wall -pedantic -std=c++11 mainSerial.cpp serialComm.cpp -lpthread -lboost_system -o serialArduino

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

SerialComm::~SerialComm() {
  sp.close();
}

// teste
void SerialComm::sendMessage(std::string message) {

    std::ostringstream os;
    os << message << '\n';

    // Read a line of input entered by the user.
    async_write(sp, buffer(os.str()), boost::bind(&SerialComm::sendMessageHandler, this, 
      boost::asio::placeholders::error));
}

void SerialComm::sendMessageHandler(const boost::system::error_code& ec){

  // do nothing
  std::cout << "Order sent to Arduino Serial" << std::endl;

}