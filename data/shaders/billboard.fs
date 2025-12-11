#version 330 core
#define MAX_LIGHTS 16

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord0;
out vec4 fragColor;


uniform vec3 uDiffuseColor;
uniform sampler2D uDiffuseTexture;

void main()
{
	vec4 baseColor = texture(uDiffuseTexture, vTexCoord0);
	fragColor = baseColor;
}