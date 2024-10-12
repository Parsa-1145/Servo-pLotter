#pragma once
#include "BaseClasses.h"
#include "Plotter.h"

class Arm;

namespace LinkageNS {
	class Linkage;
	class LinkageState : public StateBase {
	public:
		Linkage* linkage;

		void render() override;
	};
	class Idle : public LinkageState {
	public:
		void keyboardCB(InputState* state) override;
	};
	class Following : public LinkageState {
	public:
		void cursorPosCB(InputState* state) override;
		void keyboardCB(InputState* state) override;
	};
	class Executing : public LinkageState {
	public:
		int curElementIndex = 0;

		void init();
		void update(float deltaTime) override;
	};
	class Linkage : public ObjectBase<LinkageState> {
	public:
		PlotterApp* plotterApp;
		std::vector<Arm*> arms;
		vec2 curPos;

		Idle* idle;
		Following* following;
		Executing* excuting;

		unsigned int vbo;
		unsigned int textureId;
		mat4 worldPosMat;
		std::vector<vec3> boundingBoxVertices;
		float speed = 12000;

		Linkage(Engine* engine, PlotterApp* plotterApp, Arm* arm1, Arm* arm2);
		void moveTo(vec2 position);
		void prepareMachineCode();

	private:
		void initVbo();
		void initValidArea();
	};
}