#include "Updater.h"

void Updater::init(GLFWwindow* loaderContext){
	this->loaderContext = loaderContext;

	this->signalList.push_back(&this->mouseBtnSignal	);
	this->signalList.push_back(&this->scrollSignal		);
	this->signalList.push_back(&this->keyPressSignal	);
	this->signalList.push_back(&this->cursorMoveSignal	);
	this->signalList.push_back(&this->windowResizeSignal);

	while(!(*this->shouldStart)){}
	glfwMakeContextCurrent(this->loaderContext);
	this->updateLoop();
}

void Updater::updateLoop(){
	this->prevTime = std::chrono::high_resolution_clock::now();
	while (!*this->shouldTerminate) {
		this->handleEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		//this->curTime = std::chrono::high_resolution_clock::now();
		//this->deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(this->curTime - this->prevTime).count();
		//cout << deltaTime << endl;
		this->updateSignal();

		//this->prevTime = this->curTime;
	}
}

void Updater::handleEvents() {
	for (int i = 0; i < this->eventList.size();i++) {
		(*this->signalList[this->eventList[0]->curEvent])();
		delete(this->eventList[0]);
		this->locker->lock();
		this->eventList.erase(this->eventList.begin());
		this->locker->unlock();
	}
}
