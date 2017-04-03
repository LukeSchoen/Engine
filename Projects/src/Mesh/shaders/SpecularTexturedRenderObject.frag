
#version 330

in vec2 passTexcoord0;
in vec3 passNormal0;
in vec3 passPosition0;

uniform sampler2D texture0;

uniform vec3 camPos;
uniform vec3 camDir;

out vec4 fragColor;

void main()
{
  vec3 normal = normalize(passNormal0);
  
	float specularDistance = 0.3;
	vec3 lightDirection = normalize(vec3(0.5, -1, 0));
	
	// Diffuse
	float diffuseIntensity = max(dot(normal, -lightDirection), dot(normal, lightDirection));
	vec3 diffuse = texture(texture0, passTexcoord0).rgb;
	
	// Specular
	vec3 fragmentToCameraDirection = normalize(-camPos - passPosition0);
	vec3 lightReflection = reflect(lightDirection, normal);
	float specularIntensity = smoothstep(1.0 - specularDistance, 1.0, clamp(dot(fragmentToCameraDirection, reflect(lightDirection, normal)), 0.0, 1.0));
	vec3 specular = vec3(1.0, 0.75, 0.675);
	
	fragColor = vec4(diffuseIntensity * diffuse + specularIntensity * specular * 0.4, 1.0);
}
