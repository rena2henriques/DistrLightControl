#include "serialComm.h"

// g++ -g -O -Wall -pedantic -std=c++11 mainSerial.cpp serialComm.cpp -lpthread -lboost_system -o serialArduino

SerialComm::SerialComm(io_service& io_serv,std::string port_name)
	: io(io_serv), sp(io), tim(io), input_(io, ::dup(STDIN_FILENO)), input_buffer_(1024){

	try{
		sp.open(port_name);
	    sp.set_option(serial_port_base::baud_rate(115200));

	    // //program timer for write operations
	    // tim.expires_from_now(std::chrono::seconds(5));
	    // tim.async_wait(boost::bind(&SerialComm::timer_handler, this, boost::asio::placeholders::error));
	    //program chain of read operations
		async_read_until(sp,read_buf,'\n',boost::bind(&SerialComm::read_handler, this, boost::asio::placeholders::error));

		start_read_input();
		
	} catch(boost::system::system_error& error) {
	    boost::system::error_code ec =	error.code();
	    std::cerr<<"ERROR:initialization of serial port "<<ec.message()<<std::endl;
	}
}

SerialComm::~SerialComm() {

  sp.close();
}

void SerialComm::read_handler(const error_code &ec) {
   //data is now available at read_buf

   std::cout << "Read_handler -> " << &read_buf << std::endl;

   //program new read cycle
   async_read_until(sp,read_buf,'\n', boost::bind(&SerialComm::read_handler, this, boost::asio::placeholders::error));

}

void SerialComm::write_handler(const error_code &ec) {
   //writer done – program new deadline
    tim.expires_from_now(std::chrono::seconds(5));
    tim.async_wait(boost::bind(&SerialComm::timer_handler, this, boost::asio::placeholders::error));
}

void SerialComm::timer_handler(const error_code &ec) {
   //timer expired – launch new write operation
   std::ostringstream os;
   os << "Timer_Handler -> Counter = " << ++counter << "\n";
   async_write(sp, buffer(os.str()), boost::bind(&SerialComm::write_handler, this, boost::asio::placeholders::error));

}

void SerialComm::start_read_input() {

    // Read a line of input entered by the user.
    boost::asio::async_read_until(input_, input_buffer_, '\n', 
        boost::bind(&SerialComm::handle_read_input, this, _1, _2));
}

void SerialComm::handle_read_input(const boost::system::error_code& error,
      std::size_t length) {
    if (!error)
    {
       std::cout << &input_buffer_ << std::endl;

       async_write(sp, input_buffer_, boost::bind(&SerialComm::start_read_input, this));
    }
    start_read_input();
}