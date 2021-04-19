SET( GLSL_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/includes )

INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/includes/Nyx.h DESTINATION ${CMAKE_PROJECT_NAME}/glsl/ COMPONENT devel )
ADD_SUBDIRECTORY( draw                  )
ADD_SUBDIRECTORY( graph_draw_model      )
ADD_SUBDIRECTORY( draw_3d               )
ADD_SUBDIRECTORY( test                  )
ADD_SUBDIRECTORY( test_buffer_reference )
ADD_SUBDIRECTORY( test_subpass          )