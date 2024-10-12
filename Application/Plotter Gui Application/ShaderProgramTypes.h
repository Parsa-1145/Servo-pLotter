#pragma once
#include "Common.h"
struct simpleLine_SP {
	unsigned int id;
	
	unsigned int viewProjecUni;
	unsigned int modelUni;
	unsigned int colorUni;

	void initUniforms() {
		this->modelUni = glGetUniformLocation(this->id, "M");
		this->viewProjecUni = glGetUniformLocation(this->id, "VP");
		this->colorUni = glGetUniformLocation(this->id, "Color");
	}
};
struct grid_SP {
	unsigned int id;

	unsigned int screenCordUni;
	unsigned int scrSizeUni;
	unsigned int baseWidthUni;

	void initUniforms() {
		this->screenCordUni = glGetUniformLocation(this->id, "screenPos");
		this->scrSizeUni = glGetUniformLocation(this->id, "scrSize");
		this->baseWidthUni = glGetUniformLocation(this->id, "baseWidth");
	}
};
struct validAreaCompute_SP {
	unsigned int id;
	unsigned int viewProjecUni;
	unsigned int lengthData;
	unsigned int origData;

	void initUniforms() {
		this->origData = glGetUniformLocation(this->id, "origData");
		this->lengthData = glGetUniformLocation(this->id, "lengthData");
		this->viewProjecUni = glGetUniformLocation(this->id, "VP");
	}
};
struct texturedQuaad_SP {
	unsigned int id;

	unsigned int viewProjecUni;
	unsigned int modelUni;
	unsigned int textureUni;

	void initUniforms() {
		this->modelUni = glGetUniformLocation(this->id, "M");
		this->viewProjecUni = glGetUniformLocation(this->id, "VP");
		this->textureUni = glGetUniformLocation(this->id, "tex");
	}
};
struct ShaderProgramList{
	simpleLine_SP simpleLine;
	grid_SP grid;
	validAreaCompute_SP validAreaCompute;
	texturedQuaad_SP texturedQuad;
};