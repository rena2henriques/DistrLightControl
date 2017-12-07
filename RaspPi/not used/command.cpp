#include "command.h"

string Command::getCommand(char message[]) {

	cout << "Get command " << message << endl;

	return "Work in progress";
}


string Command::setCommand(char message[]) {

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

	printf("%s %s\n", desk, flag); // test

	//sendSerialMsg(desk, flag);

	return "Work in progress";
}

string Command::restartCommand() {

	// sends a restart flag to the main arduino 

	//sendSerialMsg("r");

	return "ack";
}

string Command::streamCommand() {



	return "Work in progress";
}