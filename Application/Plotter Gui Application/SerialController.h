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
		std::queue<std::string> messageQueue;

		char inChar;
		std::string inStr;

		int state;

		SerialController(Engine* engine, PlotterApp* plotterApp, int baud);
		~SerialController();

		void update();
		void executeCode(std::string code);
		void parse(std::string str);
		void sendMessages();
	};
}
