cmake_minimum_required( VERSION 3.8.0 )

set( KGL_MEMORY_SOURCES 
     Memory.cpp
     Array.cpp
     Image.cpp
   )
      
set( KGL_MEMORY_HEADERS
     Memory.h
     Array.h
     Image.h
   )

set( KGL_MEMORY_INCLUDE_DIRS
   )

set( KGL_MEMORY_LIBRARIES
    )

ADD_LIBRARY               ( kgl_memory SHARED  ${KGL_MEMORY_SOURCES} ${KGL_MEMORY_HEADERS} )
TARGET_LINK_LIBRARIES     ( kgl_memory PUBLIC  ${KGL_MEMORY_LIBRARIES}                     )
TARGET_INCLUDE_DIRECTORIES( kgl_memory PRIVATE ${KGL_MEMORY_INCLUDE_DIRS}                  )

ADD_SUBDIRECTORY( vkg )
ADD_SUBDIRECTORY( cg  )