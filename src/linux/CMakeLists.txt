FIND_PACKAGE( X11 ) 

IF( ${X11_FOUND} )

  SET( KGL_LINUX_SOURCES 
       Linux.cpp
       Window.cpp
     )
        
  SET( KGL_LINUX_HEADERS
       Linux.h
       Window.h
     )
  
  SET( KGL_LINUX_LIBRARIES
        ${X11_LIBRARIES}
        kgl_event 
      )
  
  ADD_LIBRARY               ( kgl_linux SHARED  ${KGL_LINUX_SOURCES} ${KGL_LINUX_HEADERS} )
  TARGET_LINK_LIBRARIES     ( kgl_linux PUBLIC  ${KGL_LINUX_LIBRARIES}                    )
  TARGET_INCLUDE_DIRECTORIES( kgl_linux PRIVATE ${KGL_LINUX_INCLUDE_DIRS}                 )
  
  BUILD_TEST( TARGET kgl_linux )

  INSTALL( FILES  ${KGL_LINUX_HEADERS}  DESTINATION  ${INCLUDE_DIR}/linux )
  INSTALL( TARGETS kgl_linux EXPORT kgl 
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}/linux
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}/linux
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}/linux
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
ENDIF()

