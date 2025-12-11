#version 330 core
in vec2 v_texcoord;
in vec3 v_normal;
out vec4 frag_color;

uniform sampler2D u_texture;

void main()
{
	float gray = abs(dot(vec3(0.7, 0.8, 0.9), v_normal));
	frag_color = vec4(vec3(gray), 1.0) * texture(u_texture, v_texcoord);
}