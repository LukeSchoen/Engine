#version 330

uniform sampler2D texture0;

in vec2 passTexcoord0;

in float d;

out vec4 fragColor;

void main()
{
    vec4 fogColor = vec4(192 / 255.0, 140 / 255.0, 105 / 255.0, 1.0) + vec4(0.2,0.2,0.2,0);
    float dist = (d / 1000.0) - 0.05;
	dist = max(dist,0);
	dist = min(dist,1.0);
	vec4 col = vec4(texture(texture0, passTexcoord0).rgb, 1);
	col = (col * (1-dist));
	col = col + (fogColor * dist);
	fragColor = col;
}
