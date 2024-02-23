#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aLayer;

out vec2 TexCoords;
out vec2 uv;
out float layer;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	layer = aLayer;
	uv = vec2(dot(aNormal.zxy, aPos), dot(aNormal.yzx, aPos));
	TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}