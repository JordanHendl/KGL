SET( NYX_WINDOW_SOURCES 
     Window.cpp
   )
      
SET( NYX_WINDOW_HEADERS
     Window.h
   )

SET( NYX_WINDOW_INCLUDE_DIRS
     ${SDL2_INCLUDE_DIRS}
   )

SET( NYX_WINDOW_LIBRARIES
      ${SDL2_LIBRARIES}
      nyx_event 
    )

ADD_LIBRARY               ( nyx_window SHARED  ${NYX_WINDOW_SOURCES} ${NYX_WINDOW_HEADERS} )
TARGET_LINK_LIBRARIES     ( nyx_window PUBLIC  ${NYX_WINDOW_LIBRARIES}                     )
TARGET_INCLUDE_DIRECTORIES( nyx_window PRIVATE ${NYX_WINDOW_INCLUDE_DIRS}                  )
ADD_DEPENDENCIES          ( nyx_window EXT_SDL                                             )
BUILD_TEST( TARGET nyx_window )

INSTALL( TARGETS nyx_window EXPORT Nyx COMPONENT release 
           RUNTIME  DESTINATION ${EXPORT_LIB_DIR}
           ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}
           LIBRARY  DESTINATION ${EXPORT_LIB_DIR}
           INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )

