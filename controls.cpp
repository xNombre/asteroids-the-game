#include "controls.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "shots.hpp"
#include "ship.hpp"

const int space_key = 32;

static bool controls_enabled = true;

void onKeyboardEvent(unsigned char key, int x, int y)
{
	if (!controls_enabled)
		return;

	switch (key) {
	case space_key:
		createNewShot();
		break;
	}
}

static ship_mode_dir getMoveDir(const int key)
{
	ship_mode_dir new_dir = ship_mode_dir::NONE;
	switch (key) {
	case GLUT_KEY_LEFT:
		new_dir = ship_mode_dir::LEFT;
		break;
	case GLUT_KEY_RIGHT:
		new_dir = ship_mode_dir::RIGHT;
		break;
	}

	return new_dir;
}

void onSpecialKey(int key, int x, int y)
{
	if (!controls_enabled)
		return;

	const auto new_dir = getMoveDir(key);
	if (new_dir == ship_mode_dir::NONE)
		return;

	setShipMoveDir(new_dir);
}

void onSpecialKeyUp(int key, int x, int y)
{
	if (!controls_enabled)
		return;

	const auto new_dir = getMoveDir(key);
	if (new_dir == ship_mode_dir::NONE)
		return;

	cancelShipMoveDir(new_dir);
}

void setControlsEnabled(bool enabled)
{
	controls_enabled = enabled;
}