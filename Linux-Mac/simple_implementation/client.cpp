#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const char * IP_ADDRESS = "127.0.0.1";
const int PORT = 8080;

int main() {
	int clientSocket = socket(AF_INET, SOCK_DGRAM, 0); // IPv4, UDP, Default protocol
	if (clientSocket < 0) {
		std::cerr << "Error initializing socket" << std::endl;
		close(clientSocket);
		return 1;
	}

	// server address config
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	
	if (inet_pton(AF_INET, IP_ADDRESS, &address.sin_addr) <= 0) {
		std::cerr << "Converting info to bytes failed" << std::endl;
		close(clientSocket);
		return 1;
	}

	std::string msg = "Hello World";
	const char * buffer = msg.c_str(); // Data buffer to send 
	
	ssize_t bytesSent = sendto(
			clientSocket,
			buffer,
			msg.length(),
			0,
			(struct sockaddr *)&address,
			sizeof(address)
	);

	if (bytesSent < 0) {
		std::cerr << "Failed to send message" << std::endl;
	} else {
		std::cout << "Message sent" << std::endl;
	}

	close(clientSocket);
	return 0;
}
