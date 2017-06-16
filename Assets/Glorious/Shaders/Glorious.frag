#version 330

uniform sampler2DArray texture0;
in vec3 passTexcoord0;
in vec3 passColour0;
out vec4 fragColor;

vec3 saturation(vec3 rgb, float adjustment)
{
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}

void main()
{
	vec4 c = texture(texture0, passTexcoord0);
	fragColor = c;//vec4(saturation(c.rgb * passColour0, 1.5), c.w);
}