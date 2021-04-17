#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable
#include "Nyx.h"

layout ( location = 0 ) in vec4  vertex     ; 
layout ( location = 1 ) in vec4  normals    ; 
layout ( location = 2 ) in vec4  weights    ;
layout ( location = 3 ) in uvec4 ids        ;
layout ( location = 4 ) in vec2  tex_coords ;

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
  mat4        model    ;
  mat4        view     ;
  mat4        proj     ;
  vec4        position ;
  vec4        normal   ;
  float       weight   ;
  uint        id       ;

  normal   = normals                                   ;
  weight   = weights[ 0 ]                              ;
  id       = ids    [ 0 ]                              ;
  iterator = const_iterator                            ;
  position = vec4( vertex.x, vertex.y, vertex.z, 1.0 ) ;

  nyx_seek( iterator, 0 ) ;

  model = nyx_get( device_ptr, iterator ).model ;
  view  = nyx_get( device_ptr, iterator ).view  ;
  proj  = nyx_get( device_ptr, iterator ).proj  ;
  
  frag_coords = tex_coords.xy                  ;
  gl_Position = proj * view * model * position ;
}