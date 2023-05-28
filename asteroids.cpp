#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>

#include "asteroids.hpp"
#include "game.hpp"
#include "ship.hpp"

const float polygonRadius = 0.4f;
const float maxDeformationDist = 0.2f;

std::mt19937 e2(std::random_device{}());
std::uniform_real_distribution<float> uniform_dist(-1.0, 1.0);

std::mutex asteroids_mtx;
std::condition_variable asteroids_cv;
std::thread asteroids_thread;
std::list<asteroid> asteroids;
unsigned asteroids_sleep_us = 10000;

std::thread asteroids_generator_thread;
std::condition_variable asteroids_generator_cv;
std::mutex asteroids_generator_mtx;
unsigned generator_delay = 3000;

static bool should_run = true;

locked_data<std::list<asteroid>> get_asteroids_list()
{
	locked_data<std::list<asteroid>> data(asteroids, std::unique_lock(asteroids_mtx));
	return data;
}

static float randDeform()
{
	return ((float)rand() / RAND_MAX) * maxDeformationDist * 2 - maxDeformationDist;
}

static void generateRandomPolygon()
{
	asteroid newAsteroid;

	for (int i = 0; i < 6; i++) {
		newAsteroid.x = uniform_dist(e2);
		newAsteroid.y = 1.2;

		float angle = i * 2 * M_PI / 6; // angle of current vertex
		float x = polygonRadius * cos(angle) + randDeform();
		float y = polygonRadius * sin(angle) + randDeform();
		newAsteroid.vertices[i][0] = x / 5.f;
		newAsteroid.vertices[i][1] = y / 5.f;
	}
	{
		std::lock_guard<std::mutex> lock(asteroids_mtx);
		if (asteroids.empty()) {
			asteroids_cv.notify_one();
		}
		asteroids.push_back(newAsteroid);
	}
}

static void asteroids_loop()
{
	std::unique_lock<std::mutex> lock(asteroids_mtx);

	while (should_run) {
		auto&& ship_pos_data = getShipPosX();
		const auto& ship_x_pos = ship_pos_data.get_data();
		for (auto it = asteroids.begin(); it != asteroids.end(); ) {
			it->y -= 0.002;

			if (it->y <= -1.2) {
				it = asteroids.erase(it);
				gameOnAsteroidPass();
				continue;
			}

			bool overlap = false;
			for (int i = 0; i < 6; i++) {
				float asteroid_x = it->vertices[i][0] + it->x;
				float asteroid_y = it->vertices[i][1] + it->y;

				float distance = std::sqrt(std::pow(ship_x_pos - asteroid_x, 2) +
										   std::pow(ship_y_pos - asteroid_y, 2));
				if (distance <= 0.1) {
					overlap = true;
					break;
				}
			}

			if (overlap) {
				gameOnEndGame();
			}

			it++;
		}
		ship_pos_data.release();

		if (asteroids.empty()) {
			asteroids_cv.wait(lock);
		}
		else {
			asteroids_cv.wait_until(lock, std::chrono::system_clock::now() +
									std::chrono::microseconds(asteroids_sleep_us));
		}
	}
}

static void asteroids_generator_loop()
{
	while (should_run) {
		std::unique_lock<std::mutex> lock(asteroids_generator_mtx);

		generateRandomPolygon();

		generator_delay *= 0.9;
		std::uniform_int_distribution random_time(-400, 400);
		generator_delay += random_time(e2);
		if (generator_delay <= 150)
			generator_delay = 400;

		asteroids_sleep_us = (double)asteroids_sleep_us * 0.98;

		asteroids_generator_cv.wait_until(lock, std::chrono::system_clock::now() +
										  std::chrono::milliseconds(generator_delay));
	}
}

void stopAsteroidsThreads()
{
	{
		std::lock(asteroids_mtx, asteroids_generator_mtx);
		std::unique_lock<std::mutex> lock(asteroids_mtx, std::adopt_lock);
		std::unique_lock<std::mutex> lock2(asteroids_generator_mtx, std::adopt_lock);
		should_run = false;

		asteroids_generator_cv.notify_one();
		asteroids_cv.notify_one();
	}

	asteroids_generator_thread.join();
	asteroids_thread.join();
}

void startAsteroidsThreads()
{
	should_run = true;

	asteroids_thread = std::thread(asteroids_loop);
	asteroids_generator_thread = std::thread(asteroids_generator_loop);
}