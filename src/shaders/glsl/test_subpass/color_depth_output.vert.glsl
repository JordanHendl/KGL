#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) in  vec4 vertex ;
layout( location = 0 ) out vec2 coords ;

void main()
{
  coords      = vertex.zw                   ;
  gl_Position = vec4( vertex.xy, 0.0, 1.0 ) ;
}