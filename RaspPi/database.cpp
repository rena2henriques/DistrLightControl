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