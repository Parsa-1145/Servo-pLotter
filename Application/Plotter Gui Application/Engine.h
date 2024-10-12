#pragma once
#include "Updater.h"
#include "Renderer.h"
#include "Common.h"
#include "InputHandler.h"


class Engine {
public:
	GLFWwindow* window;
	GLFWwindow* loaderContext;

	InputState inputState;

	Updater updater;
	thread updateThread;

	Renderer renderer;
	thread renderThread;

	std::mutex locker;
	bool shouldTerminate = false;
	bool shouldStart = false;

	void init(int width, int height, const char* title);
	void eventLoop();
	void test() { cout << "woho" << endl; }
	void addEventToUpdater();

private:

	void initCallbacks();
};