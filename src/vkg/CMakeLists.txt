# Find Vulkan

FIND_PACKAGE( Vulkan 1.2 REQUIRED )

# Only build if we found vulkan.
IF( ${Vulkan_FOUND} )
    SET( NYX_VULKAN_SOURCES 
         Vulkan.cpp
         Buffer.cpp
         Chain.cpp
         Device.cpp
         Descriptor.cpp
         Instance.cpp
         Image.cpp
         RenderPass.cpp
         Renderer.cpp
         Queue.cpp
         Synchronization.cpp
         Pipeline.cpp
         CommandBuffer.cpp
         Swapchain.cpp
         NyxShader.cpp
       )

    SET( NYX_VULKAN_HEADERS
         Vulkan.h
         Buffer.h
         Chain.h
         Device.h
         Descriptor.h
         Instance.h
         Image.h
         RenderPass.h
         Renderer.h
         Synchronization.h
         Queue.h
         Pipeline.h
         CommandBuffer.h
         Swapchain.h
         NyxShader.h
       )

    SET( NYX_VULKAN_INCLUDE_DIRS
         ${Vulkan_INCLUDE_DIRS}
       )

    SET( NYX_VULKAN_LIBRARIES
         ${Vulkan_LIBRARIES}
         nyx_library
         nyx_loaders 
        )
    
    # Add the appropriate OS library to link depending on platform being built.
    IF( UNIX AND NOT APPLE )
      SET( NYX_VULKAN_LIBRARIES ${NYX_VULKAN_LIBRARIES} nyx_linux )
    ELSEIF( WIN32 )
      SET( NYX_VULKAN_LIBRARIES ${NYX_VULKAN_LIBRARIES} nyx_win32 )
    ENDIF()

    # Make NYX::VKG library.
    ADD_LIBRARY               ( nyx_vkg SHARED  ${NYX_VULKAN_SOURCES} ${NYX_VULKAN_HEADERS} )
    TARGET_LINK_LIBRARIES     ( nyx_vkg PUBLIC  ${NYX_VULKAN_LIBRARIES}                     )
    TARGET_INCLUDE_DIRECTORIES( nyx_vkg PRIVATE ${NYX_VULKAN_INCLUDE_DIRS}                  )
    
    INCLUDE_DIRECTORIES( ${NYX_VULKAN_INCLUDE_DIRS} )

    BUILD_TEST( TARGET nyx_vkg )
    
    ADD_EXECUTABLE            ( nyx_vkg_window_test RenderTest.cpp                          )
    TARGET_LINK_LIBRARIES     ( nyx_vkg_window_test PUBLIC  ${NYX_VULKAN_LIBRARIES} nyx_vkg )
    TARGET_INCLUDE_DIRECTORIES( nyx_vkg_window_test PRIVATE ${NYX_VULKAN_INCLUDE_DIRS}      )

    INSTALL( FILES  ${NYX_VULKAN_HEADERS} DESTINATION ${HEADER_INSTALL_DIR}/vkg )
    INSTALL( TARGETS nyx_vkg EXPORT Nyx   
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )

ENDIF()
