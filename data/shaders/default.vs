#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord0;
layout(location = 4) in vec2 aTexCoord1;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord0;
out vec2 vTexCoord1;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
	vPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));
	vNormal = mat3(uModelMatrix) * aNormal;
	vTexCoord0 = vec2(aTexCoord0.x, aTexCoord0.y);
	vTexCoord1 = vec2(aTexCoord1.x, 1.0 - aTexCoord1.y);
}