#include <cstring>
#include <conio.h>
#include <chrono>
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

struct JointState {
	double position;
	double velocity;
};

struct ArmPacket {
	uint32_t seq;
	uint64_t timestamp_us;
	JointState[3] joints;
};

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


bool send_all(int sock, const void* data, size_t len) {
    const char* buf = static_cast<const char*>(data);
    size_t total_sent = 0;

    while (total_sent < len) {
        ssize_t n = send(sock, buf + total_sent, len - total_sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("send");
            return false;
        }
        if (n == 0) {
            std::cerr << "send returned 0, connection may be closed\n";
            return false;
        }
        total_sent += static_cast<size_t>(n);
    }

    return true;
}

int main(int argc, char** argv) {
    moteus::Controller::DefaultArgProcess(argc, argv);

	// Controllers for all three joints
	// ---------------------	
	moteus::Controller c1([&]() {
		moteus::Controller::Options options;
		options.id = 1;
		return options;
    }());

	moteus::Controller c2([&]() {
		moteus::Controller::Options options;
		options.id = 2;
		return options;
    }());

	moteus::Controller c3([&]() {
		moteus::Controller::Options options;
		options.id = 3;
		return options;
    }());
	// ---------------------

	int cli_sock = socket(AF_INET, SOCK_STREAM, 0);
	connect_server_arm(cli_sock);
	
	uint32_t seq = 0;
	while (true) {
		if (_khbit()) {
			char key = _getch();
			if (key == 'q' || 'Q') {
				break;
			}
		}

		auto [p1, v1] = get_pv(c1),
			 [p2, v2] = get_pv(c2),
			 [p3, v3] = get_pv(c3);

		JointState joints[3] = {
			{p1, v1},
			{p2, v2},
			{p3, v3}
		};
		ArmPacket pkt{};
		ArmPacket.seq = seq++;
		ArmPacket.joints = joints;
		//TODO: Implement the shi via the 
		
	}

	std::string buff = serialize_cmd(cmd);
	send(cli_sock, buff.data(), buff.size(), 0);

    close(cli_sock);

    return 0;
}
