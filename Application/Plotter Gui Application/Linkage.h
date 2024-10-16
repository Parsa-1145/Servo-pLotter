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
		float speed = 0.05;
		unsigned int delay = 3;

		float propoGain = 15;
		float integGain = 0;
		float derivGain = 30;
		float offset = 0;
		float velSmooth = 0;

		bool canSendPos = true;

		std::queue<std::string> machineCode;
		std::mutex machineCodeMutex;

		Linkage(Engine* engine, PlotterApp* plotterApp, Arm* arm1, Arm* arm2);
		void moveTo(vec2 position);
		void prepareMachineCode();
		void prepareMchineCode(SVGElementBase* element);

	private:
		void initVbo();
		void initValidArea();
	};
}