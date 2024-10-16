#pragma once
#include "Plotter.h"
#include "BaseClasses.h"
#include <serial/serial.h>

namespace SerialControllerNS {
	class SerialController;
	class State : public StateBase {
	public:
		SerialController* con;
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
		std::string strBuffer[10];
		float floatBuffer[10];

		std::chrono::time_point<std::chrono::high_resolution_clock> conStartTime;
		float curTime;
		

		int state[10];

		~SerialController();

		void init(Engine* engine, PlotterApp* plotterApp, int baud);
		void update();
		void executeCode(std::string code);
		void execute();
		void parse(std::string str);
		void sendMessages();
	};
}
extern SerialControllerNS::SerialController serialController;
