#include "database.h"

Database::Database() {


}

Database::Database(int capacity_) : capacity(capacity_) {
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
	}

	// insert the data in the circular buffers
	buffs[address].ilum.push_front(lux);
	buffs[address].dutyCycle.push_front(dc);

	return;
}


void Database::clearBuffers(){

	for(int i = 0; i < 127; i++) {
		buffs[i].ilum.clear();
		buffs[i].dutyCycle.clear();
	}

}

void Database::printBuffers(int address){

	//for(int i = 0; i < buffs[address].ilum.size(); i++) {
	if (buffs[address].ilum.size() > 0 && buffs[address].dutyCycle.size() > 0) {
		cout << "lux: " << buffs[address].ilum[0] << ", dc: " <<  buffs[address].dutyCycle[0] << " of address " << address << endl;
	}

}


std::string Database::getCurrentValues(char message[]) {

	char request = 'z';
	int address = -1;
	int occup;
	char aux_response[20];

	if( sscanf(message, "%c %d %d", &request, &address, &occup) != 3)
		return "Message Invalid\n"; // message hasnt been sent correctly

	if (request == 'l'){
		if (buffs[address].ilum.size() > 0 ) {

			snprintf(aux_response, 20, "l %d %.2f", address, buffs[address].ilum[0]);

		}

	} else if (request == 'd') {

		if (buffs[address].dutyCycle.size() > 0 ) {
			snprintf(aux_response, 20, "l %d %.2f", address, buffs[address].dutyCycle[0]);
		}
	}

	std::string response(aux_response);

	response += '\n';

	return response;
}