/*
	Simple example of loading/rotating/displaying sprites in Blit3D
*/
//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"
#include "Ball.h"
#include "player.h"

#include<random>
Blit3D *blit3D = NULL;
Sprite* lifeSprite =  NULL;
Sprite* backgroundSprite = NULL;
Sprite* gameOverSprite= NULL;

Sprite* ball1Sprite = NULL;
Sprite* ball2Sprite = NULL;
Sprite* ball3Sprite = NULL;

AngelcodeFont* digital40 = NULL;

//GLOBAL DATA
std::vector<Ball*> ballList;
Player* player;
std::mt19937 rng;
float playTimer = 0.f;
int playTimerMinutes = 0;
int playTimerSeconds = 0;
bool startLevel = false;

void TestBalls(Ball* A, Ball* B)
{
	A->spin = B->spin = 0.f;
	A->position = glm::vec2(A->radius + 1.f, blit3D->screenHeight / 2);
	B->position = glm::vec2(blit3D->screenWidth - (B->radius + 100.f), blit3D->screenHeight / 2 - B->radius * 0.75f);

	B->velocity = glm::vec2(0.f, 0.f);
	A->velocity = glm::vec2( 300.f, 0.f);
}

void Init()
{
	//seed the rng
	std::random_device rd;
	rng.seed(rd());

	glClearColor(0.4f, 0.4f, 1.0f, 0.0f);	//clear colour: r,g,b,a 
	player = new Player(glm::vec2((1920 / 2), (1080 / 2)), 60, 60);

	//load graphics
	ball1Sprite  = blit3D->MakeSprite(0, 0, 106, 107, "Media\\ball1.png");
	ball2Sprite  = blit3D->MakeSprite(0, 0, 160, 160, "Media\\ball2.png");
	ball3Sprite  = blit3D->MakeSprite(0, 0, 214, 213, "Media\\ball3b.png");
	Sprite* playerSprite = blit3D->MakeSprite(0, 0, 80,  20,  "Media\\player.png");
	Sprite* playerSprite2= blit3D->MakeSprite(0, 0, 60, 60,   "Media\\playerSquare.png");

	lifeSprite			 = blit3D->MakeSprite(0, 0, 30, 30,	  "Media\\smallHeart.png");
	backgroundSprite     = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\background.png");
	gameOverSprite		 = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\gameOver2.png");
	player->sprite = playerSprite2;

	digital40 = blit3D->MakeAngelcodeFontFromBinary32("Media\\digital40.bin");
	
	std::uniform_int_distribution<int> ballType(0, 2);
	std::uniform_int_distribution<int> ballNo(4, 6);

	for (int i = 0; i < ballNo(rng); ++i)
	{
		switch (ballType(rng))
		{
		case 0: //medium sized volleyball
		{
			for (int i = 0; i < 2; i++)
			{
				Ball* b1 = new Ball(ball1Sprite, 106.5f / 2, 1.f);
				PositionBall(b1);
				ballList.push_back(b1);
			}
			break;
		}

		case 1: //large exercise ball
		{
			Ball* b2 = new Ball(ball2Sprite, 160.f / 2, 1.f);
			PositionBall(b2);
			ballList.push_back(b2);
			break;
		}

		default: //small tennis ball
		{
			Ball* b3 = new Ball(ball3Sprite, 213.5f / 2, 1.f);
			PositionBall(b3);
			ballList.push_back(b3);
			break;
		}
		}
	}

}

