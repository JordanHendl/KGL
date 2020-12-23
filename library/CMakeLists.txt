find_package( Vulkan )

if( ${Vulkan_FOUND} )
  ADD_DEFINITIONS( -DKGL_VULKAN_FOUND )
endif()

set( KGL_VULKAN_SOURCES 
     Vulkan.cpp
   )
      
set( KGL_VULKAN_HEADERS
     Vulkan.h
   )

set( KGL_VULKAN_INCLUDE_DIRS
     ${Vulkan_INCLUDE_DIRS}
   )

set( KGL_VULKAN_LIBRARIES
     ${Vulkan_LIBRARIES}
    )

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

INCLUDE_DIRECTORIES( ${KGL_VULKAN_DIR} )

ADD_LIBRARY               ( kgl_vulkan SHARED  ${KGL_VULKAN_SOURCES} ${KGL_VULKAN_HEADERS} )
TARGET_LINK_LIBRARIES     ( kgl_vulkan PUBLIC ${KGL_VULKAN_LIBRARIES}                      )
TARGET_INCLUDE_DIRECTORIES( kgl_vulkan PRIVATE ${KGL_VULKAN_INCLUDE_DIRS}                  )

ADD_LIBRARY               ( kgl_memory SHARED  ${KGL_MEMORY_SOURCES} ${KGL_MEMORY_HEADERS} )
TARGET_LINK_LIBRARIES     ( kgl_memory PUBLIC ${KGL_MEMORY_LIBRARIES}                      )
TARGET_INCLUDE_DIRECTORIES( kgl_memory PRIVATE ${KGL_MEMORY_INCLUDE_DIRS}                  )

ADD_EXECUTABLE( kgl_memory_test test.cpp )
TARGET_LINK_LIBRARIES( kgl_memory_test PUBLIC kgl_memory kgl_vulkan )

