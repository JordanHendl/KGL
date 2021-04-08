#version 450 core
#extension GL_GOOGLE_include_directive    : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf            : enable
#include "Nyx.h"

layout( location = 0 ) out vec4 out_color ; ///< The output color of this render pass.

// The structure of data coming in.
NyxBuffer SceneData 
{
  mat4 model ;
  mat4 view  ;
  mat4 proj  ;
};

NyxPushConstant push
{
  SceneData   ptr  ;
  NyxIterator iter ;
};

void main()
{  
  //debugPrintfEXT("Pointer Address: %d", ptr ) ;
  mat4 model = ptr.model ;

  if( model[0][0] < 0.5 ) out_color = vec4( 0.5, 0.0, 0.0, 1.0 ) ;
  else                    out_color = vec4( 0.0, 0.5, 0.0, 1.0 ) ;
}