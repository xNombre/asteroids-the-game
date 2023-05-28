#include "controls.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "shots.hpp"
#include "ship.hpp"

const int space_key = 32;

void onKeyboardEvent(unsigned char key, int x, int y)
{
	switch (key) {
	case space_key:
		createNewShot();
		break;
	}
}

void onSpecialKey(int key, int x, int y) {
	move_dir_enum new_dir = move_dir_enum::NONE;
	switch (key) {
	case GLUT_KEY_LEFT:
		new_dir = move_dir_enum::LEFT;
		break;
	case GLUT_KEY_RIGHT:
		new_dir = move_dir_enum::RIGHT;
		break;
	}

	if (new_dir == move_dir_enum::NONE)
		return;

	setShipMoveDir(new_dir);
}

void onSpecialKeyUp(int key, int x, int y) {
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

	cancelShipMoveDir(new_dir);
}