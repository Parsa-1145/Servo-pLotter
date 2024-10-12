#pragma once
#include "Common.h"
#include "Engine.h"
#include "SVGElementBase.h"

namespace ArmNS { class Arm; }
namespace CameraNS{ class Camera; }
namespace GridNS { class Grid; }
namespace SVGControllerNS { class SVGController; }
namespace LinkageNS { class Linkage; }
namespace SelectionRectNS { class SelectionRect; }
namespace SceneControllerNS { class SceneController; }
namespace SerialControllerNS { class SerialController; }

using ArmNS::Arm;
using CameraNS::Camera;
using GridNS::Grid;
using SVGControllerNS::SVGController;
using LinkageNS::Linkage;
using SelectionRectNS::SelectionRect;
using SceneControllerNS::SceneController;
using SerialControllerNS::SerialController;

class PlotterApp {
public:
	Engine* engine;

	Arm* arm1;
	Arm* arm2;
	Linkage* linkage;

	Grid* grid;
	Camera* camera;
	SelectionRect* selectionRect;

	SVGController* svgController;
	SceneController* sceneController;
	SerialController* serialController;

	PlotterApp(Engine* engine);
	~PlotterApp();
};