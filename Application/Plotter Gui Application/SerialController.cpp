#include "SerialController.h"
#include "Linkage.h"
#include "Globals.h"
SerialControllerNS::Idle idle;
SerialControllerNS::SerialController::SerialController(Engine* engine, PlotterApp* plotterApp, int baud){
	this->baudRate = baud;
	this->engine = engine;
	this->plotterApp = plotterApp;
	this->curState = &idle;
	idle.serialController = this;
	this->initCallbacks();

	/*cout << "Choose which Port to use:" << endl;
	std::vector<serial::PortInfo> ports = serial::list_ports();
	for (int i = 0; i < ports.size(); i++){
		cout << i+1 << ": " << ports[i].port << " | ";
	}
	cout << endl;

	int selectedPort = 0;
	std::cin >> selectedPort;

	serialConnection = new serial::Serial(ports[selectedPort-1].port, this->baudRate, serial::Timeout::simpleTimeout(3000));*/
}

SerialControllerNS::SerialController::~SerialController(){
	delete this->serialConnection;
	this->serialConnection->close();
}

void SerialControllerNS::SerialController::executeCode(std::string code){
	int lines = 0;
	bool ready = false;
	std::string tmp;
	char inChar = ' ';
	for(char c: code) {
		if (c == '\n') lines += 1;
		this->serialConnection->write(std::string(1, c));
		if (lines == 3) {
			lines = 0;
			tmp = "";
			vec2 go = { 0, 0 };
			int k = 0;
			while (true) {
				
				if (this->serialConnection->available()) {
					inChar = this->serialConnection->read(1)[0];
					if (inChar == '\n') {
						
						if (tmp == std::string("canReceive")) {
							break;
						}
						else {
							if (k == 2) {
								k = 0;
								this->plotterApp->linkage->moveTo(go);	
								cout << "1: " <<go[0]<<" 2: "<<go[1]<<endl;
							}
							go[k] = std::stof(tmp);
							k++;
							tmp = "";
						}
					}
					else {
						tmp += inChar;
					}
				}
			}
		}
	}
}

void SerialControllerNS::SerialController::execute(std::string str){

}

void SerialControllerNS::SerialController::sendMessages(){
	serialOutMutex.lock();
	while (this->messageQueue.size() != 0) {
		this->serialConnection->write(this->messageQueue.front());
		this->messageQueue.pop();
	}
	serialOutMutex.unlock();
}

void SerialControllerNS::Idle::update(float deltaTime){
	SerialController* con = this->serialController;
	if (con->connected) {
		while (con->serialConnection->available()) {
			con->inChar = con->serialConnection->read(1)[0];
			switch (con->inChar){
			case '\n':
				con->sendMessages();
				con->parse(con->inStr);
				con->inStr = "";
				break;
			case '\t':
				con->parse(con->inStr);
				con->inStr = "";
				break;
			default:
				con->inStr += con->inChar;
				break;
			}
		}
	}
}

void SerialControllerNS::Idle::render() {
	ImGui::Begin("Serial connection");
	if (this->serialController->connected) {
		ImGui::TextColored(ImVec4(0, 1, 0, 1), this->serialController->connectionStatus.c_str());
	}
	else {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: Not connected");
	}
	
	if (ImGui::Button("Check Ports")) {
		this->serialController->availablePorts = serial::list_ports();
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::BeginCombo("##portCombo", this->serialController->port.port.c_str())) {
		for (serial::PortInfo& p : this->serialController->availablePorts) {
			if (ImGui::Selectable(p.port.c_str(), false)) {
				this->serialController->port = p;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Text("Baud rate:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##baud rate", ImGuiDataType_U32, &this->serialController->baudRate);
	ImGui::PushItemWidth(-1);
	if (ImGui::Button("Connect / disconnect")) {
		if (!this->serialController->connected) {
			try {
				this->serialController->serialConnection = new serial::Serial(this->serialController->port.port, this->serialController->baudRate, serial::Timeout::simpleTimeout(3000));
				if (this->serialController->serialConnection->isOpen()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					this->serialController->serialConnection->flushInput();
					this->serialController->connected = true;
					this->serialController->connectionStatus = "Status: Connected to " + this->serialController->port.port;
				}
			}
			catch (...) {
				ImGui::OpenPopup("CannotConnectPopup");
				this->serialController->connected = false;
			}
		}
		else {
			this->serialController->connected = false;
			this->serialController->serialConnection->close();
		}
	}
	if (ImGui::BeginPopup("CannotConnectPopup")) {
		std::string tmp = "Cannot establish a serial connection to port " + this->serialController->port.port;
		ImGui::Text(tmp.c_str());
		ImGui::EndPopup();
	}
	ImGui::End();
}
