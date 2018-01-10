#include "serialComm.h"

// Constructor
SerialComm::SerialComm(io_service& io_serv,std::string port_name , shared_ptr <Database> db_, shared_ptr <I2Comm> i2c_slave_)
	: io(io_serv), sp(io), input_(io, ::dup(STDIN_FILENO)), input_buffer_(1024), db(db_), i2c_slave(i2c_slave_) {

	try{
    // opens port
		sp.open(port_name);
    // sets baudrate
	  sp.set_option(serial_port_base::baud_rate(115200));
		
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
  std::cout << "Order sent to Arduino Serial" << std::endl;

}

std::string SerialComm::getCommand(char message[]) {

  std::cout << "Get command " << message << std::endl;

  std::string s(message);
  std::string response;

  // get current illuminance
  if (message[2] == 'l') {
    response = db->getCurrentValues(message);

  // get current duty cycle
  } else if (message[2] == 'd') {
    response = db->getCurrentValues(message);

  // other type of get
  } else {
    // sends message to arduino master
    sendMessage(s);

    // waits response
    response = i2c_slave->receiveGet();
  }

  // the messages need to have \n in the end for the client to work
  response += '\n';

  return response;
}


std::string SerialComm::setCommand(char message[]) {

  std::string s(message);  

  cout << "Setting: " << s;

  // sends request to arduino
  sendMessage(s);

  return "ack\n";
}

std::string SerialComm::restartCommand() {

  //sends a restart flag to the main arduino 
  sendMessage("r");

  // resets buffers and timer
  db->clearBuffers();

  return "ack\n";
}

std::string SerialComm::lastMinCommand(char message[]) {

  std::string response;

  // gets the response of the values corresponding to one minute ago
  response = db->getLastMinuteValues(message);

  return response;
}


std::string SerialComm::consensusCommand() {
  //sends a flag to the main arduino to turn off consensuss
  sendMessage("k");

  return "ack\n";
}


std::string SerialComm::streaming(int address, char type) {

  std::string response;

  // the case of having received new data from this arduino
  if ( db->getLastReadState(address) == 0) {

    response = db->getStreamValues(address, type);

  // the case of not having received new data from this arduino
  } else {
    response = "no new data";
  }

  return response;
}