#include "SceneController.h"
#include "Camera.h"
#include "Grid.h"
#include "Linkage.h"

SceneControllerNS::SceneController::SceneController(Engine* engine, PlotterApp* plotterApp) {
	this->plotterApp = plotterApp;
	this->engine = engine;

	this->idle = new Idle();
	this->idle->sceneController = this;
	this->moving = new Moving();
	this->moving->sceneController = this;
	this->curState = this->idle;

	this->initCallbacks();
}
bool SceneControllerNS::SceneController::isSelected(SVGElementBase* element) {
	return (this->selectedEntities.find(element) != this->selectedEntities.end());
}
void SceneControllerNS::Idle::mouseButtonCB(InputState* state) {
	std::set<SVGElementBase*> copyList = this->sceneController->selectedEntities;

	if (state->mouseBtnPressed(GLFW_MOUSE_BUTTON_2)) {
		this->hoveringSelected = false;
		for (SVGElementBase* e : this->sceneController->selectedEntities) {
			if (e->isHovered) {
				this->hoveringSelected = true;
				break;
			}
		}
		for (SVGElementBase* e : this->sceneController->svgElementList) {
			if (!e->isHovered) {
				if (!this->hoveringSelected && !state->keyIsHeld(GLFW_KEY_LEFT_SHIFT)) {
					e->deselect();
				}
			}
		}
		for (SVGElementBase* e : this->sceneController->svgElementList) {
			if (e->isHovered) {
				e->select();
				this->hoveringSelected = true;
			}
		}
	}
	else if (state->mouseBtnReleased(GLFW_MOUSE_BUTTON_2)) {
		if (this->hoveringSelected && !state->keyIsHeld(GLFW_KEY_LEFT_SHIFT)) {
			for (SVGElementBase* e : copyList) {
				if (!e->isHovered) {
					e->deselect();
				}
			}
		}
		this->hoveringSelected = false;
	}
}
void SceneControllerNS::Idle::cursorPosCB(InputState* state) {
	if (this->hoveringSelected) {
		this->hoveringSelected = false;

		bool hoveringVertex = false;
		for (SVGElementBase* e : this->sceneController->selectedEntities) {
			if (e->hoveredVertexIndex != -1) {
				this->sceneController->moving->startPos = e->getHoveringVertexPosition();
				hoveringVertex = true;
			}
		}
		if (!hoveringVertex) {
			this->sceneController->moving->startPos = this->sceneController->plotterApp->camera->getScreenData() + state->curMousePos * this->sceneController->plotterApp->camera->zoomScale;
		}
		this->sceneController->moving->prevWorldPos = this->sceneController->moving->startPos;
		this->sceneController->curState = this->sceneController->moving;
	}
}
void SceneControllerNS::Moving::mouseButtonCB(InputState* state) {
	if (state->mouseBtnReleased(GLFW_MOUSE_BUTTON_2)) {
		this->sceneController->curState = this->sceneController->idle;
	}
}
void SceneControllerNS::Moving::cursorPosCB(InputState* state) {
	vec2 translationMask = { 1, 1 };
	vec2 worldPos = this->sceneController->plotterApp->camera->getScreenData() + state->curMousePos * this->sceneController->plotterApp->camera->zoomScale;

	if (state->keyIsHeld(GLFW_KEY_LEFT_SHIFT)) {
		vec2 tmp = abs(worldPos - this->startPos);

		if (tmp[0] > tmp[1])	translationMask[1] = 0;
		else					translationMask[0] = 0;
		worldPos = worldPos * translationMask + this->startPos * (vec2(1) - translationMask);
	}

	if (state->keyIsHeld(GLFW_KEY_LEFT_CONTROL)) {
		vec2 devidedPos = worldPos / this->sceneController->plotterApp->grid->baseWidth;
		devidedPos = round(devidedPos);
		worldPos = devidedPos * this->sceneController->plotterApp->grid->baseWidth;
	}
	for (SVGElementBase* e : this->sceneController->selectedEntities) {
		e->translate((worldPos - this->prevWorldPos));
	}
	this->prevWorldPos = worldPos;
}

void SceneControllerNS::State::render(){
	ImGui::Begin("Objects");
		for (int i = 0; i < this->sceneController->svgElementList.size(); i++) {
			ImGui::PushID(i);
			sprintf_s(buffer, "%d", i);
			ImGui::SetNextItemAllowOverlap();
			if (ImGui::Selectable(buffer, this->sceneController->isSelected(this->sceneController->svgElementList[i]))) {
				if (this->sceneController->isSelected(this->sceneController->svgElementList[i])) {
					this->sceneController->svgElementList[i]->deselect();
				}
				else {
					this->sceneController->svgElementList[i]->select();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				if (this->sceneController->isSelected(this->sceneController->svgElementList[i])) {
					this->sceneController->svgElementList[i]->deselect();
				}
				this->sceneController->svgElementList[i]->remove();
				this->sceneController->svgElementList.erase(this->sceneController->svgElementList.begin() + i);
			}
			ImGui::SameLine();
			if (ImGui::Button("Draw")) {
				this->sceneController->plotterApp->linkage->prepareMchineCode(this->sceneController->svgElementList[i]);
				this->sceneController->plotterApp->linkage->curState = this->sceneController->plotterApp->linkage->excuting;
			}
			ImGui::SameLine();
			ImGui::Button("up");
			ImGui::SameLine();
			ImGui::Button("down");
			ImGui::PopID();
		}
	ImGui::End();
}
