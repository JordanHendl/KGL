# 
# Copyright (C) 2020 Jordan Hendl
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 

# 
# File:   CMakeLists.h
# Author: Jordan Hendl
# 
# Created on December 31, 2020, 11:30 PM
# 

CMAKE_MINIMUM_REQUIRED( VERSION 3.18.0 )

LIST( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )

INCLUDE( Message     )
INCLUDE( BuildTest   )

# Build options
OPTION( BUILD_CUDA    "Whether or not the Nyx::CUDA library will be built, if available"   ON  )
OPTION( BUILD_VULKAN  "Whether or not the Nyx::VKG library will be built, if available"    ON  )
OPTION( BUILD_WINDOW  "Whether or not the Nyx::Window library will be built, if available" ON  )
OPTION( BUILD_DOCS    "Whether or not to generate documentation of Nyx"                    OFF )
OPTION( BUILD_RELEASE "Whether or not the to build for release."                           OFF )
OPTION( BUILD_TESTS   "Whether or not the Nyx tests should be built"                       ON  )
OPTION( COMPILE_GLSL  "Whether or not the to compile all glsl shaders."                    ON  )
OPTION( RUN_TESTS     "Whether or not the Nyx tests should be run"                         ON  )

PROJECT( NyxGPU CXX )

MESSAGE( INFO "Nyx GPU Library Prebuild" ) 
MESSAGE( INFO " " ) 

IF( BUILD_RELEASE  )
    MESSAGE( INFO "Building for release" )
    IF( MSVC )
      ADD_COMPILE_OPTIONS( /W2 /02 )
    ELSEIF( UNIX AND NOT APPLE )
      ADD_COMPILE_OPTIONS( -Wall -Wextra -pedantic -Werror -fPIC -O2 )
    ENDIF()
ELSE()
    MESSAGE( INFO "Building for debug" )
    IF( MSVC )
      ADD_COMPILE_OPTIONS( /W2 /DEBUG )
    ELSEIF( UNIX )
      ADD_COMPILE_OPTIONS( -Wall -Wextra -pedantic -Werror -fPIC -g )
    ENDIF()
ENDIF()

# Print build options
MESSAGE( STATUS "" ) 
MESSAGE( INFO "Build Options:" ) 
MESSAGE( INFO "├─BUILD_WIN32    ${BUILD_WIN32}  " )
MESSAGE( INFO "├─BUILD_LINUX    ${BUILD_LINUX}  " )
MESSAGE( INFO "├─BUILD_CUDA     ${BUILD_CUDA}   " )
MESSAGE( INFO "├─BUILD_VULKAN   ${BUILD_VULKAN} " )
MESSAGE( INFO "├─BUILD_RELEASE  ${BUILD_RELEASE}" )
MESSAGE( INFO "└─RUN_TESTS      ${RUN_TESTS}    " )
MESSAGE( STATUS "" ) 

# Set build config.
SET( ARCHITECTURE      "64bit" CACHE STRING "The system architecture."                     )
SET( CXX_STANDARD      "17"    CACHE STRING "The C++ standard to use for building."        )
SET( MAJOR             "0"     CACHE STRING "The major version of this build."             )
SET( MINOR             "0"     CACHE STRING "The minor version of this build."             )
SET( BRANCH            "0"     CACHE STRING "The branch version of this build."            )
SET( REVISION          "0"     CACHE STRING "The revision of this build."                  )
SET( GENERATOR         "DEB"   CACHE STRING "The Package Generator to use for this build." )

IF( WIN32 )
  SET( INSTALL_LOCATION  "C:\\Program Files"         CACHE STRING "The default NSIS install location of this library" )
  SET( CMAKE_PREFIX_PATH "C:\\Program Files\\Athena" CACHE STRING "The default path to look for dependancies."        )
ELSEIF( UNIX AND NOT APPLE )
  SET( INSTALL_LOCATION  "/usr/local/lib"                            CACHE STRING "The RPM install location of this library."  ) 
  SET( CMAKE_PREFIX_PATH "/usr/local/lib/Athena;/usr/lib;/usr/local" CACHE STRING "The default path to look for dependancies." )
ENDIF()

SET(CMAKE_CXX_STANDARD          17 )
SET(CMAKE_CXX_STANDARD_REQUIRED ON )

SET( PROJECT_VERSION "${MAJOR}.${MINOR}.${BRANCH}" )

