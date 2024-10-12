#pragma once
#include "BaseClasses.h"
#include "Plotter.h"
namespace CameraNS {
	class Camera;

	class CameraState :public StateBase {
	public:
		int i;
		Camera* camera;
		void windowResizeCB(InputState* input) override;
		void scrollCB(InputState* input) override;
	};

	class Idle :public CameraState {
	public:
		void mouseButtonCB(InputState* state) override;
	};

	class Panning :public CameraState {
	public:
		void mouseButtonCB(InputState* state) override;
		void cursorPosCB(InputState* state) override;
	};

	class Camera:public ObjectBase<CameraState>{
	public:
		PlotterApp* plotterApp;

		vec3 worldPosition;
		int *wWidth, *wHeight;
		float zoomScale;

		mat4 VPMat;

		Idle* idle;
		Panning* panning;

		Camera(Engine* engine, PlotterApp* plotterApp);
		void calcVPMat();
		vec2 getScreenData();
	private:
		mat4 viewMat;
		mat4 projMat;
	};
}