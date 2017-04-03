#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 MVP;

out vec2 uvs;

void main()
{
    vec4 vPos = MVP * gl_in[0].gl_Position;
	
	float s = 100;
	
    gl_Position = vPos + vec4(-s, -s, 0.0, 0.0);
	uvs = vec2(0, 0);
    EmitVertex();
				
    gl_Position = vPos + vec4(s, -s, 0.0, 0.0);
	uvs = vec2(0, 1);
    EmitVertex();
				
    gl_Position = vPos + vec4(-s, s, 0.0, 0.0);
	uvs = vec2(1, 0);
    EmitVertex();
				
    gl_Position = vPos + vec4(s, s, 0.0, 0.0);
	uvs = vec2(1, 1);
    EmitVertex();
}
