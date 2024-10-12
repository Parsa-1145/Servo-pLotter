#pragma once
#include "BaseClasses.h"
#include "Plotter.h"

namespace SVGElements {
	namespace PolyLine {
		class PolyLine;
		class PolyLineState :public StateBase {
		public:
			PolyLine* polyLine;
			void render() override;
		};
		class Idle:public PolyLineState{
		public:
			int clickNum = 0;
			void cursorPosCB(InputState* state) override;
		};
		class Selected : public PolyLineState {
		public:
			void cursorPosCB(InputState* state) override;
		private:
			bool leaveClickCheck = 0;
			std::chrono::time_point<std::chrono::steady_clock> startTime;
		};
		class PolyLine :public ObjectBase<PolyLineState>, public SVGElementBase {
		public:
			PlotterApp* plotterApp;
			std::vector<vec3> vertices;
			mat4 modelMat;
			int targetIndex = 0;
			vec3 curPos;
			vec4 color = { 1, 0, 0, 1};

			Idle* idle;
			Selected* selected;

			unsigned int vbo;

			PolyLine(Engine* engine, PlotterApp* plotterApp, std::vector<vec3>* vertices);
			void initVbo();
			void updateModelMat();
			moveData moveOnShape(float moveDistance) override;
			void initExecution() override;
			void translate(vec2 transVec) override;
			void select() override;
			void deselect() override;
			vec3 getHoveringVertexPosition() override;
			bool isInBox(vec2 xRange, vec2 yRange);
			std::string getMachineCode();
		};
	}
}
using SVGElements::PolyLine::PolyLine;