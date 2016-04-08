// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 0) in vec3 a_normal;

out vec3 v_normal;
out vec3 v_color;

void main()
{
    gl_Position = vec4(a_position,1);
}
