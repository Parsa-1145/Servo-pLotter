#include "Engine.h"
void keyCBWrapper(GLFWwindow* window, int key, int scancode, int action, int mod) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiKey(key), action);
	if (!io.WantCaptureKeyboard) {
		Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
		if (action != 2) {
			engine->inputState.prevKeys = engine->inputState.curKeys;

			if (action == 1) engine->inputState.curKeys.insert(key);
			else if (action == 0) engine->inputState.curKeys.erase(key);

			engine->inputState.keyboardPress.key = key;
			engine->inputState.keyboardPress.action = action;
			engine->inputState.curEvent = keyPressEvent;

			engine->addEventToUpdater();

		}
	}
}
void curserPosCBWrapper(GLFWwindow* window, double x, double y) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y);
	if (!io.WantCaptureMouse) {
		Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
		engine->inputState.prevMousePos = engine->inputState.curMousePos;

		engine->inputState.curMousePos[0] = (int)x;
		engine->inputState.curMousePos[1] = (int)engine->renderer.wHeight - y;

		engine->inputState.curEvent = cursorMoveEvent;

		engine->addEventToUpdater();
	}
}
void mouseButtonCBWrapper(GLFWwindow* window, int button, int action, int mods) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, action);
	if (!io.WantCaptureMouse) {
		Engine* engine = (Engine*)glfwGetWindowUserPointer(window);

		engine->inputState.prevMouseBtns = engine->inputState.curMouseBtns;

		if (action == 1) engine->inputState.curMouseBtns.insert(button);
		else if (action == 0) engine->inputState.curMouseBtns.erase(button);

		engine->inputState.mouseBtnPress.btn = button;
		engine->inputState.mouseBtnPress.action = action;

		engine->inputState.curEvent = mouseButtonEvent;


		engine->addEventToUpdater();
	}
}
void scrollCBWrapper(GLFWwindow* window, double xoffset, double yoffset) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(xoffset, yoffset);
	if (!io.WantCaptureMouse) {
		Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
		engine->inputState.scroll = yoffset;

		engine->inputState.curEvent = scrollEvent;

		engine->addEventToUpdater();
	}
}
void frameBufferRisizeCBWrapper(GLFWwindow* window, int width, int height) {
	Engine* engine = (Engine*)glfwGetWindowUserPointer(window);

	engine->renderer.wHeight = height;
	engine->renderer.wWidth = width;

	engine->locker.lock();
	engine->renderer.shouldResizeViewport = true;
	engine->locker.unlock();

	engine->inputState.curEvent = windowResize;

	engine->addEventToUpdater();
}

void Engine::init(int width, int height, const char* title) {

	if (!glfwInit()) {
		cout << "Main: GLFW failed to initialize." << endl;
		exit(1);
	}
	///// Main Window /////
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	this->window = glfwCreateWindow(width, height, title, NULL, this->loaderContext);
	if (this->window == NULL) {
		cout << "Main: Failed to create the window." << endl;
		glfwTerminate();
		exit(1);
	}
	glfwSetWindowUserPointer(this->window, this);
	glfwMakeContextCurrent(this->window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to intialize glad.";
		exit(1);
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	glfwMakeContextCurrent(NULL);
	
	this->initCallbacks();

	///// Loader Context /////
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, false);
	this->loaderContext = glfwCreateWindow(100, 100, "", NULL, this->window);
	if (this->loaderContext == NULL) {
		cout << "Main: Failed to create the loader context." << endl;
		glfwTerminate();
		exit(1);
	}

	///// Initializing threads /////
	this->renderer.locker = &this->locker;
	this->renderer.shouldTerminate = &this->shouldTerminate;
	this->renderer.shouldStart = &this->shouldStart;
	this->renderer.wHeight = height;
	this->renderer.wWidth = width;
	this->renderThread = thread(&Renderer::init, &this->renderer, this->window);
	while (!this->renderer.initDone) {};

	this->updater.locker = &this->locker;
	this->updater.shouldTerminate = &this->shouldTerminate;
	this->updater.shouldStart = &this->shouldStart;
	this->updateThread = thread(&Updater::init, &this->updater, this->loaderContext);

	glfwMakeContextCurrent(this->window);
}

void Engine::addEventToUpdater(){
	InputState* tmp = new InputState();
	*tmp = this->inputState;
	this->locker.lock();
	this->updater.eventList.push_back(tmp);
	this->locker.unlock();
}

void Engine::initCallbacks() {
	glfwSetKeyCallback(this->window, keyCBWrapper);
	glfwSetCursorPosCallback(this->window, curserPosCBWrapper);
	glfwSetMouseButtonCallback(this->window, mouseButtonCBWrapper);
	glfwSetScrollCallback(this->window, scrollCBWrapper);
	glfwSetFramebufferSizeCallback(this->window, frameBufferRisizeCBWrapper);
}

void Engine::eventLoop(){
	glfwMakeContextCurrent(NULL);
	this->shouldStart = true;
	while(!glfwWindowShouldClose(this->window)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		glfwPollEvents();
	}

	this->shouldTerminate = true;
	this->updateThread.join();
	this->renderThread.join();
	
	glfwTerminate();
}
