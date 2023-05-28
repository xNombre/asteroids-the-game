#pragma once

#include <cstring>
#include <list>

#include "locked_data.hpp"

struct asteroid {
	float vertices[6][2];
	float x, y;

	asteroid()
		: x(0), y(0)
	{
		memset(vertices, 0, sizeof(vertices));
	}
};

extern locked_data<std::list<asteroid>> get_asteroids_list();
extern void stopAsteroidsThreads();
extern void startAsteroidsThreads();