#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <random>
#include <vector>
#include <list>
#include <cstring>
#include <chrono>

#include "render.hpp"
#include "controls.hpp"
#include "asteroids.hpp"
#include "shots.hpp"

const int bonus_for_asteroid_shoot = 10;
const int penalty_for_passed_asteroid = -35;

int score = 0;

std::chrono::time_point<std::chrono::system_clock> last_ast_shoot = std::chrono::system_clock::now();
const unsigned shoot_combo_threshold_ms = 80;

bool should_run = true;

static void update_points()
{
	std::string title = "Asteroids  -  The Game  -  Score: ";
	title += std::to_string(score);

	set_window_title(title.c_str());
}

void gameOnAsteroidPass()
{
	score += penalty_for_passed_asteroid;
	update_points();
}

void gameOnAsteroidShoot()
{
	auto cur_time = std::chrono::system_clock::now();
	if (cur_time <= last_ast_shoot + std::chrono::milliseconds(shoot_combo_threshold_ms)) {
		score += bonus_for_asteroid_shoot;
	}
	last_ast_shoot = cur_time;
	score += bonus_for_asteroid_shoot;

	update_points();
}

void endGame()
{

}

void close()
{
	should_run = false;



}

int main(int argc, char** argv)
{
	init_glut();
	
	startShotsLoop();
	start();
	update_points();
	
	atexit(close);

	start_loop();

	return 0;
}