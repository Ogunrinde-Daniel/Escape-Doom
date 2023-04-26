#pragma once
#include "Blit3D.h"
#include "Ball.h"

class Player
{
public:
	Sprite* sprite;
	glm::vec2 position, velocity;
	float speed;

	float width;
	float height;
	float radius;
	float mass;
	int lives = 3;
	float collisionTimer = 2.f;

	bool movingUp	 = false;
	bool movingDown  = false;
	bool movingLeft  = false;
	bool movingRight = false;
	bool thrusting	 = false;

	Player(glm::vec2 tempPosition, float tempWidth, float tempHeight);
	
	void draw();
	void update(float seconds);

};

float DistanceSquared(glm::vec2 pos1, glm::vec2 pos2);

bool Collision(Ball *b, Player *p);
