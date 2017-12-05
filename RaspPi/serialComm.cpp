#include "serialComm.h"

SerialComm::SerialComm(io_service& io_serv,std::string port_name) : io(io_serv), sp(io), tim(io){

	try{
		sp.open(port_name);
	    sp.set_option(serial_port_base::baud_rate(115200));

	    //program timer for write operations
	    tim.expires_from_now(std::chrono::seconds(5));
	    tim.async_wait(boost::bind(&SerialComm::timer_handler, this, boost::asio::placeholders::error));
	    //program chain of read operations
		async_read_until(sp,read_buf,'\n',boost::bind(&SerialComm::read_handler, this, boost::asio::placeholders::error));
		
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
   std::cout << &read_buf;
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
   os << "Counter = " << ++counter;
   async_write(sp, buffer(os.str()), boost::bind(&SerialComm::write_handler, this, boost::asio::placeholders::error));

}