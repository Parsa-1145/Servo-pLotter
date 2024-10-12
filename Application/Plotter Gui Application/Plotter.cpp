#include "Plotter.h"
#include "Arm.h"
#include "Camera.h"
#include "Grid.h"
#include "SVGController.h"
#include "Linkage.h"
#include "selectionRect.h"
#include "SceneController.h"
#include "SerialController.h"

PlotterApp::PlotterApp(Engine* engine){
	this->engine = engine;

	this->arm1 = new Arm(this->engine, this, { 8 , 8  }, { -2.2265 , 0, 0 });
	this->arm2 = new Arm(this->engine, this, { 8 , 8  }, { 2.2265, 0, 0 });
	this->arm2->reversed = true;
	this->linkage = new Linkage(this->engine, this, this->arm1, this->arm2);

	this->camera = new Camera(engine, this);
	this->grid   = new Grid(engine, this);
	this->svgController = new SVGController(engine, this);
	this->selectionRect = new SelectionRect(engine, this);
	this->sceneController = new SceneController(engine, this);
	this->serialController = new SerialController(engine, this, 19200);
}

PlotterApp::~PlotterApp(){
	cout << "deleting these bitches" << endl;
	delete this->arm1;
	delete this->arm2;
	delete this->camera;
	delete this->grid;
	delete this->svgController;
	delete this->sceneController;
	delete this->linkage;
	delete this->selectionRect;
}
