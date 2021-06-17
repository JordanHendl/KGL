SET( EXT_VULKAN_VER       1.2                            )
SET( EXT_VULKAN_REL       170                            )
SET( EXT_VULKAN_REL_VER   ${EXT_VULKAN_VER}.${EXT_VULKAN_REL} )

SET( EXT_VULKAN_TAR_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/vulkansdk-linux-x86_64-1.2.170.0.tar.gz ) 
SET( EXT_VULKAN_BLD_DIR   ${CMAKE_BINARY_DIR}/ext/Vulkan/vulkan_sdk                           )
SET( EXT_VULKAN_SRC_DIR   ${EXT_VULKAN_BLD_DIR}/src/1.2.170.0/                                )
SET( EXT_VULKAN_CMAKE_DIR ${EXT_VULKAN_SRC_DIR}                                               )
SET( EXT_VULKAN_LIB_DIR   ${EXT_VULKAN_SRC_DIR}/x86_64/lib                                    )
SET( EXT_VULKAN_INCS      ${EXT_VULKAN_SRC_DIR}/x86_64/include/                               )

FILE( MAKE_DIRECTORY ${EXT_VULKAN_BLD_DIR}     )
FILE( MAKE_DIRECTORY ${EXT_VULKAN_BLD_DIR}/src )

IF( WIN32 )
  FIND_PACKAGE( Vulkan REQUIRED )
  SET( EXT_VULKAN_LIB  ${Vulkan_LIBRARY}      )
  SET( EXT_VULKAN_LIBS ${Vulkan_LIBRARIES}    )
  SET( EXT_VULKAN_INCS ${Vulkan_INCLUDE_DIRS} )
ENDIF()

IF( UNIX )
  SET( EXT_VULKAN_LIB      ${EXT_VULKAN_LIB_DIR}/libvulkan.so                                                                                           )
  SET( EXT_VULKAN_LIBS ${EXT_VULKAN_LIB_DIR}/libvulkan.so.${EXT_VULKAN_REL_VER};${EXT_VULKAN_LIB_DIR}/libvulkan.so.1 ${EXT_VULKAN_LIB_DIR}/libvulkan.so )
  
MESSAGE( INFO "Untarring Vulkan SDK" )
EXECUTE_PROCESS( 
  COMMAND tar -xf ${EXT_VULKAN_TAR_DIR}
  WORKING_DIRECTORY ${EXT_VULKAN_BLD_DIR}/src
  )

ENDIF()


SET( Vulkan_LIBRARIES    ${EXT_VULKAN_LIBS} CACHE INTERNAL "Vulkan Libary dir"   )
SET( Vulkan_INCLUDE_DIRS ${EXT_VULKAN_INCS} CACHE INTERNAL "Vulkan Include Dirs" )

INSTALL( FILES ${EXT_VULKAN_LIBS} DESTINATION ${EXPORT_LIB_DIR} COMPONENT release )
