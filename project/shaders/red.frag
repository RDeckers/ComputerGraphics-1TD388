// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) out vec4 frag_color;

void main()
{
  frag_color = vec4(0.6,0,0, 1);
}
