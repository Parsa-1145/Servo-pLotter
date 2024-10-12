#include "Renderer.h"

Renderer::Renderer(){
}

void Renderer::init(GLFWwindow* window){
	this->window = window;

	glfwMakeContextCurrent(this->window);

	glViewport(0, 0, this->wWidth, this->wHeight);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);

	this->initVaos();
	this->initShaders();

	glfwMakeContextCurrent(NULL);
	this->initDone = true;

	while(!(*this->shouldStart)){}
	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);
	this->renderLoop();
}

void Renderer::renderLoop(){
	while (!*this->shouldTerminate) {
		if (this->shouldResizeViewport) {
			glViewport(0, 0, this->wWidth, this->wHeight);
			if (this->locker->try_lock()) {
				this->shouldResizeViewport = false;
				this->locker->unlock();
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		this->renderSignal();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(this->window);
	}
}
