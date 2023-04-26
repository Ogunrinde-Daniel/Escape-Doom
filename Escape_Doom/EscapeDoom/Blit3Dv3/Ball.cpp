#include "Ball.h"
#include <random>

extern std::mt19937 rng;

Ball::Ball(Sprite* ImageSprite, float Radius, float Density)
{
	sprite = ImageSprite;
	radius = Radius;
	density = Density;
	//calculate mass, area opf a circle times the density
	mass = glm::pi<float>() * radius * radius * density;
}

void Ball::Draw()
{
	sprite->angle = angle;
	sprite->Blit(position.x, position.y);
}

void Ball::Update(float seconds) 
{
	//update ball position
	position = position + velocity * seconds;

	//update ball angle
	angle = angle + spin * seconds;

	//bounds check each side of the window with the ball

	//left side -is the ball too far to the left?
	if (position.x < 0.f + radius)
	{
		//too far to the left
		//1) move the ball back inside the window
		position.x = 0.f + radius;

		//2)reflect the velocity off the side of the screen
		velocity.x = velocity.x * -1.f;

		//3)update spin
		spin = spin + velocity.y * 0.1f;
		if (spin > 1020.f) spin = 1020.f;
		if (spin < -1020.f) spin = -1020.f;
	}

	//right side -is the ball too far to the right?
	if (position.x > 1920.f - radius)
	{
		//too far to the right
		//1) move the ball back inside the window
		position.x = 1920.f - radius;

		//2)reflect the velocity off the side of the screen
		velocity.x = velocity.x * -1.f;

		//3)update spin
		spin = spin - velocity.y * 0.1f;
		if (spin > 1020.f) spin = 1020.f;
		if (spin < -1020.f) spin = -1020.f;
	}

	//bottom side -is the ball too far down?
	if (position.y < 0.f + radius)
	{
		//too far down
		//1) move the ball back inside the window
		position.y = 0.f + radius;

		//2)reflect the velocity off the side of the screen
		velocity.y = velocity.y * -1.f;

		//3)update spin
		spin = spin - velocity.x * 0.1f;
		if (spin > 1020.f) spin = 1020.f;
		if (spin < -1020.f) spin = -1020.f;
	}

	//top side -is the ball too high up?
	if (position.y > 1080.f - radius)
	{
		//too far to the right
		//1) move the ball back inside the window
		position.y = 1080.f - radius;

		//2)reflect the velocity off the side of the screen
		velocity.y = velocity.y * -1.f;

		//3)update spin
		spin = spin + velocity.x * 0.1f;
		if (spin > 1020.f) spin = 1020.f;
		if (spin < -1020.f) spin = -1020.f;
	}

}

bool CollideBalls(Ball* A, Ball* B)
{
	if (A == B) return false;

	bool collision = false;
	glm::vec2 line = A->position - B->position;

	//check cheap, fast box2box collision
	float Aleft = A->position.x - A->radius;
	float Aright = A->position.x + A->radius;
	float Atop = A->position.y + A->radius;
	float Abottom = A->position.y - A->radius;

	float Bleft = B->position.x - B->radius;
	float Bright = B->position.x + B->radius;
	float Btop = B->position.y + B->radius;
	float Bbottom = B->position.y - B->radius;

	if(!(Aleft > Bright || Aright < Bleft || Atop < Bbottom || Abottom > Btop))
	{
		//the bounding boxes collided, but not neccessarily the enclosed circles
		// NOW we do more expensive, proper circle2circel collision
	
		//calculate the distance between the centers of the two balls
		float length = glm::length(line);

		if (length < A->radius + B->radius)
		{
			//collision!
			collision = true;
			//update velocities

			//calculate the vector from center-to-center of the balls
			glm::vec2 normalVector = glm::normalize(line);

			//to preserve momentum, use the center-of-mass frame (https://en.wikipedia.org/wiki/Center-of-momentum_frame)
			glm::vec2 vCenterMass = (A->velocity * A->mass + B->velocity * B->mass) / (A->mass + B->mass);

			//This is the velocity of an observer to whom the two balls seem to have equal 
			// and opposite momenta.
			//The velocities in that observer's frame are:
			A->velocity -= vCenterMass;
			B->velocity -= vCenterMass;

			//reflect the velocities off the plane tangent to the collison 
			A->velocity = glm::reflect(A->velocity, normalVector);
			B->velocity = glm::reflect(B->velocity, normalVector);

			//adjust back from the observer's frame to the original frame
			A->velocity += vCenterMass;
			B->velocity += vCenterMass;

			//calculate overlap
			float overlap = (A->radius + B->radius) - length;
			overlap = overlap / 2.f;

			//moves the balls apart by the half overlap - cheesy hack
			A->position += normalVector * overlap;
			B->position -= normalVector * overlap;

			//adjust spin
			//TODO - calculate adjustment to A and B spin and apply it

			//cap spin
			if (A->spin > 1020.f) A->spin = 1020.f;
			if (A->spin < -1020.f) A->spin = -1020.f;
			if (B->spin > 1020.f) B->spin = 1020.f;
			if (B->spin < -1020.f) B->spin = -1020.f;
		}
	}
	return collision;
}

void PositionBall(Ball* ball)
{
	//distributuions for a 1080p screen size
	std::uniform_real_distribution<float> yDist(ball->radius, 1080.f - ball->radius);
	std::uniform_real_distribution<float> xDist(ball->radius, 1920.f - ball->radius);

	//position the ball
	ball->position.x = xDist(rng);
	ball->position.y = yDist(rng);

	//assign a random velocity
	std::uniform_real_distribution<float> angleDist(0.f, 360.f);
	float angle = angleDist(rng);
	ball->velocity.x = cos(glm::radians(angle));
	ball->velocity.y = sin(glm::radians(angle));
	//scale the unit velocity up by a random amount
	std::uniform_real_distribution<float> speedDist(5.f, 1000.f);
	ball->velocity *= speedDist(rng);
}