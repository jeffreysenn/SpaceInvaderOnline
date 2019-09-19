#pragma once
#include <stdint.h>
struct input
{
	input();
	input(bool up, bool down, bool space, float delta_seconds);

	bool has_up() const;
	bool has_down() const;
	bool has_space() const;

	uint8_t input_;
	uint64 dt;
};

