#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) out vec4 outColor ;
layout( location = 0 ) in  vec2 coords   ;

layout( binding = 0 ) uniform ColorInfo
{
  bool blue_or_green ;
} info ;

void main()
{
  if( info.blue_or_green )
  {
    outColor = vec4( 0.0, 1.0, 0.0, 1.0 ) ;
  }
  else
  {
    outColor = vec4( 0.0, 0.0, 1.0, 1.0 ) ;
  }

  outColor = vec4( 1.0, 0.0, 0.0, 1.0 ) ;
}