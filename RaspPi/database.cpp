#include "database.h"

Database::Database(int capacity_) : capacity(capacity_) {

	last_restart = std::chrono::system_clock::now(); 
}


Database::~Database() {

	// clean buffers after stopping the i2c communication
	clearBuffers();
}


void Database::insertBuffer(int address, float lux, float dc) {

	// tests if the arduino has been present til now
	if(buffs[address].ilum.capacity() == 0 && buffs[address].dutyCycle.capacity() == 0) {
		buffs[address].ilum.set_capacity(capacity);
		buffs[address].dutyCycle.set_capacity(capacity);
		numBuffers++;
	}

	struct Info aux;
	aux.timestamp = std::chrono::system_clock::now(); 

	// insert the data in the circular buffers
	aux.data = lux;
	buffs[address].ilum.push_front(aux);

	aux.data = dc;
	buffs[address].dutyCycle.push_front(aux);

	// to be used in the stream mode
	buffs[address].lastRead = 0;

	return;
}


void Database::clearBuffers(){

	for(int i = 0; i < 127; i++) {
		buffs[i].ilum.clear();
		buffs[i].dutyCycle.clear();
		buffs[i].lastRead = 0;
	}

	last_restart = std::chrono::system_clock::now();  
}

int Database::getNumBuffers(){
	return numBuffers;
}

std::string Database::getCurrentValues(char message[]) {

	char request = 'z';
	char type = 'z';
	int address = -1;
	char aux_response[20];

	if( sscanf(message, "%c %c %d", &request, &type, &address) != 3)
		return "Message Invalid\n"; // message hasnt been sent correctly

	if (type == 'l'){
		if (buffs[address].ilum.size() > 0 && buffs[address].ilum.capacity() != 0) {
			snprintf(aux_response, 20, "l %d %.2f", address, buffs[address].ilum[0].data);
		} else {
			return "Message Invalid\n"; // message hasnt been sent correctly
		}

	} else if (type == 'd') {
		if (buffs[address].dutyCycle.size() > 0 && buffs[address].dutyCycle.capacity() != 0) {
			snprintf(aux_response, 20, "d %d %.2f", address, buffs[address].dutyCycle[0].data);
		} else {
			return "Message Invalid\n"; // message hasnt been sent correctly
		}
	} else {
		return "Message Invalid\n"; // message hasnt been sent correctly
	}

	std::string response(aux_response);

	return response;
}

// returns the values of a buffer in the last minute
std::string Database::getLastMinuteValues(char message[]) {

	char request = 'z';
	int address = -1;
	char type = 'z';
	unsigned int i = 0;
	std::string response("b ");

	if( sscanf(message, "%c %c %d", &request, &type, &address) != 3)
		return "Message Invalid\n"; // message hasnt been sent correctly

	response += type;
	response += ' ';
	response.append(to_string(address));
	response += ' ';

	// gets the current time
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); 

	// if requested luminance
	if (type == 'l') {

		if (buffs[address].ilum.size() == 0 && buffs[address].ilum.capacity() == 0) {
			return "Address not valid!\n";
		}

		while (std::chrono::duration_cast<std::chrono::seconds>(now - buffs[address].ilum[i].timestamp).count() < 60) {

			// apends the value to the response
			response.append(to_string(buffs[address].ilum[i].data));
			
			i++;

			// in case we get to the end of the buffer
			if (buffs[address].ilum.size() == i)
				break;

			response += ',';
		}

	// if requested dutyCycle
	} else if (type == 'd') {

		if (buffs[address].dutyCycle.size() == 0 && buffs[address].dutyCycle.capacity() == 0) {
			return "Address not valid!\n";
		}

		while (std::chrono::duration_cast<std::chrono::seconds>(now - buffs[address].dutyCycle[i].timestamp).count() < 60) {

			// apends the value to the response
			response.append(to_string(buffs[address].dutyCycle[i].data));

			i++;

			// in case we get to the end of the buffer
			if (buffs[address].dutyCycle.size() == i)
				break;

			response += ',';
		}
	}

	// ends with a newline delimiter
	response += '\n';

	return response;
}

// returns the state of the buffer, which means if the latest value has been streamed or not
int Database::getLastReadState(int address){
	return buffs[address].lastRead;
}

// returns the last saved values from the db
std::string Database::getStreamValues(int address, char type){

	// set up of the response string
	std::string response("c ");

	response += type;
	response += ' ';
	response.append(to_string(address));
	response += ' ';
	if (type == 'l') {
		// testing if the address requested corresponds to an arduino
		if (buffs[address].ilum.size() > 0 && buffs[address].ilum.capacity() != 0) {
			response.append(to_string(buffs[address].ilum[0].data));
		} else {
			return "Address not valid!\n";
		}
	} else if (type == 'd') {
		// testing if the address requested corresponds to an arduino
		if (buffs[address].dutyCycle.size() > 0 && buffs[address].dutyCycle.capacity() != 0) {
			response.append(to_string(buffs[address].dutyCycle[0].data));
		} else {
			return "Address not valid!\n";
		}
	}
	response += ' ';
	// tempo
	response.append(to_string(chrono::duration_cast<chrono::milliseconds>(buffs[address].ilum[0].timestamp - last_restart).count()));

	buffs[address].lastRead = 1;

	response += '\n';

	return response;
}


