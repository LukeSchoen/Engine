#version 330 core

in vec3 position;
in vec3 color;
uniform float LAYER;
uniform mat4 MVP;
uniform vec3 regionPos;
out vec3 VERTCOLOR;

void main()
{
	VERTCOLOR = color;
	gl_Position = MVP * vec4((position + regionPos) * LAYER, 1);
	
	//if(gl_Position.z  < 1 || abs(gl_Position.x) > gl_Position.z || abs(gl_Position.y) > gl_Position.z)
	//{
	//	gl_Position = vec4(0, 0, 0, -1);
	//	gl_PointSize = 1;
	//}
	//else
	{
		gl_PointSize = (0.5 + max(1300.0 / gl_Position.z * LAYER, 0.5));
	}
}