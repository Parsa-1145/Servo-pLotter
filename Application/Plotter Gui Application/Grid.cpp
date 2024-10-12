#include "Grid.h"
#include "Camera.h"

GridNS::Grid::Grid(Engine* engine, PlotterApp* plotterApp){
	this->engine = engine;
	this->plotterApp = plotterApp;

	this->idle = new Idle();
	this->idle->grid = this;
	this->curState = this->idle;

	this->initCallbacks();
	this->initVbo();

	this->baseWidth = 10;
}

void GridNS::Grid::initVbo(){
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, 48, this->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GridNS::Idle::render(){
	while (true) {
		if (this->grid->baseWidth / this->grid->plotterApp->camera->zoomScale < 5) {
			this->grid->baseWidth *= 10;
		}
		else if(this->grid->baseWidth / this->grid->plotterApp->camera->zoomScale > 50){
			this->grid->baseWidth /= 10;
		}
		else {
			break;
		}
	}
	glBindVertexArray(this->grid->engine->renderer.vaoList.simpleQuad);
	glUseProgram(this->grid->engine->renderer.programList.grid.id);
	glBindVertexBuffer(0, this->grid->vbo, 0, 12);
	vec2 screenData = this->grid->plotterApp->camera->getScreenData();
	glUniform4f(this->grid->engine->renderer.programList.grid.screenCordUni,screenData[0], screenData[1], 
		this->grid->plotterApp->camera->zoomScale, this->grid->plotterApp->camera->zoomScale / this->grid->baseWidth );
	glUniform2f(this->grid->engine->renderer.programList.grid.scrSizeUni, *this->grid->plotterApp->camera->wWidth, 
		*this->grid->plotterApp->camera->wHeight);
	glUniform1f(this->grid->engine->renderer.programList.grid.baseWidthUni, this->grid->baseWidth);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
