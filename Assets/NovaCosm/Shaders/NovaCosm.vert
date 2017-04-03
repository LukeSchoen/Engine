#version 330 core

in vec3 position;
in vec3 color;
uniform float LAYER;

uniform float SWAY;
uniform float swayAmount;
uniform vec3 regionPos;
out vec3 VERTCOLOR;

void main()
{
	VERTCOLOR = color;
	float y = (position + regionPos).y * LAYER;
	float ys = cos(y*0.2) * 1.5;
	vec3 sway = vec3(sin(SWAY + y) * ys, 0, cos(SWAY + y) * ys);
	gl_Position = vec4((position + regionPos) * LAYER + sway*swayAmount, 1);
//	gl_Position = vec4((position + regionPos) * LAYER, 1);
}