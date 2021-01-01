IF( WIN32 )
 SET( CPACK_GENRATOR          "NSIS"                        )
 SET( CPACK_NSIS_PACKAGE_NAME install_${CMAKE_PROJECT_NAME} )
 SET( CONFIG_INSTALL_DIR      ""                            )
ENDIF() 

IF( UNIX )
  SET( CPACK_GENERATOR    "RPM"   )
  SET( CONFIG_INSTALL_DIR "local" )
ENDIF() 

SET( CMAKE_INSTALL_PREFIX         "${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}" )
SET( CPACK_PACKAGE_VERSION        ${PROJECT_VERSION}                              )
SET( CPACK_PACKAGE_NAME           install_${CMAKE_PROJECT_NAME}                   )
SET( CPACK_INSTALL_PREFIX         ${CMAKE_INSTALL_PREFIX}                         )

INCLUDE( CPack )