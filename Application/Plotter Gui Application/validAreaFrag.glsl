#version 430 core
layout (location = 0) out vec4 outColor;
uniform vec4 origData;
uniform vec2 lengthData;
in vec4 pos;

float pi = atan(1) * 4;

vec2 calcAngles(vec2 pos, vec2 orig, vec2 lengths, bool reversed){
	vec2 dist = pos - orig;

	float z = length(dist);
	float a1 = atan(dist[1] / dist[0]);
	float a2 = acos((z*z + lengths[0] * lengths[0] - lengths[1] * lengths[1]) / (2 * z * lengths[0]));
	if (a1 < 0) a1 += pi;
	if (reversed) a2 *= -1;

	float a3 = acos((-z*z + lengths[0] * lengths[0] + lengths[1] * lengths[1]) / (2 * lengths[1] * lengths[0]));

	return vec2(degrees(a1 + a2), degrees(a3));
}

void main(){
	if((length(pos.xy-origData.xy) < lengthData[0] + lengthData[1]) && (length(pos.xy-origData.zw) < lengthData[0] + lengthData[1])){
		vec2 a1 = calcAngles(pos.xy, origData.xy, lengthData, false);
		vec2 a2 = calcAngles(pos.xy, origData.zw, lengthData, true);

		if(a1.x < 180 && a2.x > 16.5 && (540 - ((180-a2.x) + a2.y + a1.x + a1.y)) < 160) {
		outColor = vec4(1, 1, 1, 0.5);
		}else{
		discard;
		}
	}
}