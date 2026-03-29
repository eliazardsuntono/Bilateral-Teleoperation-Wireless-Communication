#include <cstdlib>
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>
#include <tuple>
#include "moteus.h"

namespace moteus = mjbots::moteus;

std::tuple<double, double> get_pv(moteus::Controller c) {
	try {

		moteus::PositionMode::Command cmd;
		cmd.position = std::numeric_limits<double>::quiet_NaN();
		
		const auto res = c.SetPosition(cmd);

		if (res) {
			std::tuple<double, double> rv{
				res->values.position, res->values.velocity};
			return rv; 
		} else {
			c.SetStop();
			std::cerr << "failed to get position" << std::endl;
			std::exit(EXIT_FAILURE);
		}

	} catch (const std::exception& e) {
		std::cerr << "moteus error: " << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void move_slowly(moteus::Controller c, double c_pos, double t_pos) {
	try {
		moteus::PositionMode::Command cmd;

		const double step = 0.01;
		const int iterations = 10000;

		for (int i = 0; i < iterations; ++i) {
			if (c_pos >= t_pos) {
				std::cout << "Successfully moved to target position" << std::endl;	
				break;
			}

			c_pos += step;
			cmd.position = c_pos;
			cmd.velocity = 0.0;
			cmd.maximum_torque = 0.2;

			const auto res = c.SetPosition(cmd);
			if (res) {
				const auto& v = res->values;
				std::cout << "Position: " << v.position << std::endl
						  << "Velocity: " << v.velocity << std::endl;
			} else {
				std::cerr << "failed to move slowly to position via iteration" << std::endl;
				c.SetStop();
				std::exit(EXIT_FAILURE);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

		c.SetStop();
  } catch (const std::exception& e) {
	std::cerr << "moteus error: " << e.what() << std::endl;
  }
}


void fail_stop(int argc, char** opt) {
	moteus::Controller c([]() {
			moteus::Controller::Options options;
			options.id = 2;
			return options;
		}());

	c.SetStop();
}
