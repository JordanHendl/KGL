#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf            : enable
#include "Nyx.h"

// Standard Ngg model vertex layout.
layout ( location = 0 ) in vec2  vertex     ;
layout ( location = 1 ) in vec2  tex_coords ;

     layout( location = 0 ) out vec2 frag_coords    ;
flat layout( location = 1 ) out uint texture_index  ;

NyxBuffer TextureData
{
  uint texture_id ;
};

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
  TextureData    texture_ptr              ;
  NyxIterator    const_tex_iterator       ;
  PositionData   transform_device_ptr     ;
  NyxIterator    const_transform_iterator ;
  ProjectionData proj_device_ptr          ;
  NyxIterator    const_proj_iterator      ;
};

void main()
{
  NyxIterator model_iterator ;
  NyxIterator proj_iterator  ;
  NyxIterator tex_iterator   ;
  mat4        model          ;
  mat4        view_proj      ;
  vec4        position       ;
  vec2        tex            ;

  model_iterator = const_transform_iterator ;
  proj_iterator  = const_proj_iterator      ;
  tex_iterator   = const_tex_iterator       ;

  position = vec4( vertex.x, vertex.y, 0.0, 1.0 ) ;

  nyx_seek( model_iterator, gl_InstanceIndex ) ;
  nyx_seek( proj_iterator , gl_InstanceIndex ) ;
  nyx_seek( tex_iterator  , gl_InstanceIndex ) ;

  model           = nyx_get( transform_device_ptr, model_iterator ).model      ;
  view_proj       = nyx_get( proj_device_ptr     , proj_iterator  ).viewproj   ;
  texture_index   = nyx_get( texture_ptr         , tex_iterator   ).texture_id ;
  frag_coords     = tex_coords                                                 ;

  gl_Position = view_proj * model * position ;  
}