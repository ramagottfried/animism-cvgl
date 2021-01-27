#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

uniform mat4 transform_matrix;
uniform float time;

out vec2 Texcoord;
out float Time;

void main()
{
    Time = time;
    Texcoord = texcoord;
    gl_Position = transform_matrix * vec4(position, 1.0);
}
