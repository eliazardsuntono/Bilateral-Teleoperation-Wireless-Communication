#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const char * IP_ADDRESS = "127.0.0.1";
const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
	int sockfd;
	char buffer[BUFFER_SIZE];
	sockaddr_in cliAddr, serAddr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		std::cerr << "Failed to assign socket fd" << std::endl;
		return 1;
	}

	memset(&cliAddr, 0, sizeof(cliAddr));
	memset(&serAddr, 0, sizeof(serAddr));
	
	//server config
	serAddr.sin_family = AF_INET;
	serAddr.sin_addr.s_addr = INADDR_ANY; // Listen to all interfaces
	serAddr.sin_port = htons(PORT);
	
	if (bind(sockfd, (const struct sockaddr *)&serAddr, sizeof(serAddr)) < 0) {
		std::cerr << "Error binding socket fd to server" << std::endl;
		close(sockfd);
		return 1;
	}

	std::cout << "Server is listening..." << std::endl;
	while (true) {
		socklen_t len = sizeof(cliAddr);
		int n = recvfrom(sockfd,
				(char *)buffer,
				BUFFER_SIZE - 1, 
				0, 
				(struct sockaddr *)&cliAddr, 
				&len
		);

		if (n < 0) {
			std::cerr << "Failed to recieve message" << std::endl;
			continue;
		}

		buffer[n] = '\n';
		char cliIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(cliAddr.sin_addr), cliIP, INET_ADDRSTRLEN);
		std::cout << buffer << std::endl;
	}
	
	close(sockfd);
	return 0;	
}
