#include "SVGController.h"
#include "SceneController.h"



using namespace rapidxml;

SVGControllerNS::SVGController::SVGController(Engine* engine, PlotterApp* plotterApp){
	this->state.parser = this;
	this->curState = &this->state;
	this->engine = engine;
	this->plotterApp = plotterApp;
	this->initCallbacks();
}

void SVGControllerNS::SVGController::readFile(std::string path){
	this->source = "";
	std::ifstream file(path);
	if (!file.is_open()) {
		cout << "failed to open " << path << " ." << endl;
	}
	std::string tmp;
	while (std::getline(file, tmp)) {
		this->source += tmp + '\n';
	}
	file.close();
}

std::vector<float> SVGControllerNS::SVGController::extractFloats(std::string str){
	std::string tmp;
	std::vector<float> out;

	for (char c : str) {
		if (!(c == ',' || c == ' ')) {
			tmp += c;
		}
		else if (tmp.size() != 0){
			out.push_back(std::stof(tmp));
			cout << out[out.size() - 1];
			tmp = "";
		}
	}
	return out;
}

void SVGControllerNS::SVGController::parse(std::string path){
	this->readFile(path);
	char* sourceData = &this->source[0];
	//cout << sourceData << endl;

	this->xmlDocument.parse<0>(sourceData);
	this->mainNode = this->xmlDocument.first_node("svg")->first_node("g");
	if (this->mainNode == NULL) {
		cout << "No SVG root in the file.";
		return;
	}
	
	xml_node<>* tmp = this->mainNode->first_node();
	if (tmp == NULL) {
		cout << "No entities in the svg file";
		return;
	}
	
	for (xml_node<>* node = this->mainNode->first_node(); node; node = node->next_sibling()) {
		if (node->name() == std::string("line")) {
			std::vector<vec3> vertices;
			vertices.push_back({ 0, 0 , 0});
			vertices.push_back({ 0, 0 , 0 });
			for (xml_attribute<>* attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				if (attr->name()		== std::string("x1")) vertices[0][0] = std::stof(attr->value()) / 10;
				else if (attr->name()	== std::string("x2")) vertices[1][0] = std::stof(attr->value()) / 10;
				else if (attr->name()	== std::string("y1")) vertices[0][1] = std::stof(attr->value()) / -10;
				else if (attr->name()	== std::string("y2")) vertices[1][1] = std::stof(attr->value()) / -10;
			}
			PolyLine* line = new PolyLine(this->engine, this->plotterApp, &vertices);
			this->plotterApp->sceneController->svgElementList.push_back(line);
		}else if (node->name() == std::string("polyline")) {
			std::vector<vec3> vertices;
			std::vector<float> data;
			xml_attribute<>* attr = node->first_attribute("points");

			data = this->extractFloats(attr->value());

			for (int i = 0; i < data.size(); i += 2) {
				vertices.push_back({ data[i] / 10, -data[i + 1] / 10, 0 });
			}

			PolyLine* line = new PolyLine(this->engine, this->plotterApp, &vertices);
			this->plotterApp->sceneController->svgElementList.push_back(line);
		}
		else if (node->name() == std::string("polygon")) {
			std::vector<vec3> vertices;
			std::vector<float> data;
			xml_attribute<>* attr = node->first_attribute("points");

			data = this->extractFloats(attr->value());

			for (int i = 0; i < data.size(); i += 2) {
				vertices.push_back({ data[i] / 10, -data[i + 1] / 10, 0 });
			}

			vertices.push_back(vertices[0]);

			PolyLine* line = new PolyLine(this->engine, this->plotterApp, &vertices);
			this->plotterApp->sceneController->svgElementList.push_back(line);
		}
	}

}

void SVGControllerNS::ParserState::keyboardCB(InputState* input){
	if (input->keyboardPress.key == GLFW_KEY_F && input->keyboardPress.action == GLFW_PRESS) {
		cout << "Insert the SVG file location:" << endl;
		//std::string path;
		//std::getline(std::cin, path);
		this->parser->parse("C:\\Users\\Parsa\\Desktop\\arm2.svg");
	}
}
