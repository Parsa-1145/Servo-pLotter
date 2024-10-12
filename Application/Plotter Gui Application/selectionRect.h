#pragma once
#include "BaseClasses.h"
#include "Plotter.h"
namespace SelectionRectNS {
	class SelectionRect;
	class State : public StateBase {
	public:
		SelectionRect* rect;
	};
	class Idle: public State {
	public:
		void mouseButtonCB(InputState* state) override;
	};
	class Selecting : public State {
	public:
		vec2 startPoint;
		vec2 endPoint;
		std::set<SVGElementBase*> startingSelections;
		void mouseButtonCB(InputState* state) override;
		void cursorPosCB(InputState* state) override;
		void render() override;
		void updateQuad();
	private:
		vec2 xRange = { 0, 0 };
		vec2 yRange = { 0, 0 };

		bool wasSelected(SVGElementBase* e);
		
	};
	class SelectionRect: public ObjectBase<State> {
	public:
		PlotterApp* plotterApp;
		unsigned int vbo;
		std::vector<vec3> vertices;
		mat4 modelMat = mat4(1);
		

		Idle idle;
		Selecting selecting;

		SelectionRect(Engine* engine, PlotterApp* plotterApp);
		void updateVbo();
	private:
		void initVbo();
		
	};
}