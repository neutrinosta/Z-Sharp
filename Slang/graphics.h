
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <boost/any.hpp>
#include "strops.h"
#include "builtin.h"
#include "main.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <chrono>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

using namespace std;

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

unordered_map<string, int> KEYS = 
    {
        {"ESCAPE", 0},
        {"1", 0},
        {"2", 0},
        {"3", 0},
        {"4", 0},
        {"5", 0},
        {"6", 0},
        {"7", 0},
        {"8", 0},
        {"9", 0},
        {"TILDE", 0},
        {"MINUS", 0},
        {"EQUALS", 0},
        {"BACKSPACE", 0},
        {"TAB", 0},
        {"Q", 0},
        {"W", 0},
        {"E", 0},
        {"R", 0},
        {"T", 0},
        {"Y", 0},
        {"U", 0},
        {"I", 0},
        {"O", 0},
        {"P", 0},
        {"BRACKET_L", 0},
        {"BRACKET_R", 0},
        {"BACKSLASH", 0},
        {"A", 0},
        {"S", 0},
        {"D", 0},
        {"F", 0},
        {"G", 0},
        {"H", 0},
        {"J", 0},
        {"K", 0},
        {"L", 0},
        {"COLON", 0},
        {"QUOTE", 0},
        {"ENTER", 0},
        {"SHIFT_L", 0},
        {"Z", 0},
        {"X", 0},
        {"C", 0},
        {"V", 0},
        {"B", 0},
        {"N", 0},
        {"M", 0},
        {"ANGLEBRACKET_L", 0},
        {"ANGLEBRACKET_R", 0},
        {"QUESTIONMARK", 0},
        {"SHIFT_R", 0},
        {"CTRL_L", 0},
        {"ALT_L", 0},
        {"SPACE", 0},
        {"ALT_R", 0},
        {"CTRL_R", 0},
        {"LEFT", 0},
        {"UP", 0},
        {"RIGHT", 0},
        {"DOWN", 0}
    };

enum Buttons
{
	PaddleOneUp = 0,
	PaddleOneDown,
	PaddleTwoUp,
	PaddleTwoDown,
};

const float PADDLE_SPEED = 1.0f;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The renderer we'll be rendering to
SDL_Renderer* gRenderer = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

bool running = true;
bool buttons[4] = {};
float dt = 0.0f;

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

class Vec2
{
public:
	Vec2()
		: x(0.0f), y(0.0f)
	{}

	Vec2(float x, float y)
		: x(x), y(y)
	{}

	Vec2 operator+(Vec2 const& rhs)
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2& operator+=(Vec2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}
	
	Vec2 operator-(Vec2 const& rhs)
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2& operator-=(Vec2 const& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;

		return *this;
	}

	Vec2 operator*(float rhs)
	{
		return Vec2(x * rhs, y * rhs);
	}

	float x, y;
};

struct _RotRect {
 _Vector2D C;
 _Vector2D S;
 float ang;
};

inline void RotateVec2Clockwise(Vec2 * v, float ang)
{
 float t,
       cosa = cos(ang),
       sina = sin(ang);
 t = v->x; v->x = t*cosa + v->y*sina; v->y = -t*sina + v->y*cosa;
}

