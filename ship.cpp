#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

#include "ship.hpp"

move_dir_enum ship_move_dir = move_dir_enum::NONE;
std::thread ship_move_thread;
std::condition_variable move_dir_cond;

std::mutex ship_pos_mtx;
float x_position = 0.0f;
float triangle_speed = 0.005f;

static void move_thread()
{
	std::unique_lock<std::mutex> lock(ship_pos_mtx);
	while (ship_move_dir != move_dir_enum::NONE) {

		if (ship_move_dir == move_dir_enum::LEFT) {
			x_position -= triangle_speed / 10.0f;
			if (x_position < -1) {
				x_position = -1;
			}
		}
		else {
			x_position += triangle_speed / 10.0f;
			if (x_position > 1) {
				x_position = 1;
			}
		}

		move_dir_cond.wait_for(lock, std::chrono::microseconds(400));
	}
}

locked_data<float> getShipPosX()
{
	locked_data<float> data(x_position, std::unique_lock(ship_pos_mtx));
	return data;
}

void setShipMoveDir(move_dir_enum dir)
{
	std::unique_lock<std::mutex> lock(ship_pos_mtx);
	if (ship_move_dir == dir)
		return;
	if (ship_move_dir == move_dir_enum::NONE) {
		ship_move_thread = std::thread(move_thread);
	}

	ship_move_dir = dir;
}

void cancelShipMoveDir(move_dir_enum dir)
{
	std::unique_lock<std::mutex> lock(ship_pos_mtx);
	if (ship_move_dir == dir) {
		ship_move_dir = move_dir_enum::NONE;
		move_dir_cond.notify_one();
		lock.unlock();
		ship_move_thread.join();
	}
}
