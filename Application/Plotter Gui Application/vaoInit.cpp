#include "Renderer.h"

void Renderer::initVaos() {
	glGenVertexArrays(1, &this->vaoList.simpleLine);
	glBindVertexArray(this->vaoList.simpleLine);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	unsigned int indices1[6] = { 0, 1, 3, 1, 2, 3 };
	unsigned int EBO1;
	glGenVertexArrays(1, &this->vaoList.simpleQuad);
	glBindVertexArray(this->vaoList.simpleQuad);
	glGenBuffers(1, &EBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &this->vaoList.texturedQuad);
	glBindVertexArray(this->vaoList.texturedQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 1);
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 12);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}