#version 330 core
in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D uTexture;

void main()
{
    vec3 color = texture(uTexture, vTexCoord).rgb;
    fragColor = vec4(color, 1.0);
}