void Restart() 
{
	player->collisionTimer = 2.f;
	ballList.clear();
	player->lives = 3;
	std::uniform_int_distribution<int> ballType(0, 2);
	std::uniform_int_distribution<int> ballNo(4, 6);

	playTimerMinutes = 0;
	playTimerSeconds = 0;
	playTimer = 0.f;

	for (int i = 0; i < ballNo(rng); ++i)
	{
		switch (ballType(rng))
		{
		case 0: //medium sized volleyball
		{
			for (int i = 0; i < 2; i++)
			{
				Ball* b1 = new Ball(ball1Sprite, 106.5f / 2, 1.f);
				PositionBall(b1);
				ballList.push_back(b1);
			}
			break;
		}

		case 1: //large exercise ball
		{
			Ball* b2 = new Ball(ball2Sprite, 160.f / 2, 1.f);
			PositionBall(b2);
			ballList.push_back(b2);
			break;
		}

		default: //small tennis ball
		{
			Ball* b3 = new Ball(ball3Sprite, 213.5f / 2, 1.f);
			PositionBall(b3);
			ballList.push_back(b3);
			break;
		}
		}
	}
}
void DeInit(void)
{
	for (auto &b : ballList) delete b;
	if (player != NULL) delete player;
	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{

	float timePassed = static_cast<float>(seconds);
	//float timePassed = (float)seconds;

	if (timePassed > 0.15f) timePassed = 0.15f;

	if (player->lives > 1)
	{
		playTimer += timePassed;

		if (playTimer > 1.f)
		{
			playTimer = 0.f;
			playTimerSeconds += 1;
		}

		if (playTimerSeconds > 60) 
		{
			playTimerSeconds = 0;
			playTimerMinutes += 1;
		}

	}

		//tenSecondsTimer = 10.f;
	
	for (auto& b : ballList) b->Update(timePassed);
	player->update(timePassed);
	
	//collison loop; check collison for each ball with every other ball exactly once
	for (int i = 0; i < ballList.size() - 1; ++i)
	{
		for (int j = i + 1; j < ballList.size(); ++j)
		{
			CollideBalls(ballList[i], ballList[j]);
		}
	}

	for (auto b : ballList)
	{
		if (Collision(b, player))
		{
			player->lives--;
			player->collisionTimer = 2.f;
		}
			
	}
	if (player->lives < 1)
	{
		for (auto& b : ballList)
			b->velocity = glm::vec2(0,0);
	}

}

void Draw(void)
{
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	backgroundSprite->Blit(1920 / 2, 1080 / 2);
	//draw stuff here
	for (auto& b : ballList) b->Draw();

	player->draw();
	for (int i = 1; i <= player->lives; i++)
		lifeSprite->Blit( (1920 - (i*60)) , (1080 - 60) );

	if (player->lives < 1)
		gameOverSprite->Blit(1920 / 2, 1080 / 2);

	std::string nanoSec = std::to_string((int) ( playTimer * 100 ) );
	std::string Seconds = std::to_string(playTimerSeconds);
	std::string Minutes = std::to_string(playTimerMinutes);
	digital40->BlitText(1920 - 180, 1080 - 100, Minutes + " : " + Seconds + " : " + nanoSec );

	if (player->lives < 1)
		digital40->BlitText(1920 - 550, 1080 - 100, "Total Time Survived ::");
	
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		if (ballList.size() > 1)
		{
			TestBalls(ballList[0], ballList[1]);
		}
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		player->thrusting = true;
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		player->thrusting = false;

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		if (player->lives < 1)
			Restart();
	}
		
	if (key == GLFW_KEY_G && action == GLFW_RELEASE)
		startLevel = false;
	
	//right
	if ( (key == GLFW_KEY_D && action == GLFW_PRESS) || (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) )
		player->movingRight = true;

	if ((key == GLFW_KEY_D && action == GLFW_RELEASE) || (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE))
		player->movingRight = false;
	//left
	if ((key == GLFW_KEY_A && action == GLFW_PRESS) || (key == GLFW_KEY_LEFT && action == GLFW_PRESS))
		player->movingLeft = true;

	if ((key == GLFW_KEY_A && action == GLFW_RELEASE) || (key == GLFW_KEY_LEFT && action == GLFW_RELEASE))
		player->movingLeft = false;
	//up
	if ((key == GLFW_KEY_W && action == GLFW_PRESS) || (key == GLFW_KEY_UP && action == GLFW_PRESS))
		player->movingUp = true;

	if ((key == GLFW_KEY_W && action == GLFW_RELEASE) || (key == GLFW_KEY_UP && action == GLFW_RELEASE))
		player->movingUp = false;
	//down
	if ((key == GLFW_KEY_S && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS))
		player->movingDown = true;

	if ((key == GLFW_KEY_S && action == GLFW_RELEASE) || (key == GLFW_KEY_DOWN && action == GLFW_RELEASE))
		player->movingDown = false;
		
}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 640, 400);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}