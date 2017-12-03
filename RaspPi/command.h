#ifndef COMMAND_H
#define COMMAND_H

#include <cstdlib>
#include <iostream>
using namespace std;

class Command {
private:


public:

	// for commands of the type 'g'
	void getCommand(char message[]);

	// for commands of the type 's'
	void setCommand();

	// for commands of the type 'r'
	void restartCommand();

	// other commands
	void streamCommand();

};


#endif
