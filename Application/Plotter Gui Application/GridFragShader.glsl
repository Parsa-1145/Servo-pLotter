#version 430 core
layout (location = 0) out vec4 outColor;
uniform vec2 scrSize;
uniform float baseWidth;
uniform vec4 screenPos;

vec2 pos;
vec2 scrPos;
vec2 devided;
vec2 devidedFract;

vec3 xAxisColor = vec3(151.0, 60.0, 73.0) / 255;
vec3 yAxisColor = vec3(84.0, 109.0, 52.0)/ 255;
vec3 mainLineColor = vec3(85.0, 85.0, 85.0)/ 255;
vec3 secondaryLineColor = vec3(74.0, 74.0, 74.0)/ 255;

float opac;

bool horizontal;
bool vertical;
bool primary;

bool devides(int a, int b){
	return (a%b) == 0;
}

void main(){
	

	pos = screenPos.xy + (gl_FragCoord.xy * screenPos.z);
	devided = pos / baseWidth;
	devidedFract = vec2(fract(devided.x), fract(devided.y));
	devided = abs(round(devided));

	horizontal = devidedFract.y <= screenPos.w;
	vertical = devidedFract.x <= screenPos.w;
	

	if(horizontal || vertical){
		scrPos = gl_FragCoord.xy - scrSize/2;
		opac = (length(scrSize/2) - length(scrPos)) / length(scrSize / 2);
		primary = (devides(int(devided.x), 10)&& vertical) || (devides(int(devided.y), 10) && horizontal);

		if(horizontal && devided.y == 0) outColor = vec4(xAxisColor, opac);
		else if(vertical && devided.x == 0) outColor = vec4(yAxisColor, opac);
		else if(primary) outColor = vec4(mainLineColor, opac / 2);
		else outColor = vec4(secondaryLineColor, opac / 4);
	}else{
		outColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}