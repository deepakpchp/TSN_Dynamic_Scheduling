// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <fstream>
#include <iostream>
#define PORT 8080 

using namespace std;
int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}
	int input;
	std::cout<<"You are going to stop the dynamic scheduler\n";
	std::cout<<"Press enter to continue or Ctrl+c to abort\n";
	std::cin>>input;

	const char* buffer = "STOP_REQUEST";
	send(sock , buffer , strlen(buffer) , 0 );

	char ret_buffer[100];
	valread = read( sock , ret_buffer, 1024); 
	char *output = NULL;
    output = strstr (ret_buffer, "Disconnect");
	if (output){
		std::cout<<"Successfully stoped Dynamic Scheduler\n";
	}
	close(sock);
	return 0; 
} 

