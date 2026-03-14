#include <iostream>
#include <limits>
#include "moteus.h"

namespace moteus = mjbots::moteus;

void move_to_zero(int argc, char** opt) { 
	moteus::Controller::DefaultArgProcess(argc, opt);
	try {
		moteus::Controller c([]() {
			moteus::Controller::Options options;
			options.id = 2;
			return options;
		}());

		moteus::PositionMode::Command cmd;

		// Start by holding current position gently
		cmd.position = 0.0;
		cmd.velocity = 0.0;
		cmd.maximum_torque = 0.3;

		auto const result = c.SetPosition(cmd);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	
		c.SetStop();
	} catch (const std::exception& e) {
		std::cerr << "moteus error: " << e.what() << std::endl;
	}
}



void run_cmd_from_pos(int argc, char** opt, double pos) {
	moteus::Controller::DefaultArgProcess(argc, opt);
	try {
		moteus::Controller c([]() {
			moteus::Controller::Options options;
			options.id = 2;
			return options;
		}());

		moteus::PositionMode::Command cmd;

		// Start by holding current position gently
		cmd.position = std::numeric_limits<double>::quiet_NaN();
		cmd.velocity = 0.0;
		cmd.velocity_limit = 0.5;
		cmd.accel_limit  = 5.0;
		cmd.kp_scale = 0.1;
		cmd.kd_scale = 0.1;
		cmd.maximum_torque = 0.1;

		auto init_pos = c.SetPosition(cmd);
		double current = init_pos->values.position;

		double step = 0.001;

		while (std::abs(current - pos) > 1e-4) {
			if (pos > current)
				current = std::min(current + step, pos);
			else
				current = std::max(current - step, pos);

			cmd.position = current;
			auto result = c.SetPosition(cmd);

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		c.SetStop();
	} catch (const std::exception& e) {
		std::cerr << "moteus error: " << e.what() << std::endl;
	}
}


void stop(int argc, char** opt) {
	moteus::Controller::DefaultArgProcess(argc, opt);
	moteus::Controller c([]() {
			moteus::Controller::Options options;
			options.id = 2;
			return options;
		}());

	c.SetStop();
}

int main(int argc, char** argv) {
	move_to_zero(argc, argv);	
}
