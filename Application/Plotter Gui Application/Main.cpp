#include"Engine.h"
#include "Arm.h"
#include "Plotter.h"

int main() {
	Engine app;
	app.init(600, 600, "Plotter");
	PlotterApp plotterApp(&app);
	app.eventLoop();
	return 0;
}