#pragma once

#include "locked_data.hpp"

enum class move_dir_enum {
	NONE,
	LEFT,
	RIGHT
};

const float ship_y_pos = -0.8f;

extern void setShipMoveDir(move_dir_enum dir);
extern void cancelShipMoveDir(move_dir_enum dir);
extern locked_data<float> getShipPosX();