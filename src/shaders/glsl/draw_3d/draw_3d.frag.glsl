#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf            : enable
#include "Nyx.h"

layout( location = 0 ) in  vec2 frag_coords ;
layout( location = 0 ) out vec4 out_color   ;

layout( binding = 0 ) uniform sampler2D mesh_texture ; 

void main()
{  
  vec4 color ;
  
  color = texture( mesh_texture, frag_coords ) ;
  if( color.a < 0.1 ) discard ;
  out_color = color ;
}