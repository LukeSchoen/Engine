#version 330

uniform sampler2D texture0;
in vec2 passTexcoord0;

out vec4 fragColor;

void main()
{
	vec3 lowRes = texture(texture0, passTexcoord0).rgb;
	vec3 midRes = texture(texture0, passTexcoord0 * 1).rgb;
	vec3 higRes = texture(texture0, passTexcoord0 * 4).rgb;
	
	float depth = pow(gl_FragCoord.z, 256);
	depth = clamp(depth, 0.25, 1);
	
	fragColor = vec4(lowRes * depth + midRes * (1-depth), 1);
}
