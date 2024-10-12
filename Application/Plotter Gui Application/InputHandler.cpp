#include "InputHandler.h"

bool InputState::keyIsHeld(GLint key){
    return (this->curKeys.find(key) != this->curKeys.end());
}

bool InputState::mouseBtnIsHeld(GLint btn)
{
    return (this->curMouseBtns.find(btn) != this->curMouseBtns.end());
}

bool InputState::keyPressed(GLint key)
{
    return this->keyboardPress.key == key && this->keyboardPress.action == 1;
}

bool InputState::mouseBtnPressed(GLint btn)
{
    return this->mouseBtnPress.btn == btn && this->mouseBtnPress.action == 1;
}

bool InputState::keyReleased(GLint key)
{
    return this->keyboardPress.key == key && this->keyboardPress.action == 0;
}

bool InputState::mouseBtnReleased(GLint btn)
{
    return this->mouseBtnPress.btn == btn && this->mouseBtnPress.action == 0;
}
