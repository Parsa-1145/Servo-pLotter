#pragma once
#include "BaseClasses.h"
#include "Plotter.h"
namespace ArmNS {
	class Arm;

	class ArmState :public StateBase {
	public:
		Arm* arm;
		void render() override;
	};

	class Idle : public ArmState {
	};
	class Arm : public ObjectBase<ArmState> {
	public:
		PlotterApp* plotterApp;
		vec2 segLengths;
		vec3 worldPosition;
		std::vector<vec3> vertices;
		mat4 modelMat;
		bool reversed = false;
		float speed = 1;

		unsigned int vbo;
		unsigned int program;
		unsigned int vao;
		void initVbo();

		Idle* idle;

		Arm(Engine* engine, PlotterApp* plotterApp, vec2 segLengths, vec3 position);
		void updateVbo();
		void moveTo(vec2 position);
		bool canMoveTo(vec2 position);
	};
}
