#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf            : enable
#include "Nyx.h"

layout ( location = 0 ) in vec4 vertex     ; 
layout ( location = 1 ) in vec2 tex_coords ;

layout( location = 0 ) out vec2 frag_coords ;

// The structure of data coming in.
NyxBuffer SceneData 
{
  mat4 model ;
  mat4 view  ;
  mat4 proj  ;
};

NyxPushConstant push
{
  SceneData   device_ptr     ;
  NyxIterator const_iterator ;
};

void main()
{
  NyxIterator iterator ;
  vec4        position ;

  iterator = const_iterator                                 ;
  position = vec4( vertex.x, vertex.y, vertex.z, vertex.w ) ;

  nyx_seek( iterator, 0 ) ;

  mat4 model = nyx_get( device_ptr, iterator ).model ;
  mat4 view  = nyx_get( device_ptr, iterator ).view  ;
  mat4 proj  = nyx_get( device_ptr, iterator ).proj  ;
  
  //debugPrintfEXT( "Id: %d, Vertex: %f, %f, %f, %f | Coords %f, %f\\n ", gl_VertexIndex, vertex.x, vertex.y, vertex.z, vertex.w, tex_coords.x, tex_coords.y ) ;

  frag_coords = tex_coords.xy                  ;
  gl_Position = proj * view * model * position ;
}