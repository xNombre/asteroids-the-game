#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <chrono>

#include "shots.hpp"
#include "ship.hpp"
#include "asteroids.hpp"
#include "game.hpp"

const unsigned shoot_threshold_ms = 150;
const unsigned fastshot_detection_threshold = 100;
const unsigned fastshot_threshold_penalty = 400;
const unsigned fastshot_max_count = 5;

std::thread shots_thread;
std::mutex shots_mtx;
std::condition_variable shots_cv;
std::list <shot> shots;

std::chrono::time_point<std::chrono::system_clock> next_shot = std::chrono::system_clock::now();
unsigned cur_fastshot = 0;

static bool should_run = true;

locked_data<std::list<shot>> getShotsList()
{
	locked_data<std::list<shot>> data(shots, std::unique_lock(shots_mtx));
	return data;
}

static void shots_loop()
{
	std::unique_lock<std::mutex> lock(shots_mtx);

	while (should_run) {
		auto&& asteroids_data = get_asteroids_list();
		auto& asteroids = asteroids_data.get_data();
		for (auto it = shots.begin(); it != shots.end(); ) {
			it->y += 0.02;

			if (it->y >= 1.0) {
				it = shots.erase(it);
				continue;
			}

			bool overlap = false;
			for (auto asteroid_it = asteroids.begin(); asteroid_it != asteroids.end(); asteroid_it++) {
				for (int i = 0; i < 6; i++) {
					float asteroid_x = asteroid_it->vertices[i][0] + asteroid_it->x;
					float asteroid_y = asteroid_it->vertices[i][1] + asteroid_it->y;

					float distance = std::sqrt(std::pow(it->x - asteroid_x, 2) + std::pow(it->y - asteroid_y, 2));
					if (distance <= 0.03) { // Overlap adjustment
						overlap = true;
						break;
					}
				}

				if (overlap) {
					gameOnAsteroidShoot();
					asteroid_it = asteroids.erase(asteroid_it);
					it = shots.erase(it);
					break;
				}
			}

			if (!overlap) {
				++it;
			}
		}
		asteroids_data.release();

		if (shots.empty()) {
			shots_cv.wait(lock);
		}
		else {
			shots_cv.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(10));
		}
	}
}

void createNewShot()
{
	auto cur_time = std::chrono::system_clock::now();
	if (next_shot > cur_time) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(shots_mtx);
		shot newShot;

		auto&& ship_pos_data = getShipPosX();
		newShot.x = ship_pos_data.get_data();
		ship_pos_data.release();

		newShot.y = ship_y_pos;
		if (shots.empty()) {
			shots_cv.notify_one();
		}
		shots.push_back(newShot);
	}

	if (next_shot <= cur_time + std::chrono::milliseconds(fastshot_detection_threshold)) {
		cur_fastshot++;
	}

	if (cur_fastshot == fastshot_max_count) {
		cur_fastshot = 0;
		next_shot = cur_time + std::chrono::milliseconds(fastshot_threshold_penalty);
	}
	else {
		next_shot = cur_time + std::chrono::milliseconds(shoot_threshold_ms);
	}
}

void startShotsLoop()
{
	shots_thread = std::thread(shots_loop);
}

void stopShotsLoop()
{
	should_run = false;
	shots_thread.join();
}