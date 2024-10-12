#pragma once
#include "Plotter.h"
#include "BaseClasses.h"
#include <serial/serial.h>
namespace SerialControllerNS {
	class SerialController;
	class State : public StateBase {
	public:
		SerialController* serialController;
	};
	class Idle : public State {
	public:
		void update(float deltaTime) override;
		void render() override;
	};
	class SerialController : public ObjectBase<State> {
	public:
		int baudRate;
		PlotterApp* plotterApp;
		serial::Serial* serialConnection;
		std::vector<serial::PortInfo> availablePorts;
		serial::PortInfo port;
		bool connected = false;
		std::string connectionStatus;

		SerialController(Engine* engine, PlotterApp* plotterApp, int baud);
		~SerialController();

		void update();
		void executeCode(std::string code);
	};
}
