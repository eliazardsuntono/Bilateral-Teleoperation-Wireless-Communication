#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "moteus.h"
#include "../send.hpp"

using json = nlohmann::json;

json deserialize(std::string cmd) {
	json res;	
	return res.parse(cmd);	
}


void listen_master_arm(int s_sock) {
	sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(8080);
	s_addr.sin_addr.s_addr = INADDR_ANY;

	bind(s_sock, (struct sockaddr*)&s_addr,
			sizeof(s_addr));

	listen(s_sock, 5);
}


int main(int argc, char** argv) {
	moteus::Controller::DefaultArgProcess(argc, argv);

	moteus::Controller c([&]() {
		moteus::Controller::Options options;
		options.id = 2;
		return options;
    }());

	int s_sock = socket(AF_INET, SOCK_STREAM, 0);
	listen_master_arm(s_sock);
	
	int cli_sock = accept(s_sock, nullptr, nullptr);
	
	std::string buffer;
	buffer.resize(1024);
	ssize_t bytesReceived = recv(cli_sock, buffer.data(), buffer.size(), 0);

	if (bytesReceived > 0) {
		buffer.resize(bytesReceived);  // trim to actual size
	    std::cout << "Received: " << buffer << std::endl;
	}
	
	json cmd = deserialize(buffer);
	double c_pos = cmd["position"];	
	double t_pos = c_pos + 5; 
	
	move_slowly(c, c_pos, t_pos);

	close(s_sock);
	return 0;
}
