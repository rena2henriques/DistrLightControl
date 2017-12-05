#ifndef COMMAND_H
#define COMMAND_H

#include <cstdlib>
#include <iostream>
#include <string.h>
using namespace std;

class Command {
private:


public:

	// for commands of the type 'g'
	string getCommand(char message[]);

	// for commands of the type 's'
	string setCommand(char message[]);

	// for commands of the type 'r'
	string restartCommand();

	// other commands
	string streamCommand();

};


#endif
