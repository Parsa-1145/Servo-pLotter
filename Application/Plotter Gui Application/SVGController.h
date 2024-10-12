#pragma once
#include "Common.h"
#include "BaseClasses.h"
#include "PolyLine.h"
#include <Rapidxml/rapidxml.hpp>

namespace SVGControllerNS {
	class SVGController;
	class ParserState : public StateBase {
	public:
		SVGController* parser;

		void keyboardCB(InputState* input) override;
	};
	class SVGController: public ObjectBase<ParserState> {
	public:
		PlotterApp* plotterApp;
		std::string source;
		rapidxml::xml_document<> xmlDocument;
		rapidxml::xml_node<>* mainNode;

		ParserState state;

		SVGController(Engine* engine, PlotterApp* plotterApp);

		
		void parse(std::string path);
	private:
		void readFile(std::string path);
		std::vector<float> extractFloats(std::string str);
	};
}