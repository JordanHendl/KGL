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
      )
  
  ADD_LIBRARY               ( kgl_linux SHARED  ${KGL_LINUX_SOURCES} ${KGL_LINUX_HEADERS} )
  TARGET_LINK_LIBRARIES     ( kgl_linux PUBLIC  ${KGL_LINUX_LIBRARIES}                    )
  TARGET_INCLUDE_DIRECTORIES( kgl_linux PRIVATE ${KGL_LINUX_INCLUDE_DIRS}                 )
  
  ADD_EXECUTABLE            ( kgl_linux_test Test.cpp         )
  TARGET_LINK_LIBRARIES     ( kgl_linux_test PUBLIC kgl_linux )

ENDIF()

