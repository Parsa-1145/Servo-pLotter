#include "Renderer.h"

void Renderer::initShaders() {
	this->shaderController.createShader("SimpleVertShader.glsl", GL_VERTEX_SHADER, &this->shaderList.simpleLine_vs);
	this->shaderController.createShader("simpleFragShader.glsl", GL_FRAGMENT_SHADER, &this->shaderList.simpleLine_fs);
	this->shaderController.createShader("GridFragShader.glsl", GL_FRAGMENT_SHADER, &this->shaderList.grid_fs);
	this->shaderController.createShader("fullScreenVert.glsl", GL_VERTEX_SHADER, &this->shaderList.fullScreen_vs);
	this->shaderController.createShader("absQuadVert.glsl", GL_VERTEX_SHADER, &this->shaderList.absQuad_vs);
	this->shaderController.createShader("validAreaFrag.glsl", GL_FRAGMENT_SHADER, &this->shaderList.validArea_fs);
	this->shaderController.createShader("texturedQuadVert.glsl", GL_VERTEX_SHADER, &this->shaderList.texturedQuad_vs);
	this->shaderController.createShader("texturedQuadFrag.glsl", GL_FRAGMENT_SHADER, &this->shaderList.texturedQuad_fs);


	this->shaderController.createProgram(&this->programList.simpleLine.id, this->shaderList.simpleLine_vs,
		this->shaderList.simpleLine_fs);
	this->programList.simpleLine.initUniforms();

	this->shaderController.createProgram(&this->programList.grid.id, this->shaderList.fullScreen_vs,
		this->shaderList.grid_fs);
	this->programList.grid.initUniforms();

	this->shaderController.createProgram(&this->programList.validAreaCompute.id, this->shaderList.absQuad_vs, this->shaderList.validArea_fs);
	this->programList.validAreaCompute.initUniforms();

	this->shaderController.createProgram(&this->programList.texturedQuad.id, this->shaderList.texturedQuad_vs, this->shaderList.texturedQuad_fs);
	this->programList.texturedQuad.initUniforms();
}