// Rotated Rectangles Collision Detection, Oren Becker, 2001
int RotRectsCollision(_RotRect * rr1, _RotRect * rr2)
{
 Vec2 A, B,   // vertices of the rotated rr2
	   C,      // center of rr2
	   BL, TR; // vertices of rr2 (bottom-left, top-right)

 float ang = rr1->ang - rr2->ang, // orientation of rotated rr1
       cosa = cos(ang),           // precalculated trigonometic -
       sina = sin(ang);           // - values for repeated use

 float t, x, a;      // temporary variables for various uses
 float dx;           // deltaX for linear equations
 float ext1, ext2;   // min/max vertical values

 // move rr2 to make rr1 cannonic
 C = rr2->C;
 C-=rr1->C;

 // rotate rr2 clockwise by rr2->ang to make rr2 axis-aligned
 RotateVector2DClockwise(&C, rr2->ang);

 // calculate vertices of (moved and axis-aligned := 'ma') rr2
 BL = TR = C;
 BL-=rr2->S;
 TR+=rr2->S;

 // calculate vertices of (rotated := 'r') rr1
 A.x = -rr1->S.y*sina; B.x = A.x; t = rr1->S.x*cosa; A.x += t; B.x -= t;
 A.y =  rr1->S.y*cosa; B.y = A.y; t = rr1->S.x*sina; A.y += t; B.y -= t;

 t = sina*cosa;

 // verify that A is vertical min/max, B is horizontal min/max
 if (t < 0)
 {
  t = A.x; A.x = B.x; B.x = t;
  t = A.y; A.y = B.y; B.y = t;
 }

 // verify that B is horizontal minimum (leftest-vertex)
 if (sina < 0) { B.x = -B.x; B.y = -B.y; }

 // if rr2(ma) isn't in the horizontal range of
 // colliding with rr1(r), collision is impossible
 if (B.x > TR.x || B.x > -BL.x) return 0;

 // if rr1(r) is axis-aligned, vertical min/max are easy to get
 if (t == 0) {ext1 = A.y; ext2 = -ext1; }
 // else, find vertical min/max in the range [BL.x, TR.x]
 else
 {
  x = BL.x-A.x; a = TR.x-A.x;
  ext1 = A.y;
  // if the first vertical min/max isn't in (BL.x, TR.x), then
  // find the vertical min/max on BL.x or on TR.x
  if (a*x > 0)
  {
   dx = A.x;
   if (x < 0) { dx -= B.x; ext1 -= B.y; x = a; }
   else       { dx += B.x; ext1 += B.y; }
   ext1 *= x; ext1 /= dx; ext1 += A.y;
  }
  
  x = BL.x+A.x; a = TR.x+A.x;
  ext2 = -A.y;
  // if the second vertical min/max isn't in (BL.x, TR.x), then
  // find the local vertical min/max on BL.x or on TR.x
  if (a*x > 0)
  {
   dx = -A.x;
   if (x < 0) { dx -= B.x; ext2 -= B.y; x = a; }
   else       { dx += B.x; ext2 += B.y; }
   ext2 *= x; ext2 /= dx; ext2 -= A.y;
  }
 }

 // check whether rr2(ma) is in the vertical range of colliding with rr1(r)
 // (for the horizontal range of rr2)
 return !((ext1 < BL.y && ext2 < BL.y) ||
	  (ext1 > TR.y && ext2 > TR.y));
}

class Sprite
{
public:
	Sprite(std::string path, Vec2 position, Vec2 scale, double angle)
		: position(position), angle(angle)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = scale.x;
		rect.h = scale.y;

		SDL_Surface* surface = loadSurface(path);
		texture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);
	}

	void Draw()
	{
		rect.y = static_cast<int>(position.y);

		SDL_RenderCopy(gRenderer, texture, NULL, &rect);
	}

	Vec2 position;
	double angle;
	std::string path;
	SDL_Rect rect{};
	SDL_Texture* texture;
};/*

class PlayerScore
{
public:
	PlayerScore(Vec2 position, SDL_Renderer* renderer, TTF_Font* font, SDL_Color scoreColor)
		: renderer(renderer), font(font)
	{
		surface = TTF_RenderText_Solid(font, "0", scoreColor);
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = width;
		rect.h = height;
	}

	~PlayerScore()
	{
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	void Draw()
	{
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}

	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Surface* surface{};
	SDL_Texture* texture{};
	SDL_Rect rect{};
};*/

int cleanupGraphics()
{
	// Cleanup
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}


