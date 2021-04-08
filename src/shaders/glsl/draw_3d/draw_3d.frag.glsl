#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) out vec4 out_color ; ///< The output color of this render pass.

void main()
{
  out_color = vec4( 0.0, 0.5, 0.0, 1.0 ) ;
}