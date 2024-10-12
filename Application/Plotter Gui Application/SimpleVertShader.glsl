#version 430 core
layout (location = 0) in vec3 aPos;
uniform mat4x4 VP;
uniform mat4x4 M;
void main()
{
	gl_Position = VP * M * vec4(aPos, 1.0);
}