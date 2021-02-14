FIND_PACKAGE( X11 ) 

IF( UNIX AND NOT APPLE )

  SET( NYX_LINUX_SOURCES 
       Linux.cpp
       Window.cpp
     )
        
  SET( NYX_LINUX_HEADERS
       Linux.h
       Window.h
     )
  
  SET( NYX_LINUX_LIBRARIES
        ${X11_LIBRARIES}
        nyx_event 
      )
  
  ADD_LIBRARY               ( nyx_linux SHARED  ${NYX_LINUX_SOURCES} ${NYX_LINUX_HEADERS} )
  TARGET_LINK_LIBRARIES     ( nyx_linux PUBLIC  ${NYX_LINUX_LIBRARIES}                    )
  TARGET_INCLUDE_DIRECTORIES( nyx_linux PRIVATE ${NYX_LINUX_INCLUDE_DIRS}                 )
  
  BUILD_TEST( TARGET nyx_linux )

  INSTALL( FILES  ${NYX_LINUX_HEADERS}  DESTINATION  ${HEADER_INSTALL_DIR}/linux )
  INSTALL( TARGETS nyx_linux EXPORT Nyx 
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
ENDIF()

