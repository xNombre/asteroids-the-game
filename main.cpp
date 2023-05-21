
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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

std::mutex ship_pos_mtx;
float x_position = 0.0f;
float y_position = -0.8f;

float triangle_speed = 0.005f;

int window_width = 800;
int window_height = 600;

struct asteroid {
	float vertices[6][2];
	float x, y;

	asteroid():x(0), y(0)
	{
		memset(vertices, 0, sizeof(vertices));
	}
};

struct shot {
	float x, y;

	shot():x(0), y(0) { }
};

std::mutex asteroids_mtx;
std::mutex shots_mtx;

std::mt19937 e2(std::random_device{}());
const float radius = 0.4f; // radius of the polygon
const float randRange = 0.2f; // maximum deformation distance
std::list <asteroid> asteroids;
std::list <shot> shots;

// Function to generate a random deformation for a vertex
float randDeform()
{
	return ((float)rand() / RAND_MAX) * randRange * 2 - randRange;
}

// Function to generate a new polygon with deformed vertices
void generateRandomPolygon()
{
	asteroid newAsteroid;

	for (int i = 0; i < 6; i++) {

		std::uniform_real_distribution<float> uniform_dist(-1.0, 1.0);
		newAsteroid.x = uniform_dist(e2);
		newAsteroid.y = 1.2;

		float angle = i * 2 * M_PI / 6; // angle of current vertex
		float x = radius * cos(angle) + randDeform();
		float y = radius * sin(angle) + randDeform();
		newAsteroid.vertices[i][0] = x / 5.f;
		newAsteroid.vertices[i][1] = y / 5.f;
	}
	{
		std::unique_lock<std::mutex> lock(asteroids_mtx);
		asteroids.push_back(newAsteroid);
	}
}

std::chrono::time_point<std::chrono::system_clock> next_shot = std::chrono::system_clock::now();
const unsigned shoot_threshold_ms = 150;
const unsigned fastshot_detection_threshold = 100;
const unsigned fastshot_threshold_penalty = 700;
const unsigned fastshot_max_count = 5;
unsigned cur_fastshot = 0;

void shoot()
{
	auto cur_time = std::chrono::system_clock::now();
	if (next_shot > cur_time) {
		return;
	}

	shot newShot;
	newShot.x = x_position;
	newShot.y = y_position;
	shots.push_back(newShot);

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

void display()
{
	float scale = 10.0f;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.0f);

	{
		std::lock_guard<std::mutex> lock(ship_pos_mtx);
		auto x_position_swp = x_position;
	}
	glTranslatef(x_position, y_position, 0.0f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f / scale, 1.0f / scale);
	glVertex2f(-0.6f / scale, -0.6f / scale);
	glVertex2f(0.0f / scale, -0.3f / scale);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f / scale, 1.0f / scale);
	glVertex2f(0.6f / scale, -0.6f / scale);
	glVertex2f(0.0f / scale, -0.3f / scale);
	glEnd();

	{
		std::unique_lock<std::mutex> lock(asteroids_mtx);

		glColor3f(1.0f, 1.0f, 1.0f);
		for (auto element : asteroids) {
			glLoadIdentity();
			glTranslatef(element.x, element.y, 0.0f);
			glBegin(GL_POLYGON);
			for (int i = 0; i < 6; i++) {
				glVertex2fv(element.vertices[i]);
			}
			glEnd();
		}
	}

	for (auto element : shots) {
		glLoadIdentity();
		glTranslatef(element.x, element.y, 0.0f);
		glBegin(GL_POLYGON);

		glVertex2f(0.05f / scale, 0.1f / scale);
		glVertex2f(-0.05f / scale, 0.1f / scale);
		glVertex2f(-0.05f / scale, -0.1f / scale);
		glVertex2f(0.05f / scale, -0.1f / scale);
		glEnd();
	}

	glFlush();
	glutPostRedisplay();

	glutSwapBuffers();
}

void resize(int w, int h)
{
	glutReshapeWindow(window_width, window_height);
}

void move_thread();
enum class move_dir_enum {
	NONE,
	LEFT,
	RIGHT
};
move_dir_enum ship_move_dir = move_dir_enum::NONE;
std::thread ship_move_thread;
std::condition_variable move_dir_cond;

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 32:
		shoot();
		break;
	}
}

void specialKeys(int key, int x, int y)
{
	move_dir_enum new_dir = move_dir_enum::NONE;
	switch (key) {
	case GLUT_KEY_LEFT:
		new_dir = move_dir_enum::LEFT;
		break;
	case GLUT_KEY_RIGHT:
		new_dir = move_dir_enum::RIGHT;
		break;
	case 116: {
		generateRandomPolygon();
		return;
		break;
	}
	}

	if (new_dir == move_dir_enum::NONE)
		return;

	std::unique_lock<std::mutex> lock(ship_pos_mtx);
	if (ship_move_dir == new_dir)
		return;
	if (ship_move_dir == move_dir_enum::NONE) {
		ship_move_thread = std::thread(move_thread);
	}

	ship_move_dir = new_dir;
}


void specialKeysUp(int key, int x, int y)
{
	move_dir_enum new_dir = move_dir_enum::NONE;
	switch (key) {
	case GLUT_KEY_LEFT: {
		new_dir = move_dir_enum::LEFT;
		break;
	}
	case GLUT_KEY_RIGHT:
		new_dir = move_dir_enum::RIGHT;
		break;
	}

	if (new_dir == move_dir_enum::NONE)
		return;

	std::unique_lock<std::mutex> lock(ship_pos_mtx);
	if (ship_move_dir == new_dir) {
		ship_move_dir = move_dir_enum::NONE;
		move_dir_cond.notify_one();
		lock.unlock();
		ship_move_thread.join();
	}
}

void move_thread()
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

std::thread asteroids_thread;
std::condition_variable asteroids_cv;
bool should_run = true;

void asteroids_loop()
{
	while (should_run) {
		std::unique_lock<std::mutex> lock(asteroids_mtx);

		for (auto& elem : asteroids) {
			elem.y -= 0.002;
		}

		asteroids_cv.wait_for(lock, std::chrono::milliseconds(10));
	}
}

std::thread shots_thread;
std::condition_variable shots_cv;

void shots_loop()
{
	while (should_run) {
		std::unique_lock<std::mutex> lock(shots_mtx);

		for (auto& elem : shots) {
			elem.y += 0.02;
		}

		shots_cv.wait_for(lock, std::chrono::milliseconds(10));
	}
}

std::thread asteroids_generator_thread;
std::condition_variable asteroids_generator_cv;
int generator_delay = 3000;

void asteroids_generator_loop()
{
	while (should_run) {
		std::unique_lock<std::mutex> lock(shots_mtx);

		generateRandomPolygon();

		generator_delay *= 0.9;
		//std::cout << "delay " << generator_delay << std::endl;
		std::uniform_int_distribution random_time(-400, 400);
		generator_delay += random_time(e2);
		if (generator_delay <= 150)
			generator_delay = 400;

		asteroids_generator_cv.wait_for(lock, std::chrono::milliseconds(generator_delay));
	}
}

void close()
{
	should_run = false;
	asteroids_generator_cv.notify_one();
	asteroids_thread.join();
	shots_thread.join();
	asteroids_generator_thread.join();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Asteroids - The Game");
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	asteroids_thread = std::thread(asteroids_loop);
	asteroids_generator_thread = std::thread(asteroids_generator_loop);
	shots_thread = std::thread(shots_loop);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutSpecialUpFunc(specialKeysUp);
	atexit(close);
	glutMainLoop();

	return 0;
}