#pragma once

#include "Car.hpp"
#include "Arena.hpp"
#include "Ball.hpp"

class Collision {
public:
	Collision();
	~Collision();

	bool carArenaCollision(Car &car, Arena &arena, bool backing);
	bool ballArenaCollision(Ball &ball, Arena &arena);
	bool carBallCollision(Car &car, Ball &ball);
	bool carCarCollision(Car &car, Car &car2);
};