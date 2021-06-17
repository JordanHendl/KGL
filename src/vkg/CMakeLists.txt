# Only build if we found vulkan.
    SET( NYX_VULKAN_SOURCES 
         Vulkan.cpp
         Buffer.cpp
         Chain.cpp
         Device.cpp
         Descriptor.cpp
         Instance.cpp
         Image.cpp
         RenderPass.cpp
         Pipeline.cpp
         Queue.cpp
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
         Pipeline.h
         Queue.h
         Pipeline.h
         CommandBuffer.h
         Swapchain.h
         NyxShader.h
       )

    SET( NYX_VULKAN_INCLUDE_DIRS
         ${Vulkan_INCLUDE_DIRS}
         ${SDL2_INCLUDE_DIRS}
       )

    SET( NYX_VULKAN_LIBRARIES
	       ${Vulkan_LIBRARIES}
         ${SDL2_LIBRARIES}
         nyx_library
         nyx_loaders 
         nyx_window
        )

    # Make NYX::VKG library.
    ADD_LIBRARY               ( nyx_vkg SHARED  ${NYX_VULKAN_SOURCES} ${NYX_VULKAN_HEADERS} )
    TARGET_LINK_LIBRARIES     ( nyx_vkg PUBLIC  ${NYX_VULKAN_LIBRARIES}                     )
    TARGET_INCLUDE_DIRECTORIES( nyx_vkg PRIVATE ${NYX_VULKAN_INCLUDE_DIRS}                  )
    ADD_DEPENDENCIES          ( nyx_vkg EXT_SDL                                            )

    INCLUDE_DIRECTORIES( ${NYX_VULKAN_INCLUDE_DIRS} )

    BUILD_TEST( TARGET nyx_vkg )
    
    INSTALL( FILES  ${NYX_VULKAN_HEADERS} DESTINATION ${HEADER_INSTALL_DIR}/vkg COMPONENT devel )
    INSTALL( TARGETS nyx_vkg EXPORT Nyx COMPONENT release 
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
