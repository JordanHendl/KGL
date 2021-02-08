SET( NYX_LIBRARY_SOURCES 
     Memory.cpp
     Array.cpp
     Image.cpp
     Window.cpp
     Renderer.cpp
     Nyx.cpp
   )
      
SET( NYX_LIBRARY_HEADERS
     Memory.h
     Array.h
     Image.h
     Window.h
     Renderer.h
     Nyx.h
   )

SET( NYX_LIBRARY_INCLUDE_DIRS
   )

SET( NYX_LIBRARY_LIBRARIES
    )

ADD_LIBRARY               ( nyx_library SHARED  ${NYX_LIBRARY_SOURCES} ${NYX_LIBRARY_HEADERS} )
TARGET_LINK_LIBRARIES     ( nyx_library PUBLIC  ${NYX_LIBRARY_LIBRARIES}                      )
TARGET_INCLUDE_DIRECTORIES( nyx_library PRIVATE ${NYX_LIBRARY_INCLUDE_DIRS}                   )

INSTALL( FILES  ${NYX_LIBRARY_HEADERS} DESTINATION ${HEADER_INSTALL_DIR}/library )
INSTALL( TARGETS nyx_library EXPORT Nyx DESTINATION 
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
