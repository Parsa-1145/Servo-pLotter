#pragma once
#include "BaseClasses.h"
#include "Plotter.h"

namespace GridNS {
	class Grid;
	class GridState :public StateBase {
	public:
		Grid* grid;
	};
	class Idle :public GridState {
	public:
		void render() override;
	};

	class Grid : public ObjectBase<GridState> {
	public:
		PlotterApp* plotterApp;
		float baseWidth = 10;

		Idle* idle;

		unsigned int vbo;

		Grid(Engine* engine, PlotterApp* plotterApp);
	private:
		float vertices[16] = { -1, 1, 0, 1, 1, 0, 1, -1, 0, -1, -1, 0 };

		void initVbo();
	};
}