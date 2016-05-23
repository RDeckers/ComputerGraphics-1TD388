// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec2 a_vertices;
out vec2 v_texCoord;

void main()
{
  gl_Position = vec4(a_vertices,0,1);
  v_texCoord = (1+a_vertices)*0.5;
}
