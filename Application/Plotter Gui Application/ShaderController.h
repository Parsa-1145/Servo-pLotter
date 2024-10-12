#pragma once
#include "common.h"

class ShaderController {
public:
	void createProgram(unsigned int* program, unsigned int vertShader, unsigned int fragShader);
	void createShader(std::string path, GLuint type, unsigned int* shaderObject);
private:
	std::vector<std::string*> shaderSources;

	std::string loadFile(std::string path);
	void shaderCompileCheck(unsigned int shaderId);
	void programLinkCheck(unsigned int programId);
};