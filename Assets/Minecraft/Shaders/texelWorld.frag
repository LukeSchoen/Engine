#version 330

uniform sampler2DArray texture0;

uniform float fade;

in vec3 passTexcoord0;
in vec3 passColour0;
in float passDepth;

out vec4 fragColor;

vec3 fogColor = vec3(136/ 255.0, 222/ 255.0, 255 / 255.0);
float fogStart = 48;
float fogEnd = 96;

void main()
{
	vec4 texSample = texture(texture0, passTexcoord0);
	if(texSample.a != 1) discard; // bad!
	fragColor = vec4(texSample.rgb * passColour0.rgb, texSample.a * fade);
	float fogAmt = 1.0 - clamp((passDepth - fogStart) / (fogEnd - fogStart),0, 1);
	fragColor = fragColor * fogAmt + vec4(fogColor, fragColor.w) * (1 - fogAmt);
}
