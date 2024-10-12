#pragma once
#include "Common.h"
#include "ShaderController.h"
#include "ShaderTypes.h"
#include "ShaderProgramTypes.h"
#include "VaoTypes.h"

class Renderer {
public:
	GLFWwindow* window;
	ShaderController shaderController;
	ShaderList shaderList;
	ShaderProgramList programList;
	VaoList vaoList;

	mat4* VPMat;
	int wWidth, wHeight;

	signal<void()> renderSignal;

	bool initDone = false;
	bool* shouldTerminate;
	bool* shouldStart;
	bool shouldResizeViewport = false;

	std::mutex* locker;

	Renderer();
	void init(GLFWwindow* window);

private:
	void renderLoop();
	void initVaos();
	void initShaders();
};