# Print build configurations.
MESSAGE( INFO " " ) 
MESSAGE( INFO "Build Configuration:"                      )
MESSAGE( INFO "├─ARCHITECTURE      : ${ARCHITECTURE}"     )
MESSAGE( INFO "├─INSTALL GENERATOR : ${GENERATOR}"        )
MESSAGE( INFO "├─INSTALL LOCATION  : ${INSTALL_LOCATION}" )
MESSAGE( INFO "├─CXX_STANDARD      : ${CXX_STANDARD}"     )
MESSAGE( INFO "└─VERSION           : ${PROJECT_VERSION}"  )
MESSAGE( INFO "  ├─MAJOR           : ${MAJOR}"            )
MESSAGE( INFO "  ├─MINOR           : ${MINOR}"            )
MESSAGE( INFO "  ├─BRANCH          : ${BRANCH}"           )
MESSAGE( INFO "  └─REVISION        : ${REVISION}"         )

STRING( TIMESTAMP DATE "%Y%m%d" )
STRING( TIMESTAMP Time "%H%M%S" )

# Handle compiler information
STRING( TOLOWER ${CMAKE_CXX_COMPILER_ID} CXX_COMPILER_NAME )
SET( CXX_COMPILER  "${CXX_COMPILER_NAME}${CMAKE_CXX_COMPILER_VERSION}" )

SET( CLANG FALSE )
SET( INTEL FALSE )
SET( GNU   FALSE )
SET( MSVC  FALSE )
IF( CXX_COMPILER_NAME MATCHES "clang" )
  SET( CLANG TRUE )
ELSEIF( CXX_COMPILER_NAME MATCHES "gnu" )
  ADD_COMPILE_OPTIONS( -fno-rtti )
  ADD_COMPILE_OPTIONS( -fno-exceptions )
  SET( GNU TRUE )
ELSEIF( CXX_COMPILER_NAME MATCHES "intel" )
  SET( INTEL TRUE )
ELSEIF( CXX_COMPILER_NAME MATCHES "msvc" )
  SET( MSVC TRUE )
ENDIF()

MESSAGE( INFO " " ) 
MESSAGE( INFO  "System Configuration:"                     )
MESSAGE( INFO  "├─CXX_COMPILER_NAME: ${CXX_COMPILER_NAME}" )
MESSAGE( INFO  "├─CXX_COMPILER:      ${CXX_COMPILER}"      )
MESSAGE( INFO  "├─INTEL:             ${INTEL}"             )
MESSAGE( INFO  "├─GNU:               ${GNU}"               )
MESSAGE( INFO  "└─MSVC:              ${MSVC}"              )
MESSAGE( INFO " " ) 




IF( UNIX )
  SET( BIN_DIR     bin     )
  SET( INCLUDE_DIR include )
  SET( LIB_DIR     lib64   )
  SET( DOC_DIR     doc     )
ELSE()
  SET( BIN_DIR     "." )
  SET( INCLUDE_DIR "." )
  SET( LIB_DIR     "." )
  SET( DOC_DIR     "." )
ENDIF()

SET( CONFIG_DIR         ${CMAKE_PROJECT_NAME}                )
SET( DOXYGEN_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/doc      )
SET( BUILD_DIR          ${CMAKE_BINARY_DIR}                  )
SET( HEADER_INSTALL_DIR ${INCLUDE_DIR}/${CMAKE_PROJECT_NAME} )

SET( EXPORT_BIN_DIR     "${BIN_DIR}"     )
SET( EXPORT_INCLUDE_DIR "${INCLUDE_DIR}" )
SET( EXPORT_LIB_DIR     "${LIB_DIR}"     )
SET( EXPORT_DOC_DIR     "${DOC_DIR}"     )
SET( EXPORT_CONFIG_DIR  "${CONFIG_DIR}"  )

# Set output directories.
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64 )
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64 )
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin   )
SET(DOCUMENTATION_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc   )

IF( MSVC )
    SET( WINDOWS_EXPORT_ALL_SYMBOLS       TRUE )
    SET( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE )
    SET( BUILD_SHARED_LIBS TRUE                )
ENDIF()

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/src ) 

INCLUDE ( BuildDocs )

SET( CPACK_COMPONENTS_ALL                 release devel                                                                   )
SET( CPACK_COMPONENT_DEVEL_DISPLAY_NAME   "${CMAKE_PROJECT_NAME}-devel"                                                   )
SET( CPACK_COMPONENT_RELEASE_DISPLAY_NAME "${CMAKE_PROJECT_NAME}-release"                                                 )
SET( CPACK_COMPONENT_DEVEL_DESCRIPTION    "Development Headers & Runtime Libraries for the ${CMAKE_PROJECT_NAME} library" )
SET( CPACK_COMPONENT_RELEASE_DESCRIPTION  "Runtime Libraries for the ${CMAKE_PROJECT_NAME} library"                       )
SET( CPACK_COMPONENT_DEVEL_DEPENDS        release                                                                         )
INCLUDE( KPackage ) 

ADD_SUBDIRECTORY( src )
INSTALL( EXPORT Nyx FILE NyxGPUConfig.cmake DESTINATION ${CMAKE_PROJECT_NAME} COMPONENT devel )
