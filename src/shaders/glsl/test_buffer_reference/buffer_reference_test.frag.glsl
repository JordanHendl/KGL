#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) in  vec2 coords    ;
layout( location = 0 ) out vec4 out_color ;

layout( binding = 0 ) uniform ColorInfo
{
  bool blue_or_green ;
} info ;

void main()
{
  
  if( info.blue_or_green )
  {
    out_color = vec4( 0.0, 1.0, 0.0, 1.0 ) ;
  }
  else
  {
    out_color = vec4( 0.0, 0.0, 1.0, 1.0 ) ;
  }

  out_color = vec4( coords.x, coords.y, 0.0, 1.0 ) ;
}