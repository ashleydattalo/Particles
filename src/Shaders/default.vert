#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform float offset;
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;
out vec3 pos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    ourColor = position;
    pos = position;
}