#version 430 core
layout (location = 0) in vec3 aPos;
uniform mat4x4 VP;

out vec4 pos;

vec4 ps[4] = {{-1, 1, 1, 1}, {1, 1, 1, 1}, {1, -1, 1, 1}, {-1, -1, 1, 1}};

void main(){
	pos = vec4(aPos, 1.0);
	gl_Position = VP * vec4(aPos,1);
}