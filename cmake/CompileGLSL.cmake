# Helper function to configure the test maker's global state.
FUNCTION( CONFIGURE_GLSL_COMPILE )
  BUILD_TEST( ${ARGV} )
ENDFUNCTION()

# Function to help manage building tests.
FUNCTION( GLSL_COMPILE )
  
  CMAKE_POLICY( SET CMP0057 NEW )

  # Test configurations.
  SET( VARIABLES 
        TARGETS
        INCLUDE_DIR
        NAME
     )
  
  SET( SHOULD_REBUILD FALSE )

  # For each argument provided.
  FOREACH( ARG ${ARGV} )
    # If argument is one of the variables, set it.
    IF( "${ARG}" IN_LIST VARIABLES )
      SET( STATE ${ARG} )
    ELSE()
      # If our state is a variable, set that variables value
      IF( "${${STATE}}" )
        SET( ${STATE} ${ARG} )
      ELSE()
        LIST( APPEND ${STATE} ${ARG} )
      ENDIF()

      # If our state is a setter, set the value in the parent scope as well
      IF( "${STATE}" IN_LIST CONFIGS )
        SET( ${STATE} ${${STATE}} PARENT_SCOPE )
      ENDIF()
    ENDIF()
  ENDFOREACH()

    IF( TARGETS )
      FIND_PACKAGE( Vulkan )
      IF( COMPILE_GLSL AND ${Vulkan_FOUND} )

        # Compile SPIRV for each target.
        FOREACH( ARG ${TARGETS} )
          
          # Get GLSL timestamp and check against the cache, if it exists.
          FILE( TIMESTAMP "${CMAKE_CURRENT_SOURCE_DIR}/${ARG}" MODIFIED_TIME "%m%d%Y%H%M%S" )
          IF( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/ShaderCache/${ARG}.cache )
            FILE( READ      ${CMAKE_CURRENT_BINARY_DIR}/ShaderCache/${ARG}.cache CACHED_TIME )
          ENDIF()
          
          # If there's been a change, or if this is the first time compiling
          IF( NOT MODIFIED_TIME EQUAL CACHED_TIME )
            # Check if we can build spirv with glslang validator.
            FIND_PROGRAM( HAS_GLSLANG_VALIDATOR "glslangValidator" )

            # Build Spirv
            IF( HAS_GLSLANG_VALIDATOR )
         
              #Compile SPIRV
              ADD_CUSTOM_COMMAND(
                POST_BUILD
                OUTPUT ${ARG}_spirv_compilation
                COMMAND glslangValidator -I${GLSL_INCLUDE_DIR} -V -o ${NYXFILE_DIR}/spirv/${ARG}.h --vn spirv ${ARG}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
              )
          
              ADD_CUSTOM_TARGET(
                ${ARG}_spirv_compile_flag ALL
                DEPENDS ${ARG}_spirv_compilation
              )
            ENDIF()
            
            # Now, tell nyxfiles to rebuild for this target, and write out to cache.
            SET ( SHOULD_REBUILD TRUE                                                         )
            FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ShaderCache/${ARG}.cache ${MODIFIED_TIME} )

          ENDIF()
        ENDFOREACH()

        # Replace all delimiters of the string with a space.
        STRING ( REPLACE ";" " \n"  NEW_TARGETS "${TARGETS}")
        
        IF( ${SHOULD_REBUILD} )
          # Check if we can build .nyx files with nyxmaker.
          FIND_PROGRAM( HAS_NYXMAKER "nyxmaker" )

          # Build nyxfiles using nyxmaker.
          IF( HAS_NYXMAKER )
            ADD_CUSTOM_COMMAND(
              POST_BUILD
              OUTPUT ${NAME}_compilation
              COMMAND nyxmaker -v -h -i ${GLSL_INCLUDE_DIR} ${TARGETS} -o ${NYXFILE_DIR}/${NAME}
              WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
    
            ADD_CUSTOM_TARGET(
              ${NAME}_compile_flag ALL
              DEPENDS ${NAME}_compilation
            )
          ELSE()
            MESSAGE( INFO "Tried to build .nyx file using ${TARGETS}, but nyxmaker was not found." )
          ENDIF()
        ENDIF()
      ENDIF()
    ENDIF()
ENDFUNCTION()