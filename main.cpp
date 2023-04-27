
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

std::mutex ship_pos_mtx;
float x_position = 0.0f;
float y_position = -0.8f;

float triangle_speed = 0.005f;

int window_width = 800;
int window_height = 600;

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

void specialKeys(int key, int x, int y)
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

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Asteroids - The Game");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutSpecialFunc(specialKeys);
	glutSpecialUpFunc(specialKeysUp);
	glutMainLoop();

	return 0;
}
