#include "Collision.hpp"

#include <glm/glm.hpp>

Collision::Collision() {

}

Collision::~Collision() {

}

double fabs(double x) {
	return (((x) > 0) ? x : -x);
}

//----------------------------------------------------------------------------------------
bool Collision::carArenaCollision(Car &car, Arena &arena, bool backing) {
	glm::vec3 carPos = car.getPos();
	glm::vec3 carDir = car.getDir();
	double bounceSpeed = car.getSpeed()/1.5;

	// TODO lose kinetic energy of car
	if (!backing) {
		if (carPos.x + car.getLength()/2.0 >= arena.getLength()/2.0 && carDir.x > 0) {
			car.stop();
			car.push(glm::vec3(-carDir.x, carDir.y, carDir.z), bounceSpeed);
			return true;
		}
		else if (carPos.x - car.getLength()/2.0 <= -arena.getLength()/2.0 && carDir.x < 0) {
			car.stop();
			car.push(glm::vec3(-carDir.x, carDir.y, carDir.z), bounceSpeed);
			return true;
		}
		else if (carPos.z + car.getLength()/2.0 >= arena.getWidth()/2.0 && carDir.z > 0) {
			car.stop();
			car.push(glm::vec3(carDir.x, carDir.y, -carDir.z), bounceSpeed);
			return true;
		}
		else if (carPos.z - car.getLength()/2.0 <= -arena.getWidth()/2.0 && carDir.z < 0) {
			car.stop();
			car.push(glm::vec3(carDir.x, carDir.y, -carDir.z), bounceSpeed);
			return true;
		}
	}
	else {
		if (carPos.x + car.getLength()/2.0 >= arena.getLength()/2.0 && carDir.x < 0) {
			return true;
		}
		else if (carPos.x - car.getLength()/2.0 <= -arena.getLength()/2.0 && carDir.x > 0) {
			return true;
		}
		else if (carPos.z + car.getLength()/2.0 >= arena.getWidth()/2.0 && carDir.z < 0) {
			return true;
		}
		else if (carPos.z - car.getLength()/2.0 <= -arena.getWidth()/2.0 && carDir.z > 0) {
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------
bool Collision::ballArenaCollision(Ball &ball, Arena &arena) {
	glm::vec3 ballPos = ball.getPos();
	glm::vec3 ballDir = ball.getDir();
	double ballRadius = ball.getRadius();
	const double wall_boost = 0.02;

	// if (ballPos.x + ballRadius >= arena.getLength()/2.0) {
	// 	ball.move(glm::vec3(-ballDir.x, ballDir.y, ballDir.z), ball.getSpeed() + wall_boost);
	// 	//ball.stop();
	// 	return true;
	// }
	// if (ballPos.x - ballRadius <= -arena.getLength()/2.0) {
	// 	ball.move(glm::vec3(-ballDir.x, ballDir.y, ballDir.z), ball.getSpeed() + wall_boost);
	// 	return true;
	// }
	// if (ballPos.z + ballRadius >= arena.getWidth()/2.0) {
	// 	ball.move(glm::vec3(ballDir.x, ballDir.y, -ballDir.z), ball.getSpeed() + wall_boost);
	// 	return true;
	// }
	// if (ballPos.z - ballRadius <= -arena.getWidth()/2.0) {
	// 	ball.move(glm::vec3(ballDir.x, ballDir.y, -ballDir.z), ball.getSpeed() + wall_boost);
	// 	//std::cout << "ball top" << std::endl;
	// 	return true;
	// }

	return false;
}

//----------------------------------------------------------------------------------------
bool Collision::carBallCollision(Car &car, Ball &ball) {
	glm::vec3 carPos = car.getPos();
	glm::vec3 ballPos = ball.getPos();
	double e = (ball.getRadius() + car.getLength()/2.0) * (ball.getRadius() + car.getLength()/2.0);

	double deltaX = fabs(carPos.x - ballPos.x);
	double deltaZ = fabs(carPos.z - ballPos.z);
	double dis = deltaX * deltaX + deltaZ * deltaZ;
	if (dis <= e) {
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------
bool Collision::carCarCollision(Car &car, Car &car2) {
	glm::vec3 carPos = car.getPos();
	glm::vec3 car2Pos = car2.getPos();

	double deltaX = fabs(carPos.x - car2Pos.x);
	double deltaZ = fabs(carPos.z - car2Pos.z);
	double e = car.getLength() * car.getLength();

	double dis = deltaX * deltaX + deltaZ * deltaZ;
	if (dis <= e) {
		glm::vec3 carDir = car.getDir();
		glm::vec3 car2Dir = car2.getDir();
		double carSpeed = car.getSpeed();
		double car2Speed = car2.getSpeed();

		car.stop();
		car2.stop();

		car2.push(carDir, carSpeed);
		car.push(car2Dir, car2Speed);

		return true;
	}

	return false;
}
