CC=g++
CFLAGS=-c -Wall -std=c++11

all: SNS

SNS: amodule.o cnamemodule.o soamodule.o mxmodule.o config.o ipfilter.o dnspacket.o server.o main.o 
	g++ Build/amodule.o Build/cnamemodule.o Build/soamodule.o Build/mxmodule.o Build/config.o Build/ipfilter.o Build/dnspacket.o Build/server.o Build/main.o -o Build/SNS 

amodule.o: SNS/amodule.cpp
	$(CC) $(CFLAGS) -o Build/amodule.o SNS/amodule.cpp

cnamemodule.o: SNS/cnamemodule.cpp
	$(CC) $(CFLAGS) -o Build/cnamemodule.o SNS/cnamemodule.cpp

soamodule.o: SNS/soamodule.cpp
	$(CC) $(CFLAGS) -o Build/soamodule.o SNS/soamodule.cpp
	
mxmodule.o: SNS/mxmodule.cpp
	$(CC) $(CFLAGS) -o Builde/mxmodule.o mxmodule.cpp

config.o: SNS/config.cpp
	$(CC) $(CFLAGS) -o Build/config.o SNS/config.cpp

ipfilter.o: SNS/ipfilter.cpp
	$(CC) $(CFLAGS) -o Build/ipfilter.o SNS/ipfilter.cpp	

dnspacket.o: SNS/dnspacket.cpp
	$(CC) $(CFLAGS) -o Build/dnspacket.o SNS/dnspacket.cpp

server.o: SNS/server.cpp
	$(CC) $(CFLAGS) -o Build/server.o SNS/server.cpp

main.o: SNS/main.cpp
	$(CC) $(CFLAGS) -o Build/main.o SNS/main.cpp
	
clean:
	rm -rf Build/*.o
