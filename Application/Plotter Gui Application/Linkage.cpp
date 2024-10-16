#include "Linkage.h"
#include "Camera.h"
#include "Arm.h"
#include "SceneController.h"
#include "SerialController.h"
#include "DataController.h"
#include "Globals.h"

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
	for (SVGElementBase* e : this->plotterApp->sceneController->svgElementList) {
		machineCodeMutex.lock();
		for (std::string str : e->getMachineCode()) {
			this->machineCode.push(str);
		}
		machineCodeMutex.unlock();
	}
}
void LinkageNS::Linkage::prepareMchineCode(SVGElementBase* element){
	machineCodeMutex.lock();
	for (std::string str : element->getMachineCode()) {
		this->machineCode.push(str);
	}
	machineCodeMutex.unlock();
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
		this->linkage->prepareMachineCode();
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
	if (this->linkage->canSendPos) {
		if (this->linkage->machineCode.size() != 0) {
			this->linkage->machineCodeMutex.lock();
			serialController.messageQueue.push(this->linkage->machineCode.front());
			this->linkage->machineCode.pop();
			this->linkage->canSendPos = false;
			this->linkage->machineCodeMutex.unlock();
		}
		else {
			this->linkage->curState = this->linkage->idle;
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
	if (ImGui::Button("Upload machine config")) {
		serialOutMutex.lock();
		serialController.messageQueue.push("c\n");
		serialController.messageQueue.push(std::to_string(1) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->speed) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->delay) + '\n');
		serialController.messageQueue.push("end\n");
		serialOutMutex.unlock();
	}
	ImGui::End();

	ImGui::Begin("Incoming data");
	if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, -1))) {
		ImPlot::SetupAxes("Time", "Value", 0, 0);
		dataInputMutex.lock();
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		for (int i = 0; i < dataController.datas.size(); i++) {
			ImPlot::PlotLine(dataController.datas[i].label.c_str(),
				&dataController.datas[i].scrVec.data[0].x, &dataController.datas[i].scrVec.data[0].y,
				dataController.datas[i].scrVec.data.size(), 0, dataController.datas[i].scrVec.pointer, 2 * sizeof(float));
		}
		dataInputMutex.unlock();
		ImPlot::EndPlot();
	}
	ImGui::End();
	ImGui::Begin("Motor config");
	ImGui::Text("Proportional Gain:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##Proportional gain", ImGuiDataType_Float, &this->linkage->propoGain, NULL, NULL, "%.2f");
	ImGui::Text("Integral Gain:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##Integral gain", ImGuiDataType_Float, &this->linkage->integGain, NULL, NULL, "%.2f");
	ImGui::Text("Derivative Gain:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##Derivative gain", ImGuiDataType_Float, &this->linkage->derivGain, NULL, NULL, "%.2f");
	ImGui::Text("Velocity smoothing factor:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##vel smooth", ImGuiDataType_Float, &this->linkage->velSmooth, NULL, NULL, "%.2f");
	if (this->linkage->velSmooth > 1) this->linkage->velSmooth = 1;
	if (this->linkage->velSmooth < 0) this->linkage->velSmooth = 0;
	ImGui::Text("offset:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##offset", ImGuiDataType_Float, &this->linkage->offset, NULL, NULL, "%.2f");
	ImGui::PushItemWidth(-1);
	ImGui::PushItemWidth(-1);
	if (ImGui::Button("Upload motor config")) {
		serialOutMutex.lock();
		serialController.messageQueue.push("c\n");
		serialController.messageQueue.push(std::to_string(0) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->propoGain) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->derivGain) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->integGain) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->offset) + '\n');
		serialController.messageQueue.push(std::to_string(this->linkage->velSmooth) + '\n');
		serialController.messageQueue.push("end\n");
		serialOutMutex.unlock();
	}
	ImGui::End();
}
