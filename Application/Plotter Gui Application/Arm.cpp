#include "Arm.h"
#include "Camera.h"
#include "PolyLine.h"

ArmNS::Arm::Arm(Engine* engine, PlotterApp* plotterApp, vec2 segLengths, vec3 position) {
	this->engine = engine;
	this->plotterApp = plotterApp;
	this->segLengths = segLengths;
	this->worldPosition = position;
	this->vertices.push_back({ 0, 0, 0});
	this->vertices.push_back({ 0, this->segLengths[0], 0 });
	this->vertices.push_back({ 0, this->segLengths[0] + this->segLengths[1], 0});
	this->modelMat = {  1, 0, 0, this->worldPosition[0],
						0, 1, 0, this->worldPosition[1],
						0, 0, 1, this->worldPosition[2],
						0, 0, 0, 1 };
	/*this->vertices.push_back({ 0, 0, 0 });
	this->vertices.push_back({ 0.5, 0, 0 });
	this->vertices.push_back({ 0.5, 0.5, 0 });*/


	this->idle = new Idle();
	this->idle->arm = this;

	this->curState = this->idle;

	this->initCallbacks();

	this->initVbo();
}

void ArmNS::Arm::updateVbo(){
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, &this->vertices[0], this->vertices.size() * sizeof(vec3));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void ArmNS::Arm::moveTo(vec2 position){
	Camera* camera = this->plotterApp->camera;

	vec2 dist = position - vec2(this->worldPosition[0], this->worldPosition[1]);

	float z = hypot(dist[0], dist[1]);
	float a1 = std::atan(dist[1] / dist[0]);
	float a2 = std::acos((z / 2) / this->segLengths[0]);
	if (a1 < 0) a1 += pi;
	if (this->reversed) a2 *= -1;

	this->vertices[1][0] = this->vertices[0][0] + this->segLengths[0] * cos(a1 + a2);
	this->vertices[1][1] = this->vertices[0][1] + this->segLengths[0] * sin(a1 + a2);

	this->vertices[2][0] = dist[0];
	this->vertices[2][1] = dist[1];
	this->updateVbo();
}
bool ArmNS::Arm::canMoveTo(vec2 position){
	vec2 dist = position - vec2(this->worldPosition[0], this->worldPosition[1]);
	return hypot(dist[0], dist[1]) < this->segLengths[0] + this->segLengths[1];
}
void ArmNS::Arm::initVbo(){
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * this->vertices.size() * sizeof(float), &(this->vertices[0]), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ArmNS::ArmState::render() {
	glBindVertexArray(this->arm->engine->renderer.vaoList.simpleLine);
	
	glUseProgram(this->arm->engine->renderer.programList.simpleLine.id);
	glBindVertexBuffer(0, this->arm->vbo, 0, 3 * sizeof(float));
	glUniformMatrix4fv(this->arm->engine->renderer.programList.simpleLine.viewProjecUni, 1, GL_TRUE,
		&this->arm->plotterApp->camera->VPMat[0][0]);
	glUniformMatrix4fv(this->arm->engine->renderer.programList.simpleLine.modelUni, 1, GL_TRUE,
		&(this->arm->modelMat[0][0]));
	glUniform4f(this->arm->engine->renderer.programList.simpleLine.colorUni, 1.0f, 0.5f, 0.2f, 1.0f);
	glDrawArrays(GL_LINE_STRIP, 0, 3);
	glPointSize(10);
	glDrawArrays(GL_POINTS, 1, 1);
}
