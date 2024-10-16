#include "SerialController.h"
#include "Linkage.h"
#include "Globals.h"
#include "DataController.h"


SerialControllerNS::SerialController serialController;

SerialControllerNS::Idle idle;
void SerialControllerNS::SerialController::init(Engine* engine, PlotterApp* plotterApp, int baud){
	this->baudRate = baud;
	this->engine = engine;
	this->plotterApp = plotterApp;
	this->curState = &idle;
	idle.con = this;
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

}

void SerialControllerNS::SerialController::execute(){
	switch (state[0]){
		case 1:
			dataInputMutex.lock();
			dataController.addData(this->strBuffer[0], this->curTime, this->floatBuffer[0]);
			dataInputMutex.unlock();
			break;
		default:
			break;
	}
}

void SerialControllerNS::SerialController::parse(std::string str){
	switch (state[0]){
		case 0:
			if (std::strcmp(str.c_str(), "d") == 0) {
				this->state[0] = 1;
				this->state[1] = 0;
			}else if (std::strcmp(str.c_str(), "l")==0) {
				state[0] = 2;
			}
			else if (std::strcmp(str.c_str(), "r") == 0) {
				this->plotterApp->linkage->canSendPos = true;
			}
			else if (std::strcmp(str.c_str(), "m") == 0) {
				state[0] = 3;
				state[1] = 0;
			}
			break;
		case 1:
			if (this->state[1] == 0) this->strBuffer[0] = str;
			else if (this->state[1] == 1) {
				this->floatBuffer[0] = std::stof(str);
				this->execute();
				this->state[0] = 0;
				break;
			}
			this->state[1]++;
			break;
		case 2:
			cout << str << endl;
			state[0] = 0;
			break;
		case 3:
			if (state[1] == 2) {
				this->plotterApp->linkage->moveTo(vec2(this->floatBuffer[0], this->floatBuffer[1]));
				state[0] = 0;
			}
			else {
				this->floatBuffer[state[1]] = std::stof(str);
				state[1]++;
			}
		default:
			break;
	}
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
	if (con->connected) {
		while (con->serialConnection->available()) {
			con->inChar = con->serialConnection->read(1)[0];
			switch (con->inChar){
			case '\n':
				con->curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
					- con->conStartTime).count();
				con->sendMessages();
				if (con->inStr.size() != 0) {
					con->parse(con->inStr);
					con->inStr = "";
				}
				break;
			case '\t':
				if (con->inStr.size() != 0) {
					con->parse(con->inStr);
					con->inStr = "";
				}
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
	if (this->con->connected) {
		ImGui::TextColored(ImVec4(0, 1, 0, 1), this->con->connectionStatus.c_str());
	}
	else {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: Not connected");
	}
	
	if (ImGui::Button("Check Ports")) {
		this->con->availablePorts = serial::list_ports();
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::BeginCombo("##portCombo", this->con->port.port.c_str())) {
		for (serial::PortInfo& p : this->con->availablePorts) {
			if (ImGui::Selectable(p.port.c_str(), false)) {
				this->con->port = p;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Text("Baud rate:");
	ImGui::PushItemWidth(-1);
	ImGui::InputScalar("##baud rate", ImGuiDataType_U32, &this->con->baudRate);
	ImGui::PushItemWidth(-1);
	if (ImGui::Button("Connect / disconnect")) {
		if (!this->con->connected) {
			try {
				this->con->serialConnection = new serial::Serial(this->con->port.port, this->con->baudRate, serial::Timeout::simpleTimeout(3000));
				if (this->con->serialConnection->isOpen()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					this->con->serialConnection->flushInput();
					this->con->connected = true;
					this->con->connectionStatus = "Status: Connected to " + this->con->port.port;
					this->con->conStartTime = std::chrono::high_resolution_clock::now();
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}
			catch (...) {
				ImGui::OpenPopup("CannotConnectPopup");
				this->con->connected = false;
			}
		}
		else {
			this->con->connected = false;
			this->con->serialConnection->close();
		}
	}
	if (ImGui::BeginPopup("CannotConnectPopup")) {
		std::string tmp = "Cannot establish a serial connection to port " + this->con->port.port;
		ImGui::Text(tmp.c_str());
		ImGui::EndPopup();
	}
	ImGui::End();
}
