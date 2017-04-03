#version 330 core

in vec3 GEOMCOLOR;

out vec3 color;

vec3 saturation(vec3 rgb, float adjustment)
{
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}


void main()
{
	color = saturation(GEOMCOLOR, 1.5);
}