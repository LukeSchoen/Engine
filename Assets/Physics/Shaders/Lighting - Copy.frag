#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform sampler2D depth0;
uniform sampler2D depth1;

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
	vec3 mapColor = (texture(texture0, passTexcoord0) * texture(texture1, passTexcoord0)).rgb;
	vec3 actorColor = texture(texture2, passTexcoord0).rgb;
	
	float mapDepth = texture(depth0, passTexcoord0).x;
	float actorDepth = texture(depth1, passTexcoord0).x;
	
	if(actorDepth < mapDepth)
		fragColor = vec4( saturation(actorColor, 1.5), 1.0 );
	else
		fragColor = vec4( saturation(mapColor, 1.5) * 2.5, 1.0 );
}
