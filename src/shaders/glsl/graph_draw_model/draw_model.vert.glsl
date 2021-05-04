#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf            : enable
#include "Nyx.h"

// Standard Ngg model vertex layout.
layout ( location = 0 ) in vec4  vertex     ; 
layout ( location = 1 ) in vec4  normals    ; 
layout ( location = 2 ) in vec4  weights    ;
layout ( location = 3 ) in uvec4 ids        ;
layout ( location = 4 ) in vec2  tex_coords ;

     layout( location = 0 ) out vec2  frag_coords    ;
flat layout( location = 1 ) out uvec2 texture_index  ;

NyxBuffer PositionData 
{
  mat4 model ;
};

NyxBuffer ProjectionData
{
  mat4 viewproj ;
};

NyxPushConstant push
{
  PositionData   transform_device_ptr     ;
  NyxIterator    const_transform_iterator ;
  ProjectionData proj_device_ptr          ;
  NyxIterator    const_proj_iterator      ;
  uint           index                    ;
  uint           tex_index                ;
};

void main()
{
  NyxIterator model_iterator ;
  NyxIterator proj_iterator  ;
  mat4        model          ;
  mat4        view_proj      ;
  vec4        position       ;
  vec2        tex            ;
  vec4        normal         ;
  float       weight         ;
  uint        id             ;

  model_iterator = const_transform_iterator                  ;
  proj_iterator  = const_proj_iterator                       ;
  normal         = normals                                   ;
  weight         = weights[ 0 ]                              ;
  id             = ids    [ 0 ]                              ;
  position       = vec4( vertex.x, vertex.y, vertex.z, 1.0 ) ;
  tex            = tex_coords                                ;

  nyx_seek( model_iterator, 0 ) ;
  nyx_seek( proj_iterator , 0 ) ;

  model           = nyx_get( transform_device_ptr, model_iterator ).model    ;
  view_proj       = nyx_get( proj_device_ptr, proj_iterator       ).viewproj ;
  frag_coords     = tex                                                      ;
  texture_index.x = tex_index                                                ;

  gl_Position = view_proj * model * position ;
}