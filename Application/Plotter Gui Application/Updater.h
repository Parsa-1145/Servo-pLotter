#pragma once
#include "Common.h"
#include "InputHandler.h"



class Updater {
public:
	GLFWwindow* loaderContext;

	std::mutex* locker;
	std::vector<InputState*> eventList;
	float deltaTime;
	
	signal<void()> mouseBtnSignal;
	signal<void()> keyPressSignal;
	signal<void()> cursorMoveSignal;
	signal<void()> scrollSignal;
	signal<void()> windowResizeSignal;
	std::vector<signal<void()>*> signalList;

	signal<void()> updateSignal;

	bool* shouldTerminate;
	bool* shouldStart;

	void init(GLFWwindow* loaderContext);
private:
	void updateLoop();
	void handleEvents();
	std::string ev[5] = { "mouseBtnSignal", "scrollSignal" , "keyPressSignal" , "cursorMoveSignal" , "asd"};

	std::chrono::time_point<std::chrono::high_resolution_clock> curTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> prevTime;
};