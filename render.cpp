#include "render.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "controls.hpp"
#include "asteroids.hpp"
#include "ship.hpp"
#include "shots.hpp"

const int window_width = 800;
const int window_height = 600;
const float scale = 10.0f;

static void onWindowResize(int w, int h)
{
	glutReshapeWindow(window_width, window_height);
}

static void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.0f);

	float x_position;
	{
		auto&& ship_pos_data = getShipPosX();
		x_position = ship_pos_data.get_data();
	}
	glTranslatef(x_position, ship_y_pos, 0.0f);
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
		auto&& asteroids = get_asteroids_list();

		glColor3f(1.0f, 1.0f, 1.0f);
		for (auto element : asteroids.get_data()) {
			glLoadIdentity();
			glTranslatef(element.x, element.y, 0.0f);
			glBegin(GL_POLYGON);
			for (int i = 0; i < 6; i++) {
				glVertex2fv(element.vertices[i]);
			}
			glEnd();
		}
	}

	{
		auto&& shot_data = getShotsList();
		for (auto element : shot_data.get_data()) {
			glLoadIdentity();
			glTranslatef(element.x, element.y, 0.0f);
			glBegin(GL_POLYGON);

			glVertex2f(0.05f / scale, 0.1f / scale);
			glVertex2f(-0.05f / scale, 0.1f / scale);
			glVertex2f(-0.05f / scale, -0.1f / scale);
			glVertex2f(0.05f / scale, -0.1f / scale);
			glEnd();
		}
	}

	glFlush();
	glutPostRedisplay();

	glutSwapBuffers();
}

void init_glut()
{
	int argc = 0;
	glutInit(&argc, nullptr);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("");
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glutDisplayFunc(display);
	glutReshapeFunc(onWindowResize);
	glutKeyboardFunc(onKeyboardEvent);
	glutSpecialFunc(onSpecialKey);
	glutSpecialUpFunc(onSpecialKeyUp);
}

void glut_start_loop()
{
	glutMainLoop();
}

void set_window_title(const char* title)
{
	glutSetWindowTitle(title);
}