#include "ShaderController.h"

void ShaderController::createProgram(unsigned int* program, unsigned int vertShader, unsigned int fragShader) {
	unsigned int programId = glCreateProgram();

	glAttachShader(programId, vertShader);
	glAttachShader(programId, fragShader);
	glLinkProgram(programId);
	this->programLinkCheck(programId);
	
	*program = programId;

	//cout << "created program of type " << programType << " at id " << this->programIndexMap[programType] << endl;
}
void ShaderController::createShader(std::string path, GLuint type, unsigned int* shaderObject) {
	std::string tmp = this->loadFile(path);
	this->shaderSources.push_back(&tmp);
	const char* source = tmp.c_str();

	unsigned int shaderId;
	shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);

	this->shaderCompileCheck(shaderId);

	*shaderObject = shaderId;
	//cout << "created shader of type " << type << " at id " << this->shaderList[this->shaderIndexMap[type]] << " from the file " << path << endl;
}

std::string ShaderController::loadFile(std::string path) {
	std::ifstream shaderFile(path);
	if (!shaderFile.is_open()) {
		cout << "failed to open " << path << " ." << endl;
		return NULL;
	}
	std::string out;
	std::string tmp;
	while (std::getline(shaderFile, tmp)) {
		out += tmp + '\n';
	}
	shaderFile.close();
	return out;
}

void ShaderController::shaderCompileCheck(unsigned int shaderId) {
	int suc;
	char log[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &suc);
	if (!suc) {
		glGetShaderInfoLog(shaderId, 512, NULL, log);
		std::cout << "Failed to compile the shader\n" << log << std::endl;
	}
}
void ShaderController::programLinkCheck(unsigned int programId){
	int suc;
	char log[512];
	glGetProgramiv(programId, GL_COMPILE_STATUS, &suc);
	if (!suc) {
		glGetProgramInfoLog(programId, 512, NULL, log);
		std::cout << "Failed to link the program\n" << log << std::endl;
	}
}
