#
#  File name: makefile
#
#  Author: 2017 Renato Henriques e Paulo Eusebio
#
#  Usage:
#     1) To generate the executable file for the program client,
#       run
#         $ make
#
#     2) To clean all object files and the executable file of
#        client, run
#         $ make clean
#

#  Compiler

CXX = g++

#  Compiler Flags
CPPFLAGS = -g -O -Wall -pedantic -std=c++11 
CFLAGS = -lpthread -lboost_system -lpigpio -lrt

#  Sources
SOURCES = serialComm.cpp tcpServer.cpp i2cComm.cpp database.cpp raspServer.cpp

server: $(SOURCES)
	$(CXX) $(CPPFLAGS) $(SOURCES) $(CFLAGS) -o server

clean::
	rm -f *.o core a.out server *~

depend::
	makedepend $(SOURCES)
# DO NOT DELETE