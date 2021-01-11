IF( WIN32 )

  SET( KGL_WIN32_SOURCES 
       Win32.cpp
       Window.cpp
     )
        
  SET( KGL_WIN32_HEADERS
       Win32.h
       Window.h
     )
  
  SET( KGL_WIN32_LIBRARIES
        kgl_event 
      )
  
  ADD_LIBRARY               ( kgl_win32 SHARED  ${KGL_WIN32_SOURCES} ${KGL_WIN32_HEADERS} )
  TARGET_LINK_LIBRARIES     ( kgl_win32 PUBLIC  ${KGL_WIN32_LIBRARIES}                    )
  TARGET_INCLUDE_DIRECTORIES( kgl_win32 PRIVATE ${KGL_WIN32_INCLUDE_DIRS}                 )
          # Add Test executable.
		  FIND_PACKAGE( KT )
        ADD_EXECUTABLE       ( kgl_win32_test Test.cpp         )
        TARGET_LINK_LIBRARIES( kgl_win32_test PUBLIC kgl_win32 )

  # If running tests, make and run them.
  IF( RUN_TESTS )
    #BUILD_TEST( TARGET kgl_win32 )
  ENDIF()

  INSTALL( FILES  ${KGL_WIN32_HEADERS}  DESTINATION  ${INCLUDE_DIR}/win32 )
  INSTALL( TARGETS kgl_win32 EXPORT kgl 
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}/win32
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
ENDIF()

