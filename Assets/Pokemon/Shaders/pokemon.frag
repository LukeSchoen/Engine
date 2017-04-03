#version 330 core

uniform sampler2D texture0;
in vec2 passTexcoord0;

out vec4 fragColor;

vec3 saturation(vec3 rgb, float adjustment)
{
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}

void main()
{
	fragColor = vec4(saturation(texture(texture0, passTexcoord0).rgb, 1.5), 1);
}