#include <chrono>
#include <atomic>
#include <thread>

#include "render.hpp"
#include "controls.hpp"
#include "asteroids.hpp"
#include "shots.hpp"
#include "ship.hpp"

enum class stop_type {
	NONE,
	GAME_LOSS,
	EXIT
};

const int bonus_for_asteroid_shoot = 10;
const int penalty_for_passed_asteroid = -2;
const unsigned shoot_combo_threshold_ms = 100;

int score = 0;

std::atomic<stop_type> should_end_game = stop_type::NONE;
std::thread endGameHandlerThread;

std::chrono::time_point<std::chrono::system_clock> last_ast_shoot = std::chrono::system_clock::now();

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

static void gameEndHandler()
{
	should_end_game.wait(stop_type::NONE);

	stopAsteroidsThreads();
	stopShotsThread();

	if (should_end_game != stop_type::EXIT) {
		std::string title = "Asteroids  -  GAME OVER  -  Score: ";
		title += std::to_string(score);
		set_window_title(title.c_str());
	}

	setShipMoveDir(ship_mode_dir::NONE);
}

void gameOnEndGame()
{
	should_end_game = stop_type::GAME_LOSS;
	should_end_game.notify_all();
}

static void close()
{
	should_end_game = stop_type::EXIT;
	should_end_game.notify_all();
	endGameHandlerThread.join();
}

static void gameStartGame()
{
	startShotsThread();
	startAsteroidsThreads();
	update_points();
}

int main(int argc, char** argv)
{
	endGameHandlerThread = std::thread(gameEndHandler);
	atexit(close);
	init_glut();
	gameStartGame();

	glut_start_loop();

	return 0;
}