#pragma once
#include "Engine.h"

class StateBase {
public:
	virtual void mouseButtonCB	(InputState* inputState) {};
	virtual void scrollCB		(InputState* inputState) {};
	virtual void keyboardCB		(InputState* inputState) {};
	virtual void cursorPosCB	(InputState* inputState) {};
	virtual void windowResizeCB	(InputState* inputState) {};
	virtual void update			(float deltaTime) {};

	virtual void render() {};
	
};

template<class T> class ObjectBase {
public:
	T* curState;
	StateBase** constantStatePtr;
	boost::signals2::scoped_connection mouseBtnCon;
	boost::signals2::scoped_connection keyCon;
	boost::signals2::scoped_connection scrollCon;
	boost::signals2::scoped_connection cursorCon;
	boost::signals2::scoped_connection updateCon;
	boost::signals2::scoped_connection resizeCon;
	boost::signals2::scoped_connection renderCon;



	Engine* engine;

	void initCallbacks() {
		this->constantStatePtr = (StateBase**)(&this->curState);

		this->mouseBtnCon = this->engine->updater.mouseBtnSignal.connect([this]() {
			//cout << this->engine->updater.eventList[0]->mouseBtnPress.action << endl;
			(*this->constantStatePtr)->mouseButtonCB(this->engine->updater.eventList[0]);
			});
		this->keyCon = this->engine->updater.keyPressSignal.connect([this]() {
			(*this->constantStatePtr)->keyboardCB(this->engine->updater.eventList[0]);
			});
		this->scrollCon = this->engine->updater.scrollSignal.connect([this]() {
			(*this->constantStatePtr)->scrollCB(this->engine->updater.eventList[0]);
			});
		this->cursorCon = this->engine->updater.cursorMoveSignal.connect([this]() {
			(*this->constantStatePtr)->cursorPosCB(this->engine->updater.eventList[0]);
			});
		this->resizeCon = this->engine->updater.windowResizeSignal.connect([this]() {
			(*this->constantStatePtr)->windowResizeCB(this->engine->updater.eventList[0]);
			});
		this->updateCon = this->engine->updater.updateSignal.connect([this]() {
			(*this->constantStatePtr)->update(this->engine->updater.deltaTime);
			});
		this->renderCon = this->engine->renderer.renderSignal.connect([this]() {
			(*this->constantStatePtr)->render();
			});
	}
};