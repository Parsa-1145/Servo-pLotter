#include "selectionRect.h"
#include "Camera.h"
#include "SceneController.h"

SelectionRectNS::SelectionRect::SelectionRect(Engine* engine, PlotterApp* plotterApp){
	this->engine = engine;
	this->plotterApp = plotterApp;

	this->selecting.rect = this;
	this->idle.rect = this;
	this->curState = &this->idle;

	this->vertices.push_back({ 0, 0, 0 });
	this->vertices.push_back({ 0, 0, 0 });
	this->vertices.push_back({ 0, 0, 0 });
	this->vertices.push_back({ 0, 0, 0 });


	this->initCallbacks();
	this->initVbo();
}

void SelectionRectNS::SelectionRect::updateVbo(){
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, &this->vertices[0], this->vertices.size() * sizeof(vec3));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void SelectionRectNS::SelectionRect::initVbo(){
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * this->vertices.size(), &(this->vertices[0]), GL_DYNAMIC_DRAW);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, &this->vertices[0], this->vertices.size() * 12);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void SelectionRectNS::Idle::mouseButtonCB(InputState* state) {
	if (state->mouseBtnPressed(GLFW_MOUSE_BUTTON_1)) {
		this->rect->selecting.startPoint = this->rect->plotterApp->camera->getScreenData() + state->curMousePos * this->rect->plotterApp->camera->zoomScale;
		this->rect->selecting.endPoint = this->rect->plotterApp->camera->getScreenData() + state->curMousePos * this->rect->plotterApp->camera->zoomScale;
		this->rect->selecting.updateQuad();
		this->rect->selecting.startingSelections = this->rect->plotterApp->sceneController->selectedEntities;

		this->rect->curState = &this->rect->selecting;
	}
}

void SelectionRectNS::Selecting::mouseButtonCB(InputState* state){
	if (state->mouseBtnReleased(GLFW_MOUSE_BUTTON_1)) {
		this->rect->curState = &this->rect->idle;
	}
}

void SelectionRectNS::Selecting::cursorPosCB(InputState* state){
	this->endPoint = this->rect->plotterApp->camera->getScreenData() + state->curMousePos * this->rect->plotterApp->camera->zoomScale;

	if (this->startPoint[0] < this->endPoint[0]) {
		this->xRange[0] = this->startPoint[0];
		this->xRange[1] = this->endPoint[0];
	}
	else {
		this->xRange[1] = this->startPoint[0];
		this->xRange[0] = this->endPoint[0];
	}
	if (this->startPoint[1] < this->endPoint[1]) {
		this->yRange[0] = this->startPoint[1];
		this->yRange[1] = this->endPoint[1];
	}
	else {
		this->yRange[1] = this->startPoint[1];
		this->yRange[0] = this->endPoint[1];
	}

	for (SVGElementBase* e : this->rect->plotterApp->sceneController->svgElementList) {
		bool eWasSelected = this->wasSelected(e);
		if (e->isInBox(xRange, yRange)) {
			e->select();
		}
		else if ((!eWasSelected) || (eWasSelected && !state->keyIsHeld(GLFW_KEY_LEFT_SHIFT))) {
			e->deselect();
		}
	}

	this->updateQuad();
}

void SelectionRectNS::Selecting::render(){
	glBindVertexArray(this->rect->engine->renderer.vaoList.simpleLine);

	glUseProgram(this->rect->engine->renderer.programList.simpleLine.id);
	glBindVertexBuffer(0, this->rect->vbo, 0, 3 * sizeof(float));
	glUniformMatrix4fv(this->rect->engine->renderer.programList.simpleLine.viewProjecUni, 1, GL_TRUE,
		&this->rect->plotterApp->camera->VPMat[0][0]);
	glUniformMatrix4fv(this->rect->engine->renderer.programList.simpleLine.modelUni, 1, GL_TRUE,
		&(this->rect->modelMat[0][0]));
	glUniform4f(this->rect->engine->renderer.programList.simpleLine.colorUni, 1.0f, 1, 1, 0.3f);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDepthMask(GL_FALSE);
	glBindVertexArray(this->rect->engine->renderer.vaoList.simpleQuad);
	glBindVertexBuffer(0, this->rect->vbo, 0, 3 * sizeof(float));
	glUniform4f(this->rect->engine->renderer.programList.simpleLine.colorUni, 1.0f, 1, 1, 0.05);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SelectionRectNS::Selecting::updateQuad(){
	this->rect->vertices[0] = vec3(this->startPoint, -0.1);
	this->rect->vertices[1] = vec3(this->startPoint[0], this->endPoint[1], -0.1);
	this->rect->vertices[2] = vec3(this->endPoint, -0.1);
	this->rect->vertices[3] = vec3(this->endPoint[0], this->startPoint[1], -0.1);
	this->rect->updateVbo();
}

bool SelectionRectNS::Selecting::wasSelected(SVGElementBase* e){
	return (this->startingSelections.find(e) != this->startingSelections.end());
}
