#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform float LAYER;
uniform mat4 MVP;

in vec3 VERTCOLOR[];
out vec3 GEOMCOLOR;

void main()
{
    float size = 1.2 * LAYER;
    float OPL = 1.1;
    float CubeModeDist = 2;
    float HCS = 0.5 * LAYER;
    GEOMCOLOR = VERTCOLOR[0];

    vec4 vPos = MVP * gl_in[0].gl_Position;

    float z = vPos.z;
    if(z > LAYER)
{
    float x = vPos.x / z;
    float y = vPos.y / z;
	
	//if( ((x > -OPL) && (x < OPL) && (y > -OPL) && (y < OPL)) || (z < (32.0 * LAYER)) )
	
	if(true)
	{
			if (z < CubeModeDist)
			//if (false)
			{
				vec4 cube0 = MVP * (gl_in[0].gl_Position + vec4(-HCS, -HCS, -HCS, 0.0));
				vec4 cube1 = MVP * (gl_in[0].gl_Position + vec4(HCS, -HCS, -HCS, 0.0));
				vec4 cube2 = MVP * (gl_in[0].gl_Position + vec4(-HCS, HCS, -HCS, 0.0));
				vec4 cube3 = MVP * (gl_in[0].gl_Position + vec4(HCS, HCS, -HCS, 0.0));
				vec4 cube4 = MVP * (gl_in[0].gl_Position + vec4(-HCS, -HCS, HCS, 0.0));
				vec4 cube5 = MVP * (gl_in[0].gl_Position + vec4(HCS, -HCS, HCS, 0.0));
				vec4 cube6 = MVP * (gl_in[0].gl_Position + vec4(-HCS, HCS, HCS, 0.0));
				vec4 cube7 = MVP * (gl_in[0].gl_Position + vec4(HCS, HCS, HCS, 0.0));
				
				gl_Position = cube0;
				EmitVertex();
				gl_Position = cube1;
				EmitVertex();
				gl_Position = cube2;
				EmitVertex();
				gl_Position = cube3;
				EmitVertex();
				gl_Position = cube6;
				EmitVertex();
				gl_Position = cube7;
				EmitVertex();
				gl_Position = cube4;
				EmitVertex();
				gl_Position = cube5;
				EmitVertex();
				gl_Position = cube0;
				EmitVertex();
				gl_Position = cube1;
				EmitVertex();
				EndPrimitive();
				
				//Left
				gl_Position = cube4;
				EmitVertex();
				gl_Position = cube0;
				EmitVertex();
				gl_Position = cube6;
				EmitVertex();
				gl_Position = cube2;
				EmitVertex();
				EndPrimitive();
				
				//Right
				gl_Position = cube1;
				EmitVertex();
				gl_Position = cube5;
				EmitVertex();
				gl_Position = cube3;
				EmitVertex();
				gl_Position = cube7;
				EmitVertex();
				EndPrimitive();
				
			}
			else
			{
				if( ((x > -OPL) && (x < OPL) && (y > -OPL) && (y < OPL)))
				{
				gl_Position = vPos + vec4(-size, -size, 0.0, 0.0);
				EmitVertex();
				
				gl_Position = vPos + vec4(size, -size, 0.0, 0.0);
				EmitVertex();
				
				gl_Position = vPos + vec4(-size, size, 0.0, 0.0);
				EmitVertex();
				
				gl_Position = vPos + vec4(size, size, 0.0, 0.0);
				EmitVertex();
				EndPrimitive();
				}
			}
	}
}
}