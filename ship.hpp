#pragma once

#include "locked_data.hpp"

enum class ship_mode_dir {
	NONE,
	LEFT,
	RIGHT
};

const float ship_y_pos = -0.8f;

extern void setShipMoveDir(ship_mode_dir dir);
extern void cancelShipMoveDir(ship_mode_dir dir);
extern locked_data<float> getShipPosX();