#include "input.h"

input::input()
	: input_(0)
	, dt_(0)
{
}

input::input(bool up, bool down, bool space, uint64_t dt)
	: input_((up << 0) | (down << 1) | (space << 2))
	, dt_(dt)
{
}

bool input::has_up() const
{
	return (input_ & (1 << 0)) > 0;
}

bool input::has_down() const
{
	return (input_ & (1 << 1)) > 0;
}

bool input::has_space() const
{
	return (input_ & (1 << 2)) > 0;
}