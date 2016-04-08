// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

in vec3 v_color;

layout(location = 0) out vec4 frag_color;

void main()
{
  frag_color = vec4(v_color, 1);
}
