#include "Camera.h"

CameraNS::Camera::Camera(Engine* engine, PlotterApp* plotterApp){
	this->engine = engine;
	this->plotterApp = plotterApp;

	this->wWidth	= &(this->engine->renderer.wWidth	);
	this->wHeight	= &(this->engine->renderer.wHeight	);
	this->zoomScale = 1;
	this->engine->renderer.VPMat = &this->VPMat;

	this->idle		=	new Idle();
	this->panning	=	new Panning();

	this->panning->camera	= this;
	this->idle->camera		= this;

	this->curState = this->idle;

	this->initCallbacks();

	this->calcVPMat();

}

void CameraNS::Camera::calcVPMat(){
	this->viewMat = {   1, 0, 0, -this->worldPosition[0],
						0, 1, 0, -this->worldPosition[1],
						0, 0, 1, -this->worldPosition[2],
						0, 0, 0, 1 };
	this->projMat = {	2 / ((*this->wWidth) * this->zoomScale), 0, 0, 0,
						0, 2 / ((*this->wHeight) * this->zoomScale), 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };
	this->VPMat = this->viewMat * this->projMat;
}

vec2 CameraNS::Camera::getScreenData(){
	
	return vec2(this->worldPosition[0] - (*this->wWidth) * this->zoomScale / 2,
		this->worldPosition[1] - (*this->wHeight) * this->zoomScale / 2);
}

void CameraNS::Panning::mouseButtonCB(InputState* state){
	if (state->mouseBtnPress.btn == 2 && state->mouseBtnPress.action == 0) {
		this->camera->curState = this->camera->idle;
	}
}

void CameraNS::Panning::cursorPosCB(InputState* state){
	vec2 dif = state->curMousePos - state->prevMousePos;
	dif *= -1;
	dif *= this->camera->zoomScale;
	
	this->camera->worldPosition += vec3(dif[0], dif[1], 0 );
	this->camera->calcVPMat();
}

void CameraNS::Idle::mouseButtonCB(InputState* state) {
	if (state->mouseBtnPress.btn == 2 && state->mouseBtnPress.action == 1) {
		this->camera->curState = this->camera->panning;
	}
}

void CameraNS::CameraState::windowResizeCB(InputState* input){
	this->camera->calcVPMat();
}

void CameraNS::CameraState::scrollCB(InputState* input){
	this->camera->zoomScale -= 0.05 * input->scroll * this->camera->zoomScale;
	if (this->camera->zoomScale <= 0) this->camera->zoomScale = 0.05;
	//cout << this->camera->zoomScale << endl;
	this->camera->calcVPMat();
}