int updateLoop()
{
	// Continue looping and processing events until user exits
	while (running)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					KEYS["ESCAPE"] = 1;
				else if (event.key.keysym.sym == SDLK_0)
					KEYS["0"] = 1;
				else if (event.key.keysym.sym == SDLK_1)
					KEYS["1"] = 1;
				else if (event.key.keysym.sym == SDLK_2)
					KEYS["2"] = 1;
				else if (event.key.keysym.sym == SDLK_3)
					KEYS["3"] = 1;
				else if (event.key.keysym.sym == SDLK_4)
					KEYS["4"] = 1;
				else if (event.key.keysym.sym == SDLK_5)
					KEYS["5"] = 1;
				else if (event.key.keysym.sym == SDLK_6)
					KEYS["6"] = 1;
				else if (event.key.keysym.sym == SDLK_7)
					KEYS["7"] = 1;
				else if (event.key.keysym.sym == SDLK_8)
					KEYS["8"] = 1;
				else if (event.key.keysym.sym == SDLK_MINUS)
					KEYS["MINUS"] = 1;
				else if (event.key.keysym.sym == SDLK_EQUALS)
					KEYS["EQUALS"] = 1;
				else if (event.key.keysym.sym == SDLK_AC_BACK)
					KEYS["BACKSPACE"] = 1;
				else if (event.key.keysym.sym == SDLK_TAB)
					KEYS["TAB"] = 1;
				else if (event.key.keysym.sym == SDLK_q)
					KEYS["q"] = 1;
				else if (event.key.keysym.sym == SDLK_w)
					KEYS["w"] = 1;
				else if (event.key.keysym.sym == SDLK_e)
					KEYS["e"] = 1;
				else if (event.key.keysym.sym == SDLK_r)
					KEYS["r"] = 1;
				else if (event.key.keysym.sym == SDLK_t)
					KEYS["t"] = 1;
				else if (event.key.keysym.sym == SDLK_y)
					KEYS["y"] = 1;
				else if (event.key.keysym.sym == SDLK_u)
					KEYS["u"] = 1;
				else if (event.key.keysym.sym == SDLK_i)
					KEYS["i"] = 1;
				else if (event.key.keysym.sym == SDLK_o)
					KEYS["o"] = 1;
				else if (event.key.keysym.sym == SDLK_p)
					KEYS["p"] = 1;
				else if (event.key.keysym.sym == SDLK_LEFTBRACKET)
					KEYS["BRACKET_L"] = 1;
				else if (event.key.keysym.sym == SDLK_RIGHTBRACKET)
					KEYS["BRACKET_R"] = 1;
			}
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_w)
				{
					buttons[Buttons::PaddleOneUp] = false;
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					buttons[Buttons::PaddleOneDown] = false;
				}
				else if (event.key.keysym.sym == SDLK_UP)
				{
					buttons[Buttons::PaddleTwoUp] = false;
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					buttons[Buttons::PaddleTwoDown] = false;
				}
			}
		}

		//if (buttons[Buttons::PaddleOneUp])
		//{
		//	paddleOne.velocity.y = -PADDLE_SPEED;
		//}
		//else if (buttons[Buttons::PaddleOneDown])
		//{
		//	paddleOne.velocity.y = PADDLE_SPEED;
		//}
		//else
		//{
		//	paddleOne.velocity.y = 0.0f;
		//}

		//if (buttons[Buttons::PaddleTwoUp])
		//{
		//	paddleTwo.velocity.y = -PADDLE_SPEED;
		//}
		//else if (buttons[Buttons::PaddleTwoDown])
		//{
		//	paddleTwo.velocity.y = PADDLE_SPEED;
		//}
		//else
		//{
		//	paddleTwo.velocity.y = 0.0f;
		//}

		//// Update the paddle positions
		//paddleOne.Update(dt);
		//paddleTwo.Update(dt);

		// Clear the window to black
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(gRenderer);

		// Set the draw color to be white
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		// Draw the net
		for (int y = 0; y < WINDOW_HEIGHT; ++y)
		{
			if (y % 5)
			{
				SDL_RenderDrawPoint(gRenderer, WINDOW_WIDTH / 2, y);
			}
		}

		//// Draw the ball
		//ball.Draw(gRenderer);

		//// Draw the paddles
		//paddleOne.Draw(gRenderer);
		//paddleTwo.Draw(gRenderer);

		//// Display the scores
		//playerOneScoreText.Draw();
		//playerTwoScoreText.Draw();

		//randomAssSprite.Draw();

		// Present the backbuffer
		SDL_RenderPresent(gRenderer);

		// Calculate frame time
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();

		return 0;
	}
}

int initGraphics(std::string windowTitle, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;

	// Initialize SDL components
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	gWindow = SDL_CreateWindow(windowTitle.c_str(), 40, 40, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN || SDL_WINDOW_RESIZABLE);
	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);
	
	//Sprite randomAssSprite(
	//	Vec2((WINDOW_WIDTH / 2.0f) - (100 / 2.0f), (WINDOW_HEIGHT / 2.0f) - (100 / 2.0f)),
	//	Vec2(100, 100),
	//	0,
	//	"./circle.png");

	updateLoop();

	cleanupGraphics();

	return 0;
}

#endif