
set( KGL_MEMORY_SOURCES 
     Memory.cpp
   )
      
set( KGL_MEMORY_HEADERS
     Memory.h
   )

set( KGL_MEMORY_INCLUDE_DIRS
   )

set( KGL_MEMORY_LIBRARIES
    )

ADD_LIBRARY               ( kgl_memory SHARED  ${KGL_MEMORY_SOURCES} ${KGL_MEMORY_HEADERS} )
TARGET_LINK_LIBRARIES     ( kgl_memory PUBLIC  ${KGL_MEMORY_LIBRARIES}                     )
TARGET_INCLUDE_DIRECTORIES( kgl_memory PRIVATE ${KGL_MEMORY_INCLUDE_DIRS}                  )

ADD_SUBDIRECTORY( vkg )

ADD_EXECUTABLE( kgl_memory_test Test.cpp )
TARGET_LINK_LIBRARIES( kgl_memory_test PUBLIC kgl_memory kgl_vkg )
