#pragma once

#include <list>

#include "locked_data.hpp"

struct shot {
	float x, y;

	shot():x(0), y(0) { }
};

extern void createNewShot();
extern locked_data<std::list<shot>> getShotsList();
extern void startShotsLoop();
extern void stopShotsLoop();