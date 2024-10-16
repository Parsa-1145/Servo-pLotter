#pragma once
struct moveData {
	bool finished;
	vec3 position;
	moveData(bool finished, vec3 position) {
		this->position = position;
		this->finished = finished;
	}
};

class SVGElementBase {
public:
	virtual void initExecution() {  };
	virtual moveData moveOnShape(float moveDistance) { return moveData(false, vec3(0, 0, 0)); };
	virtual void translate(vec2 transVec) = 0;
	virtual void select() = 0;
	virtual void deselect() = 0;
	virtual vec3 getHoveringVertexPosition() = 0;
	virtual bool isInBox(vec2 xRange, vec2 yRange) = 0;
	virtual std::vector<std::string> getMachineCode() = 0;
	virtual void remove() = 0;

	bool isHovered = false;
	int hoveredVertexIndex = -1;
	vec3 worldPosition;
};