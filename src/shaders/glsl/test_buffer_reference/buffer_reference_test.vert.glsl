#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable

#include "Nyx.h"

layout( location = 0 ) in  vec4 vertex ;
layout( location = 0 ) out vec2 coords ;

// The structure of data coming in.
NyxBuffer SceneData 
{
  vec3 value ;
};

// How to input data.
layout( push_constant, scalar ) uniform push_constant
{
  SceneData   device_ptr ; ///< The buffer 'reference' aka pointer.
  NyxIterator const_iter ; ///< The iterator to manage accessing the pointer.
};

void main()
{
  // Since inputting via push constant ( which is just easy ), copy off iterator.
  NyxIterator iter = const_iter ;
    
  // Now we can use C-style functions ( preprocessor directives ) to access the iterator.
  nyx_seek( iter, 0 ) ;
  vec3 tmp = nyx_get( device_ptr, iter ).value ;
    
  coords      = vertex.zw                   ;
  gl_Position = vec4( vertex.xy, 0.0, 1.0 ) ;
}