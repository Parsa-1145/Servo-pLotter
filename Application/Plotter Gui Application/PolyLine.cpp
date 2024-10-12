#include "PolyLine.h"
#include "Camera.h"
#include "Grid.h"
#include "SceneController.h"

SVGElements::PolyLine::PolyLine::PolyLine(Engine* engine, PlotterApp* plotterApp, std::vector<vec3>* vertices){
	this->plotterApp = plotterApp;
	this->engine = engine;
	this->vertices = *vertices;
	this->worldPosition = { 0, 0, 0 };

	this->idle = new Idle();
	this->idle->polyLine = this;
	this->selected = new Selected();
	this->selected->polyLine = this;
	this->curState = this->idle;

	this->initCallbacks();

	this->initVbo();

	this->modelMat = {  1, 0, 0, this->worldPosition[0],
						0, 1, 0, this->worldPosition[1],
						0, 0, 1, this->worldPosition[2],
						0, 0, 0, 1 };
}

void SVGElements::PolyLine::PolyLine::initVbo(){
	this->engine->locker.lock();
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * this->vertices.size(), &(this->vertices[0]), GL_STATIC_DRAW);

	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, &this->vertices[0], this->vertices.size() * sizeof(vec3));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->engine->locker.unlock();

}
void SVGElements::PolyLine::PolyLine::updateModelMat(){
	this->modelMat = { 1, 0, 0, this->worldPosition[0],
						0, 1, 0, this->worldPosition[1],
						0, 0, 1, this->worldPosition[2],
						0, 0, 0, 1 };
}
moveData SVGElements::PolyLine::PolyLine::moveOnShape(float moveDistance){
	vec3 vectorToTarget = this->vertices[this->targetIndex] - this->curPos;
	float distanceToTarget = hypot(vectorToTarget[0], vectorToTarget[1]);
	if (moveDistance < distanceToTarget) {
		this->curPos += (vectorToTarget) * (moveDistance / distanceToTarget);
		return moveData(false, this->curPos + this->worldPosition);
	}
	else {
		if (this->targetIndex == this->vertices.size() - 1) {
			this->curPos = this->vertices[this->vertices.size() - 1];
			return moveData(true, this->curPos + this->worldPosition);

		}
		else {
			this->curPos = this->vertices[this->targetIndex];
			this->targetIndex += 1;
			this->moveOnShape(moveDistance - distanceToTarget);
			return moveData(false, this->curPos + this->worldPosition);

		}
	}
}
void SVGElements::PolyLine::PolyLine::initExecution(){
	this->curPos = this->vertices[0];
	this->targetIndex = 1;
}
void SVGElements::PolyLine::PolyLine::translate(vec2 transVec){
	this->worldPosition[0] += transVec[0];
	this->worldPosition[1] += transVec[1];
	this->updateModelMat();
}
void SVGElements::PolyLine::PolyLine::select(){
	if (this->curState != this->selected) {
		this->curState = this->selected;
		this->color = { 0, 1, 0, 1 };
		this->plotterApp->sceneController->selectedEntities.insert(this);
	}
}
void SVGElements::PolyLine::PolyLine::deselect(){
	if (this->curState != this->idle) {
		this->curState = this->idle;
		this->color = { 1, 0, 0, 1 };
		this->plotterApp->sceneController->selectedEntities.erase(this);
		this->isHovered = false;
		this->hoveredVertexIndex = -1;
	}
}
vec3 SVGElements::PolyLine::PolyLine::getHoveringVertexPosition(){
	return this->vertices[this->hoveredVertexIndex] + this->worldPosition;
}
bool SVGElements::PolyLine::PolyLine::isInBox(vec2 xRange, vec2 yRange){
	xRange -= this->worldPosition[0];
	yRange -= this->worldPosition[1];
	for (vec3 v : this->vertices) {
		if ((((v[0] - xRange[0]) * (v[0] - xRange[1])) < 0) && (((v[1] - yRange[0]) * (v[1] - yRange[1])) < 0)) {
			return true;
		}
	}
	return false;
}
std::string SVGElements::PolyLine::PolyLine::getMachineCode(){
	std::string code = "";
	for (vec3 v : this->vertices) {
		code += "M\n" + std::to_string(this->worldPosition[0] + v[0]) + '\n' + std::to_string(this->worldPosition[1] + v[1]) + '\n';
	}
	return code;
}
///////////////////////////States///////////////////////////
void SVGElements::PolyLine::PolyLineState::render() {
	glBindVertexArray(this->polyLine->engine->renderer.vaoList.simpleLine);

	glUseProgram(this->polyLine->engine->renderer.programList.simpleLine.id);
	glBindVertexBuffer(0, this->polyLine->vbo, 0, 3 * sizeof(float));
	glUniformMatrix4fv(this->polyLine->engine->renderer.programList.simpleLine.viewProjecUni, 1, GL_TRUE,
		&this->polyLine->plotterApp->camera->VPMat[0][0]);
	glUniformMatrix4fv(this->polyLine->engine->renderer.programList.simpleLine.modelUni, 1, GL_TRUE,
		&(this->polyLine->modelMat[0][0]));
	glUniform4f(this->polyLine->engine->renderer.programList.simpleLine.colorUni, this->polyLine->color[0]
		, this->polyLine->color[1], this->polyLine->color[2], this->polyLine->color[3]);
	glDrawArrays(GL_LINE_STRIP, 0, this->polyLine->vertices.size());
	if (this->polyLine->hoveredVertexIndex != -1) {
		glPointSize(10);
		glDrawArrays(GL_POINTS, this->polyLine->hoveredVertexIndex, 1);
	}
}
///////////////////////////Idle///////////////////////////
void SVGElements::PolyLine::Idle::cursorPosCB(InputState* state){
	vec2 worldPos = this->polyLine->plotterApp->camera->getScreenData() + state->curMousePos * this->polyLine->plotterApp->camera->zoomScale;
	bool flag = false;

	for (int i = 0; i < this->polyLine->vertices.size() - 1; i++) {
		float dist = 0;
		vec2 p1 = this->polyLine->vertices[i] + this->polyLine->worldPosition;
		vec2 p2 = this->polyLine->vertices[i + 1] + this->polyLine->worldPosition;
		if (glm::dot(worldPos - p1, p2 - p1) * glm::dot(worldPos - p2, p1 - p2) > 0) {
			dist = distFromLine(worldPos, p1, p2);
		}
		else {
			dist = fmin(getLength(worldPos - p1), getLength(worldPos - p2));
		}

		if (dist <= 0.2) {
			flag = true;
			break;
		}
	}
	if (flag) {
		this->polyLine->isHovered = true;
		this->polyLine->color = { 1, 0, 0, 0.5 };
	}
	else {
		this->polyLine->isHovered = false;
		this->clickNum = 0;
		this->polyLine->color = { 1, 0, 0, 1 };
	}
}
///////////////////////////Selected///////////////////////////
void SVGElements::PolyLine::Selected::cursorPosCB(InputState* state){
	vec2 worldPos = this->polyLine->plotterApp->camera->getScreenData() + 
		state->curMousePos * this->polyLine->plotterApp->camera->zoomScale - vec2(this->polyLine->worldPosition);
	
	bool flag = false;
	float minDist = 0.2;
	int index = 0;
	float dist = 0;

	for (int i = 1; i < this->polyLine->vertices.size(); i++) {
		dist = getLength(vec2(worldPos) - vec2(this->polyLine->vertices[i]));
		if (minDist > dist) {
			minDist = dist;
			index = i;
		}
	}

	for (int i = 0; i < this->polyLine->vertices.size() - 1; i++) {
		vec2 p1 = this->polyLine->vertices[i];
		vec2 p2 = this->polyLine->vertices[i + 1];
		dist = getLength(worldPos - p1);
		if (minDist > dist) {
			minDist = dist;
			index = i;
		}
		if (glm::dot(worldPos - p1, p2 - p1) * glm::dot(worldPos - p2, p1 - p2) > 0) {
			dist = distFromLine(worldPos, p1, p2);
		}
		else {
			dist = fmin(getLength(worldPos - p1), getLength(worldPos - p2));
		}

		if (dist <= 0.2) {
			flag = true;
			break;
		}
	}

	if (minDist <= 0.2) {
		this->polyLine->hoveredVertexIndex = index;
	}
	else {
		this->polyLine->hoveredVertexIndex = -1;
	}
	if (flag) {
		this->polyLine->isHovered = true;
		this->polyLine->color = { 0, 1, 0, 0.5 };
	}
	else {
		this->polyLine->isHovered = false;
		this->polyLine->color = { 0, 1, 0, 1 };
	}
}