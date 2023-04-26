#include "player.h"


void Player::draw()
{
	if (collisionTimer > 0)
		sprite->Blit(position.x, position.y, 1.f, 1.f, 0.75f);
	else
		sprite->Blit(position.x, position.y);

}

void Player::update(float seconds)
{
	//thrusting
	if (thrusting)
	{
		if (movingUp)
			position.y += speed*2;
		if (movingDown)
			position.y -= speed*2;
		if (movingLeft)
			position.x -= speed*2;
		if (movingRight)
			position.x += speed*2;
	}


	//movement
	if (movingUp) 
		position.y += speed;
	if (movingDown)
		position.y -= speed;
	if (movingLeft)
		position.x -= speed;
	if (movingRight)
		position.x += speed;

	

	//boundaries check
	//left
	if (position.x < 0 + (width / 2))
		position.x = (width / 2);
	//right
	if (position.x > 1920 - (width / 2))
		position.x = 1920 - (width / 2);
	//up
	if (position.y > 1080 - (height / 2))
		position.y = 1080 - (height / 2);
	//down
	if (position.y < 0 + (height / 2))
		position.y =  (height / 2);

	if (collisionTimer > 0)
		collisionTimer -= seconds;
	
	
}

Player::Player(glm::vec2 tempPosition, float tempWidth, float tempHeight)
{
	position = tempPosition;
	width	 = tempWidth;
	height	 = tempHeight;
	radius   = tempWidth / 2;
	velocity = glm::vec2(5.f, 100.f);
	speed	 = velocity.x;
	mass	 = 60000;
}

float DistanceSquared(glm::vec2 pos1, glm::vec2 pos2)
{
	return (pos1.x - pos2.x) * (pos1.x - pos2.x) +
		(pos1.y - pos2.y) * (pos1.y - pos2.y);
}

bool Collision(Ball *b, Player* p)
{
	if (p->collisionTimer > 0)
		return false;
		
	bool collision = false;
	glm::vec2 line = b->position - p->position;

	//check cheap, fast box2box collision
	float Aleft = b->position.x - b->radius;
	float Aright = b->position.x + b->radius;
	float Atop = b->position.y + b->radius;
	float Abottom = b->position.y - b->radius;

	float Bleft = p->position.x - p->radius;
	float Bright = p->position.x + p->radius;
	float Btop = p->position.y + p->radius;
	float Bbottom = p->position.y - p->radius;

	if (!(Aleft > Bright || Aright < Bleft || Atop < Bbottom || Abottom > Btop))
	{
		//the bounding boxes collided, but not neccessarily the enclosed circles
		// NOW we do more expensive, proper circle2circel collision

		//calculate the distance between the centers of the two balls
		float length = glm::length(line);

		if (length < b->radius + p->radius)
		{
			//collision!
			collision = true;
			//update velocities

			//calculate the vector from center-to-center of the balls
			glm::vec2 normalVector = glm::normalize(line);

			//to preserve momentum, use the center-of-mass frame (https://en.wikipedia.org/wiki/Center-of-momentum_frame)
			glm::vec2 vCenterMass = (b->velocity * b->mass + p->velocity * p->mass) / (b->mass + p->mass);

			//This is the velocity of an observer to whom the two balls seem to have equal 
			// and opposite momenta.
			//The velocities in that observer's frame are:
			b->velocity -= vCenterMass;
			p->velocity -= vCenterMass;

			//reflect the velocities off the plane tangent to the collison 
			b->velocity = glm::reflect(b->velocity, normalVector);
			p->velocity = glm::reflect(p->velocity, normalVector);

			//adjust back from the observer's frame to the original frame
			b->velocity += vCenterMass;
			p->velocity += vCenterMass;

			//calculate overlap
			float overlap = (b->radius + p->radius) - length;
			overlap = overlap / 2.f;

			//moves the balls apart by the half overlap - cheesy hack
			b->position += normalVector * overlap;
			p->position -= normalVector * overlap;

			//adjust spin
			//TODO - calculate adjustment to A and B spin and apply it

			//cap spin
			if (b->spin > 1020.f) b->spin = 1020.f;
			if (b->spin < -1020.f) b->spin = -1020.f;
			//if (p->spin > 1020.f) p->spin = 1020.f;
			//if (p->spin < -1020.f) p->spin = -1020.f;
		}
	}
	return collision;
}

/*
bool Collision(Ball& b, Player* p) 
{
	float bRadiusSquared = b.radius * b.radius;
	float pRadiusSquared = (p->width/2) * (p->width/2);
	if (DistanceSquared(p->position, b.position) > bRadiusSquared + pRadiusSquared) return false;
	return true;
}

*/