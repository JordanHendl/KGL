#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec4 vertex ; 

void main()
{
  gl_Position = vertex ;
}