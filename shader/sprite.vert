#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 point1;
uniform vec2 point2;
uniform mat4 u_projection;
uniform float thickness;

void main() {

    vec2 d = point2 - point1;
    vec2 normal = vec2(-d.y, d.x) / length(d);
    vec2 offset = normal * thickness * (aPos.x -0.5);


    vec2 p = point1 + d*aPos.y + offset;
 
    gl_Position = u_projection*vec4(p, 0.0, 1.0);
    TexCoord = aTexCoord;
}   