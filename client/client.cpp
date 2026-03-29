#include <cstring>
#include <chrono>
#include <cstdint>
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "moteus.h"
#include "../send.hpp"

using json = nlohmann::json;

uint64_t now_us() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

struct JointState {
	double position;
	double velocity;
};

struct ArmPacket {
	uint32_t seq;
	uint64_t timestamp_us;
	JointState joints[3];
};

bool send_all(int sock, const void* data, size_t len) {
    const char* buf = static_cast<const char*>(data);
    size_t total = 0;

    while (total < len) {
        ssize_t n = send(sock, buf + total, len - total, 0);

        if (n < 0) {
            if (errno == EINTR) continue;
            perror("send failed");
            return false;
        }

        if (n == 0) {
            std::cerr << "connection closed\n";
            return false;
        }

        total += n;
    }

    return true;
}

bool send_packet(int sock, const ArmPacket& pkt) {
    return send_all(sock, &pkt, sizeof(pkt));
}

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
		const auto& [p1, v1] = get_pv(c1);
		const auto& [p2, v2] = get_pv(c2);
		const auto& [p3, v3] = get_pv(c3);

		ArmPacket pkt{};
		pkt.seq = seq++;
		pkt.joints[0] = {p1, v1} ;
		pkt.joints[1] = {p2, v2} ;
		pkt.joints[2] = {p3, v3} ;
		pkt.timestamp_us = now_us();	

		if (!send_packet(cli_sock, pkt)) {
			std::cerr << "Failed to send information" << std::endl;
		}				
	}

    close(cli_sock);

    return 0;
}
