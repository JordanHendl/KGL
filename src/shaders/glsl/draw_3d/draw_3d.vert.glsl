#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable

#include "Nyx.h"

layout ( location = 0 ) in vec4 vertex ; 

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
  NyxIterator iterator = const_iterator ;
  
  nyx_seek( iterator, 0 ) ;

  mat4 model = nyx_get( device_ptr, iterator ).model ;
  mat4 view  = nyx_get( device_ptr, iterator ).view  ;
  mat4 proj  = nyx_get( device_ptr, iterator ).proj  ;
  
  gl_Position = proj * view * model * vertex ;
  //gl_Position = vertex ;
}