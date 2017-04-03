#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float LAYER;
uniform mat4 MVP;

in vec3 VERTCOLOR[];
out vec3 GEOMCOLOR;

float voxelSize = LAYER * 1.2;

void main()
{
    GEOMCOLOR = VERTCOLOR[0];

    vec4 vPos = MVP * gl_in[0].gl_Position;
	
    gl_Position = vPos + vec4(-voxelSize, -voxelSize, 0.0, 0.0);
    EmitVertex();
				
    gl_Position = vPos + vec4(voxelSize, -voxelSize, 0.0, 0.0);
    EmitVertex();
				
    gl_Position = vPos + vec4(-voxelSize, voxelSize, 0.0, 0.0);
    EmitVertex();
				
    gl_Position = vPos + vec4(voxelSize, voxelSize, 0.0, 0.0);
    EmitVertex();
}
