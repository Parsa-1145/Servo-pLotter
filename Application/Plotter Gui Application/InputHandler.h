#pragma once
#include "Common.h"
struct MouseBtnPress {
	int btn;
	int action;
};
struct KeyboardPress {
	int key;
	int action;
};
class InputState {
public:
	vec2 prevMousePos;
	vec2 curMousePos;

	std::set<int> curKeys = {};
	std::set<int> prevKeys = {};
	std::set<int> curMouseBtns = {};
	std::set<int> prevMouseBtns = {};

	MouseBtnPress mouseBtnPress;
	KeyboardPress keyboardPress;
	int scroll;

	inputEvent curEvent;

	InputState& operator=(const InputState& rhs) {
		curKeys = rhs.curKeys;
		prevKeys = rhs.prevKeys;
		curMousePos = rhs.curMousePos;
		prevMousePos = rhs.prevMousePos;
		curMouseBtns = rhs.curMouseBtns;
		prevMouseBtns = rhs.prevMouseBtns;
		curEvent = rhs.curEvent;
		mouseBtnPress = rhs.mouseBtnPress;
		keyboardPress = rhs.keyboardPress;
		scroll = rhs.scroll;
		return *this;
	}
	bool keyIsHeld(GLint key);
	bool mouseBtnIsHeld(GLint btn);
	bool keyPressed(GLint key);
	bool mouseBtnPressed(GLint btn);
	bool keyReleased(GLint key);
	bool mouseBtnReleased(GLint btn);
};