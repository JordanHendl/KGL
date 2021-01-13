# Find Vulkan
FIND_PACKAGE( Vulkan )

# Only build if we found vulkan.
IF( ${Vulkan_FOUND} )
    SET( KGL_VULKAN_SOURCES 
         Vulkan.cpp
         Buffer.cpp
         Device.cpp
         Instance.cpp
         Image.cpp
         RenderPass.cpp
         Queue.cpp
         Synchronization.cpp
         Pipeline.cpp
         CommandBuffer.cpp
         KgShader.cpp
       )

    SET( KGL_VULKAN_HEADERS
         Vulkan.h
         Buffer.h
         Device.h
         Instance.h
         Image.h
         RenderPass.h
         Synchronization.h
         Queue.h
         Pipeline.h
         CommandBuffer.h
         KgShader.h
       )

    SET( KGL_VULKAN_INCLUDE_DIRS
         ${Vulkan_INCLUDE_DIRS}
       )

    SET( KGL_VULKAN_LIBRARIES
         ${Vulkan_LIBRARIES}
         kgfile 
        )
    
    # Add the appropriate OS library to link depending on platform being built.
    IF( UNIX AND NOT APPLE )
      SET( KGL_VULKAN_LIBRARIES ${KGL_VULKAN_LIBRARIES} kgl_linux )
    ELSEIF( WIN32 )
      SET( KGL_VULKAN_LIBRARIES ${KGL_VULKAN_LIBRARIES} kgl_win32 )
    ENDIF()

    # Make KGL::VKG library.
    ADD_LIBRARY               ( kgl_vkg SHARED  ${KGL_VULKAN_SOURCES} ${KGL_VULKAN_HEADERS} )
    TARGET_LINK_LIBRARIES     ( kgl_vkg PUBLIC  ${KGL_VULKAN_LIBRARIES}                     )
    TARGET_INCLUDE_DIRECTORIES( kgl_vkg PRIVATE ${KGL_VULKAN_INCLUDE_DIRS}                  )
    
    INCLUDE_DIRECTORIES( ${KGL_VULKAN_INCLUDE_DIRS} )

    # If running tests, make and run them.
    IF( RUN_TESTS )
      BUILD_TEST( TARGET kgl_vkg )
    ENDIF()

    INSTALL( FILES  ${KGL_VULKAN_HEADERS} DESTINATION ${INCLUDE_DIR}/vkg )
    INSTALL( TARGETS kgl_vkg EXPORT kgl   
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}/vkg
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}/vkg
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}/vkg
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )

ENDIF()
