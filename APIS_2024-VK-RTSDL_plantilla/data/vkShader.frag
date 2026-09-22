#version 430

//no repetir bindings, el 0 es MVP
layout(binding = 1) uniform  sampler2D textureColor;

layout(location = 0) in vec4 fcolor;
layout(location = 1) in vec2 ftextCoord;

layout(location = 0) out vec4 FragColor;

void main() {

    //FragColor =fcolor;
    //texturas:
    FragColor =texture(textureColor,ftextCoord);
}
