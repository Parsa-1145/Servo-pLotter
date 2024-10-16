#pragma once
#include "BaseClasses.h"
#include "Plotter.h"

namespace SceneControllerNS {
	class SceneController;
	class State :public StateBase {
	public:
		SceneController* sceneController;
		void render() override;
	private:
		char buffer[10];
	};
	class Idle :public State {
	public:
		bool hoveringSelected = false;
		void mouseButtonCB(InputState* state) override;
		void cursorPosCB(InputState* state) override;
	};
	class Moving : public State {
	public:
		vec2 startPos;
		vec2 prevWorldPos;
		void cursorPosCB(InputState* state) override;
		void mouseButtonCB(InputState* state) override;
	};
	class SceneController :public ObjectBase<State>{
	public:
		PlotterApp* plotterApp;
		std::set<SVGElementBase*> selectedEntities;
		std::vector<SVGElementBase*> svgElementList;

		Idle* idle;
		Moving* moving;

		SceneController(Engine* engine, PlotterApp* plotterApp);
		bool isSelected(SVGElementBase* element);
	};
}
