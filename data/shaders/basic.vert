#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_normal;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
	gl_Position = u_projection * u_view * vec4(a_position, 1.0);
	v_texcoord = a_texcoord;
	v_normal = a_normal;
}