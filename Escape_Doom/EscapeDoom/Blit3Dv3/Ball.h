#pragma once

#include "Blit3D.h"

class Ball
{
private:
	float density = 1.f;
public:
	Sprite* sprite = nullptr;
	glm::vec2 position;
	glm::vec2 velocity;
	float radius = 0.f;
	float angle = 0; //ball rotation in degrees
	float spin = 0; //speed of rotation of the ball in degrees/sec
	float mass = 1.f;

	Ball(Sprite* ImageSprite, float Radius, float Density);
	void Draw();
	void Update(float seconds);
};

//return TRUE if there was a collision
bool CollideBalls(Ball* A, Ball* B);

void PositionBall(Ball* ball);