#version 330 core
out vec4 FragColor;
in float layer;

in vec2 TexCoords;

uniform sampler2DArray texarray;

//uniform float layer;

void main() {
   FragColor = vec4(1.0, 0.0, 0.0, 1.0);
   //vec2 texCoord = 1 + 0.05125 * fract(tileUV);
   //vec4 texCoord = fourTap(vec2(0,0), uv, 0.125, texture_diffuse1);
   //float actual_layer = max(0, min(float(256 - 1), floor(float(1) + 0.5)));
   //FragColor = texture(textureArray, vec3(TexCoords, actual_layer));
   //FragColor = texture(textures[textureNumber], TexCoords);
   FragColor = texture(texarray, vec3(TexCoords, layer));
}