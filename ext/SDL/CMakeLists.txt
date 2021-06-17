SET( EXT_SDL_VER       2.0                           )
SET( EXT_SDL_REL       14                            )
SET( EXT_SDL_REL_VER   ${EXT_SDL_VEW}.${EXT_SDL_REL} )

SET( EXT_SDL_TAR_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/SDL2-2.0.14.tar.gz ) 
SET( EXT_SDL_BLD_DIR   ${CMAKE_BINARY_DIR}/ext/SDL/sdl                )
SET( EXT_SDL_SRC_DIR   ${EXT_SDL_BLD_DIR}/src/SDL2-2.0.14/            )
SET( EXT_SDL_CMAKE_DIR ${EXT_SDL_SRC_DIR}                             )
SET( EXT_SDL_LIB_DIR   ${EXT_SDL_BLD_DIR}/lib                         )
SET( EXT_SDL_INCS      ${EXT_SDL_SRC_DIR}/include                     )
IF( WIN32 )
  SET( EXT_SDL_LIB     ${EXT_SDL_BLD_DIR}/Release/SDL2.lib                                     )
  SET( EXT_SDL_LIBS    ${EXT_SDL_BLD_DIR}/Release/SDL2.dll ${EXT_SDL_BLD_DIR}/Release/SDL2.lib )
ENDIF()

IF( UNIX )
  SET( EXT_SDL_LIBS    ${EXT_SDL_BLD_DIR}/libSDL2-2.0.so.0.14.0 ${EXT_SDL_BLD_DIR}/libSDL2-2.0.so.0 )
  SET( EXT_SDL_LIB     ${EXT_SDL_BLD_DIR}/libSDL2-2.0.so.0                                          )
ENDIF()

FILE( MAKE_DIRECTORY ${EXT_SDL_BLD_DIR}     )
FILE( MAKE_DIRECTORY ${EXT_SDL_BLD_DIR}/src )

MESSAGE( INFO "Untarring SDL2 library source" )
EXECUTE_PROCESS( 
  COMMAND tar -xf ${EXT_SDL_TAR_DIR}
  WORKING_DIRECTORY ${EXT_SDL_BLD_DIR}/src
  )

EXECUTE_PROCESS( 
  COMMAND ${CMAKE_COMMAND} ${EXT_SDL_CMAKE_DIR} 
  WORKING_DIRECTORY ${EXT_SDL_BLD_DIR}
  )

IF( WIN32 )
  ADD_CUSTOM_COMMAND(
    PRE_BUILD
    OUTPUT ${EXT_SDL_LIBS}
    COMMAND MSBuild SDL2.sln /property:Configuration=Release -maxcpucount:7
    WORKING_DIRECTORY ${EXT_SDL_BLD_DIR}
    )
ENDIF()

IF( UNIX )
  ADD_CUSTOM_COMMAND(
    PRE_BUILD
    OUTPUT ${EXT_SDL_LIBS}
    COMMAND $(MAKE)
    WORKING_DIRECTORY ${EXT_SDL_BLD_DIR}
    )
ENDIF()

ADD_CUSTOM_TARGET(
  EXT_SDL ALL
  DEPENDS ${EXT_SDL_LIBS}
)

SET( SDL2_LIBRARIES    ${EXT_SDL_LIBS} CACHE INTERNAL "SDL2 Libaries"     )
SET( SDL2_INCLUDE_DIRS ${EXT_SDL_INCS} CACHE INTERNAL "SDL2 Include Dirs" )

INSTALL( FILES ${EXT_SDL_LIBS} DESTINATION ${EXPORT_LIB_DIR} COMPONENT release )
