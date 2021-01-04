SET( KGL_LIBRARY_SOURCES 
     Memory.cpp
     Array.cpp
     Image.cpp
     Window.cpp
   )
      
SET( KGL_LIBRARY_HEADERS
     Memory.h
     Array.h
     Image.h
     Window.h
   )

SET( KGL_LIBRARY_INCLUDE_DIRS
   )

SET( KGL_LIBRARY_LIBRARIES
    )

ADD_LIBRARY               ( kgl_library SHARED  ${KGL_LIBRARY_SOURCES} ${KGL_LIBRARY_HEADERS} )
TARGET_LINK_LIBRARIES     ( kgl_library PUBLIC  ${KGL_LIBRARY_LIBRARIES}                      )
TARGET_INCLUDE_DIRECTORIES( kgl_library PRIVATE ${KGL_LIBRARY_INCLUDE_DIRS}                   )

INSTALL( FILES  ${KGL_LIBRARY_HEADERS} DESTINATION ${INCLUDE_DIR}/library )
INSTALL( TARGETS kgl_library EXPORT kgl DESTINATION 
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}/library
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )