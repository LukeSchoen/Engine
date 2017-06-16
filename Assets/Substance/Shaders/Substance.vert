#version 330 core

in vec3 position;
in vec3 color;

uniform mat4 MVP;

uniform float size;

out vec3 VERTCOLOR;

void main()
{
	VERTCOLOR = color;
	gl_Position = MVP * vec4(position, 1);
	gl_PointSize = max(1920.0f / gl_Position.w*0.5*size, 2);
}