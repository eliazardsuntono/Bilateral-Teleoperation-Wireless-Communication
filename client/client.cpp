#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include "moteus.h"
#include "../send.hpp"

using json = nlohmann::json;

std::string serialize_cmd(json cmd) {
	return cmd.dump();
}


void connect_server_arm(int cli_sock) {
	sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8080);
    s_addr.sin_addr.s_addr = INADDR_ANY;

    connect(cli_sock, (struct sockaddr*)&s_addr,
            sizeof(s_addr));
}


int main(int argc, char** argv) {
    moteus::Controller::DefaultArgProcess(argc, argv);

	moteus::Controller c([&]() {
		moteus::Controller::Options options;
		options.id = 2;
		return options;
    }());

	int cli_sock = socket(AF_INET, SOCK_STREAM, 0);
	connect_server_arm(cli_sock);

	double c_pos = get_position(c);
	json cmd = {
		{"position", c_pos},
		{"velocity", 0.0},
		{"maximum_torque", 0.2}
	};

	std::string buff = serialize_cmd(cmd);
	send(cli_sock, buff.data(), buff.size(), 0);

    close(cli_sock);

    return 0;
}
