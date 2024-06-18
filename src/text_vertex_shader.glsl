#version 330 core

layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec2 vertexUV;
out vec2 uv;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertexPosition_modelspace, 1.0);
    TexCoords = vertex.zw;
}
