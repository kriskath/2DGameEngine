#include <iostream>
#include "Constants.h"
#include "Game.h"
#include "AssetManager.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "/GameEngine/libglm/lib/glm/glm.hpp"


EntityManager manager;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Renderer* Game::renderer;

//Constructor
Game::Game()
{
	this->isRunning = false;
}

//Deconstructor
Game::~Game() {}

//Checks if game is running
//Returns boolean of bIsRunning
bool Game::IsRunning() const { return this->isRunning; }

//Loads info from level
void Game::LoadLevel(int LevelNumber)
{
	/* Start including new assets to the assetmanager list */
	assetManager->AddTexture("tank-image", std::string("./assets/images/tank-big-right.png").c_str());
	assetManager->AddTexture("chopper-image", std::string("./assets/images/chopper-spritesheet.png").c_str());


	/* Start including entities and also components to them */
	Entity& tankEntity(manager.AddEntity("tank"));
	tankEntity.AddComponent<TransformComponent>(0, 0, 20, 20, 32, 32, 1);
	tankEntity.AddComponent<SpriteComponent>("tank-image");

	Entity& chopperEntity(manager.AddEntity("chopper"));
	chopperEntity.AddComponent<TransformComponent>(240, 106, 0, 0, 32, 32, 1);
	chopperEntity.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);


}

//Initializes SDL window and renderer 
//Sets game as running if no errors
void Game::Initialize(int width, int height)
{
	//Checks if SLD works
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Error initializing SDL." << std::endl;
		return;
	}

	//Create the window
	window = SDL_CreateWindow(
		NULL,					//name
		SDL_WINDOWPOS_CENTERED, //x-pos
		SDL_WINDOWPOS_CENTERED, //y-pos
		width,
		height,
		SDL_WINDOW_BORDERLESS); //type of window
	//Check if window was created
	if (!window)
	{
		std::cerr << "Error creating SDL window." << std::endl;
		return;
	}

	//Create the Renderer
	renderer = SDL_CreateRenderer(
		window,					//The window to render
		-1,						//Which driver to render with (-1 is default)
		0);
	//Check if renderer was created
	if (!renderer)
	{
		std::cerr << "Error creating SDL renderer." << std::endl;
		return;
	}

	//Load the level to run
	LoadLevel(0);

	//Game is now running
	isRunning = true;
	return;
}

//Processes User Input
void Game::ProcessInput()
{
	SDL_Event Event;
	SDL_PollEvent(&Event);
	switch (Event.type)
	{
		//Closes the game when the window is closed
		case SDL_QUIT:
		{
			isRunning = false;
			break;
		}
		//Closes the game when key ESC is pressed
		case SDL_KEYDOWN: 
		{
			if (Event.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}
		}
		default:
		{
			break;
		}
	}
}

//Update the game objects
void Game::Update()
{
	//Wait until 16.6 ms has elapsed since the last frame
	//In case computations happen before 16.6 ms we need to waste time till we get to 16.6 ms
	//***Not efficient since it forces 100% of CPU to focus on this while loop. Nothing else can work with the CPU while in this.***
	//Took about 15% of CPU 
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), TicksLastFrame + FRAME_TARGET_TIME));

	//Wait until 16.6 ms has elapsed since the last frame
	//In case computations happen before 16.6 ms we need to waste time till we get to 16.6 ms
	//***Much more efficient than previous line. CPU can now do other processes in this time***
	//Takes 0-0.1% of CPU
	int TimeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - ticksLastFrame);
	if (TimeToWait > 0 && TimeToWait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(TimeToWait);
	}

	//Delta time is the difference in ticks from last frame converted to seconds
	float DeltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	//Clamp DeltaTime to a maximum value
	DeltaTime = (DeltaTime > 0.05f) ? 0.05f : DeltaTime;

	//Sets the new ticks for the current frame to be used in the next pass
	ticksLastFrame = SDL_GetTicks(); //Update the ticks to this time

	//Updates all entities
	manager.Update(DeltaTime);
}

//Render Window
//SDL has a bakc buffer screen that is updated
//The buffer swaps with the actual screen to reduce artifacts
void Game::Render()
{
	//Sets background color
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); //Draws the window as grey

	//Clears back buffer
	SDL_RenderClear(renderer); 

	//Dont render if there is nothing to render
	if (manager.HasNoEntities())
	{
		return;
	}

	//Render all entities
	manager.Render();

	//Swap front and back buffers
	SDL_RenderPresent(renderer); //Swaps the rendered screen for visualization
}

//Destroys window and renderer then quits
void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}