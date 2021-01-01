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

ADD_LIBRARY               ( kgl_memory SHARED  ${KGL_LIBRARY_SOURCES} ${KGL_LIBRARY_HEADERS} )
TARGET_LINK_LIBRARIES     ( kgl_memory PUBLIC  ${KGL_LIBRARY_LIBRARIES}                      )
TARGET_INCLUDE_DIRECTORIES( kgl_memory PRIVATE ${KGL_LIBRARY_INCLUDE_DIRS}                   )

INSTALL( FILES  ${KGL_LIBRARY_HEADERS} DESTINATION ${INCLUDE_DIR} )
INSTALL( TARGETS kgl_memory EXPORT kgl DESTINATION ${LIB_DIR}     )