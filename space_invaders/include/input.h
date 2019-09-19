#pragma once
#include <stdint.h>
struct input
{
	input();
	input(bool up, bool down, bool space, uint64_t dt);

	bool has_up() const;
	bool has_down() const;
	bool has_space() const;

	uint8_t input_;
	uint64_t dt_;
};

