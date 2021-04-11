SET( GLSL_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/includes )

ADD_SUBDIRECTORY( draw                  )
ADD_SUBDIRECTORY( draw_3d               )
ADD_SUBDIRECTORY( test                  )
ADD_SUBDIRECTORY( test_buffer_reference )
ADD_SUBDIRECTORY( test_subpass          )