#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord0;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord0;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
	vec3 cameraPosition = vec3(inverse(uViewMatrix)[3]);
	vec3 objectPosition = vec3(uModelMatrix[3]);
	vec3 objectToCamera = cameraPosition - objectPosition;
	objectToCamera.y = 0.0f;
	objectToCamera = normalize(objectToCamera);

	const vec3 worldUp = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(worldUp, objectToCamera));
	vec3 up = worldUp;
	vec3 forward = -objectToCamera;

	mat4 rotationMatrixAroundY = mat4(
		vec4(right, 0.0),
		vec4(up, 0.0),
		vec4(forward, 0.0),
		uModelMatrix[3]
	);

	gl_Position = uProjectionMatrix * uViewMatrix * rotationMatrixAroundY *  vec4(aPosition, 1.0);
	vPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));
	vNormal = mat3(uModelMatrix) * aNormal;
	vTexCoord0 = vec2(aTexCoord0.x, aTexCoord0.y);
}