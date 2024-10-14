#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <thread>
#include <mutex>
#include <map>
#include<set>
#include <fstream>
#include <string>
#include "boost/signals2/signal.hpp"
#include <boost/bind/bind.hpp>
#include<imgui_impl_opengl3.h>
#include<imgui_impl_glfw.h>
#include <stdio.h>
#include<imgui_stdlib.h>
#include<implot.h>
#include<queue>

using std::cout;
using std::endl;
using std::thread;
using glm::vec3;
using glm::vec2;
using glm::vec4;
using glm::mat4;
using boost::signals2::signal;
using boost::signals2::slot;

const float pi = atan(1) * 4;

enum inputEvent {
	mouseButtonEvent,
	scrollEvent,
	keyPressEvent,
	cursorMoveEvent,
	windowResize
};
float getLength(vec2 vector);
float distFromLine(vec2 p, vec2 l1, vec2 l2);