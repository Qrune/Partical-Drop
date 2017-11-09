#define _CRT_SECURE_NO_WARNINGS
#define RESOLUTION 512
#define TARGET_FPS 30                // controls spin update rate
#define TIME_BETWEEN_UPDATES 0.015   // seconds between motion updates
#define PRINT_FPS_INTERVAL 10.0f


#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#include "gl3w/gl3w.h"

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Window.hpp>
//#define _USE_MATH_DEFINES
//#include <math.h>
#define PI 3.14159f

#include "GLLoader.h"
#include "ShaderManager.h"
#include "GLHelper.h"
#include "RenderEngine.h"
#include "WorldState.h"

class Program4
{
public:
	Program4()
	{
		getWindowContext();
		
		initLoader();
		float ver = initLoader();
		if( ver < 1.0f ) {
			printf("OpenGL is not supported.\n");
			exit(1);
		}
		printf("OpenGL version %.1f is supported.\n", ver);

		state.init();

		state.currentRes[0] = RESOLUTION;
		state.currentRes[1] = RESOLUTION;
		render.init(state);
		resize(window->getSize().x, window->getSize().y);
		//render.buildRenderBuffers(window->getSize().x, window->getSize().y);
		
		previousPos = glm::vec2(0);
		buttonDown[0]=false;
		buttonDown[1]=false;
		buttonDown[2]=false;
		
		sf::Clock c;

		float lastFrame = c.restart().asSeconds();
		float lastPrint = lastFrame;
		float targetFrameTime = 1.0f/(float)TARGET_FPS;
		
		while (state.isRunning())
		{			
			window->setActive();
			float currentTime = c.getElapsedTime().asSeconds();
			float sinceLastFrame = currentTime - lastFrame;
			float sleepTime = targetFrameTime - sinceLastFrame;
			if(sleepTime > 0)
				sf::sleep(sf::seconds(sleepTime));
			
			currentTime = c.getElapsedTime().asSeconds();
			lastFrame = currentTime;
			float sinceLastPrint = currentTime - lastPrint;
			
			handleEvents(state, render);
			state.timeStep(currentTime);
			
			if(sinceLastPrint > PRINT_FPS_INTERVAL) {
				lastPrint = currentTime;
				state.printFPS();
			}
			
			render.display(state);
			window->display();
		}
	}
	
private:
	sf::Window * window;
	RenderEngine render;
	WorldState state;
	
	sf::Clock timer;
	float lastUpdate;
	glm::ivec2 previousPos;
	bool buttonDown[3];

	void handleEvents(WorldState & state, RenderEngine & render)
	{
		bool shiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		sf::Event event;
		
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				state.setRunning(false);
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				state.setRunning(false);
            //todo Add shader mode
            if ((event.type == sf::Event::TextEntered) && (event.text.unicode == ' '))
                state.switchShadingMode();
			if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'r'))
				state.toggleModelRotate();
			if((event.type == sf::Event::TextEntered) && (event.text.unicode == 't'))
				state.toggleLightRotate();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'o'))
                state.test();
			
			if(event.type == sf::Event::Resized) {
				resize(event.size.width, event.size.height);
			}
            
            if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q))
                state.cameraMove(-0.05);
            if ((event.type== sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::E))
                state.cameraMove(0.05);
			if(event.type == sf::Event::MouseMoved && state.mouseButtonDown)
			{
				state.cursorDragAmount[0] += state.lastFrameDragPos[0] - event.mouseMove.x;
				state.cursorDragAmount[1] += state.lastFrameDragPos[1] - (state.currentRes[1]-event.mouseMove.y);
				state.lastFrameDragPos[0] = event.mouseMove.x;
				state.lastFrameDragPos[1] = (state.currentRes[1]-event.mouseMove.y);
			}

			if(event.type == sf::Event::MouseWheelMoved)
			{
				state.zoomCamera(event.mouseWheel.delta);
				state.cursorScrollAmount += event.mouseWheel.delta;
			}

			if(event.type == sf::Event::MouseMoved)
			{
				state.cursorAbsolutePos[0] = event.mouseMove.x;
				state.cursorAbsolutePos[1] = (state.currentRes[1]-event.mouseMove.y);
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				previousPos = glm::vec2(event.mouseButton.x, event.mouseButton.y);

				if (event.mouseButton.button == sf::Mouse::Left && !shiftDown)
					buttonDown[0] = 1;
				if (event.mouseButton.button == sf::Mouse::Right)
					buttonDown[1] = true;
				if (event.mouseButton.button == sf::Mouse::Middle)
					buttonDown[2] = true;
				if (event.mouseButton.button == sf::Mouse::Left && shiftDown)
					buttonDown[2] = true;
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left && !shiftDown)
					buttonDown[0] = false;
				if (event.mouseButton.button == sf::Mouse::Right)
					buttonDown[1] = false;
				if (event.mouseButton.button == sf::Mouse::Middle)
					buttonDown[2] = false;
				if (event.mouseButton.button == sf::Mouse::Left && shiftDown)
					buttonDown[2] = false;

				lastUpdate = timer.getElapsedTime().asSeconds();
			}
		}


		lastUpdate = timer.getElapsedTime().asSeconds();
		bool needsUpdate = lastUpdate > TIME_BETWEEN_UPDATES;
		if (needsUpdate)
		{
			glm::ivec2 newPos = glm::ivec2(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);

			if (buttonDown[0])
				//state.updateRotate(previousPos, newPos);
			if (buttonDown[1])
				state.updateXYTranslate(previousPos, newPos);
			if (buttonDown[2])
				//state.updateZTranslate(previousPos, newPos);

			lastUpdate = timer.restart().asSeconds();
			previousPos = newPos;
		}
	}
	

	void resize(size_t x, size_t y)
	{
		//render.buildRenderBuffers(x, y);
		state.currentRes[0] = x;
		state.currentRes[1] = y;
	}
	
	void getWindowContext()
	{
		sf::err().rdbuf(NULL); //hide errors
		
#ifdef __APPLE__
		int nullFD = open("/dev/null", O_WRONLY);
		int oldFD = dup(2); // Duplicate the old file descriptor, so it can be restored
		dup2(nullFD, 2); // Redirect
#endif
		
		sf::VideoMode mode(RESOLUTION, RESOLUTION, 32);
#ifdef __linux__
		sf::ContextSettings settings(32, 0, 0, 3, 3);
#else
		sf::ContextSettings settings(32, 0, 0, 3, 3, sf::ContextSettings::Core);
#endif
		window = new sf::Window(mode, "SFML application", sf::Style::Default, settings);
		
#ifdef __APPLE__
		dup2(oldFD, 2); // Redirect back
		close(oldFD); // Not needed anymore
#endif
	}
};

int main()
{
	Program4 prog;
	
	return EXIT_SUCCESS;
}
