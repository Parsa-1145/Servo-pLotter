#include "Linkage.h"
#include "Camera.h"
#include "Arm.h"
#include "SceneController.h"
#include "SerialController.h"

LinkageNS::Linkage::Linkage(Engine* engine, PlotterApp* plotterApp, Arm* arm1, Arm* arm2){
	this->engine = engine;
	this->plotterApp = plotterApp;
	this->arms.push_back(arm1);
	this->arms.push_back(arm2);

	this->idle = new Idle();
	this->following = new Following();
	this->excuting = new Executing();
	this->idle->linkage = this;
	this->following->linkage = this;
	this->excuting->linkage = this;

	this->curState = this->idle;

	this->initCallbacks();

	float boundngBox[3] = { 0, 0 , 0 };
	for (Arm* arm : this->arms) {
		if (boundngBox[0] > (arm->worldPosition[0] - arm->segLengths[0] - arm->segLengths[1])) {
			boundngBox[0] = arm->worldPosition[0] - arm->segLengths[0] - arm->segLengths[1];
		}if (boundngBox[1] < (arm->worldPosition[0] + arm->segLengths[0] + arm->segLengths[1])) {
			boundngBox[1] = (arm->worldPosition[0] + arm->segLengths[0] + arm->segLengths[1]);
		}if (boundngBox[2] < (arm->worldPosition[1] + arm->segLengths[0] + arm->segLengths[1])) {
			boundngBox[2] = arm->worldPosition[1] + arm->segLengths[0] + arm->segLengths[1];
		}
	}
	this->boundingBoxVertices.push_back({ boundngBox[0], boundngBox[2], 0.01});
	this->boundingBoxVertices.push_back({ boundngBox[1], boundngBox[2],  0.01 });
	this->boundingBoxVertices.push_back({ boundngBox[1], 0, 0.01});
	this->boundingBoxVertices.push_back({ boundngBox[0], 0,0.01});

	this->initVbo();
	this->initValidArea();

}
void LinkageNS::Linkage::moveTo(vec2 position){
	for (Arm* arm : this->arms) {
		if (!arm->canMoveTo(position)) {
			return;
		}
	}

	for (Arm* arm : this->arms) {
		arm->moveTo(position);
	}
}
void LinkageNS::Linkage::prepareMachineCode(){
	std::string machinCode = "";
	for (SVGElementBase* e : this->plotterApp->sceneController->svgElementList) {
		machinCode += e->getMachineCode();
	}
	this->plotterApp->serialController->executeCode(machinCode);
}
void LinkageNS::Linkage::initVbo(){
	vec2 uvs[4] = { {0, 1}, {1, 1}, { 1, 0}, {0, 0} };
	std::vector<float> tmp;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++){
			tmp.push_back(this->boundingBoxVertices[i][j]);
		}
		for (int j = 0; j < 2; j++) {
			tmp.push_back(uvs[i][j]);
		}
	}
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * tmp.size(), &(tmp[0]), GL_STATIC_DRAW);

	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, &tmp[0], tmp.size() * 4);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void LinkageNS::Linkage::initValidArea(){
	float boundngBox[3] = { 0, 0, 0};
	for (Arm* arm : this->arms) {
		if (boundngBox[0] > (arm->worldPosition[0] - arm->segLengths[0] - arm->segLengths[1])) {
			boundngBox[0] = arm->worldPosition[0] - arm->segLengths[0] - arm->segLengths[1];
		}if (boundngBox[1] < (arm->worldPosition[0] + arm->segLengths[0] + arm->segLengths[1])) {
			boundngBox[1] = (arm->worldPosition[0] + arm->segLengths[0] + arm->segLengths[1]);
		}if (boundngBox[2] < (arm->worldPosition[1] + arm->segLengths[0] + arm->segLengths[1])) {
			boundngBox[2] = arm->worldPosition[1] + arm->segLengths[0] + arm->segLengths[1];
		}
	}
	float width = boundngBox[1] - boundngBox[0];
	float height = boundngBox[2];
	mat4 projecMat = {	2/width, 0, 0, 0,
						0, 2/height, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1};
	mat4 viewMat = { 1, 0, 0, 0,
					 0, 1, 0, -height/2,
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	mat4 VPMat = viewMat * projecMat;
	this->worldPosMat = { 1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1 };

	GLuint tmpFramebuffer;
	glGenFramebuffers(1, &tmpFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, tmpFramebuffer);

	glGenTextures(1, &this->textureId);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width * 100, height * 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textureId, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Failed to create a framebuffer";
	}
	glViewport(0, 0, width * 100, height * 100);

	glBindVertexArray(this->engine->renderer.vaoList.texturedQuad);

	glUseProgram(this->engine->renderer.programList.validAreaCompute.id);
	glBindVertexBuffer(1, this->vbo, 0, 5 * sizeof(float));
	glUniformMatrix4fv(this->engine->renderer.programList.validAreaCompute.viewProjecUni, 1, GL_TRUE,
		&VPMat[0][0]);
	glUniform4f(this->engine->renderer.programList.validAreaCompute.origData, this->arms[0]->worldPosition[0],
		this->arms[0]->worldPosition[1], this->arms[1]->worldPosition[0], this->arms[1]->worldPosition[1]);
	glUniform2f(this->engine->renderer.programList.validAreaCompute.lengthData, this->arms[0]->segLengths[0],
		this->arms[0]->segLengths[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	

	this->engine->renderer.shouldResizeViewport = true;
}
///////////// Idle /////////////
void LinkageNS::Idle::keyboardCB(InputState* state) {

	if (state->keyPressed(GLFW_KEY_A)) {
		this->linkage->excuting->init();
		this->linkage->curState = this->linkage->excuting;
	}
	else if (state->keyPressed(GLFW_KEY_M)) {
		if (this->linkage->plotterApp->sceneController->selectedEntities.size() == 0) {
			this->linkage->curState = this->linkage->following;
		}
	}
	else if (state->keyPressed(GLFW_KEY_E)) {
		this->linkage->prepareMachineCode();
	}
}
///////////// Following /////////////
void LinkageNS::Following::cursorPosCB(InputState* state) {
	this->linkage->moveTo(this->linkage->plotterApp->camera->getScreenData() + state->curMousePos * this->linkage->plotterApp->camera->zoomScale);
}

void LinkageNS::Following::keyboardCB(InputState* inputState) {
	if (inputState->keyboardPress.key == GLFW_KEY_M && inputState->keyboardPress.action == 1) {
		this->linkage->curState = this->linkage->idle;
	}
}
void LinkageNS::Executing::init() {
	if (this->linkage->plotterApp->sceneController->svgElementList.size() != 0) {
		this->linkage->plotterApp->sceneController->svgElementList[0]->initExecution();
	}
}
void LinkageNS::Executing::update(float deltaTime) {
	moveData data = this->linkage->plotterApp->sceneController->svgElementList[this->curElementIndex]->moveOnShape(this->linkage->speed * (deltaTime / 1000));
	
	this->linkage->moveTo(data.position);
	
	if (data.finished) {
		this->curElementIndex += 1;
		if (this->linkage->plotterApp->sceneController->svgElementList.size() == this->curElementIndex) {
			this->curElementIndex = 0;
			this->linkage->curState = this->linkage->idle;
		}
		else {
			this->linkage->plotterApp->sceneController->svgElementList[this->curElementIndex]->initExecution();
		}
	}
}

void LinkageNS::LinkageState::render(){
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, this->linkage->textureId);
	glBindVertexArray(this->linkage->engine->renderer.vaoList.texturedQuad);
	glUseProgram(this->linkage->engine->renderer.programList.texturedQuad.id);
	glBindVertexBuffer(1, this->linkage->vbo, 0, 5 * sizeof(float));
	glUniformMatrix4fv(this->linkage->engine->renderer.programList.texturedQuad.viewProjecUni, 1, GL_TRUE,
		&this->linkage->plotterApp->camera->VPMat[0][0]);
	glUniformMatrix4fv(this->linkage->engine->renderer.programList.texturedQuad.modelUni, 1, GL_TRUE,
		&this->linkage->worldPosMat[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);

	ImGui::Begin("Machine config");
	ImGui::Text("Speed:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##speed", ImGuiDataType_Float, &this->linkage->speed, 0, 0, "%.2f", 0);
	ImGui::Text("Delay:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##delay", ImGuiDataType_U16, &this->linkage->delay);
	ImGui::Button("Upload machine config");
	ImGui::End();
}
