# Helper function to configure the test maker's global state.
FUNCTION( CONFIGURE_GLSL_COMPILE )
  BUILD_TEST( ${ARGV} )
ENDFUNCTION()

# Function to help manage building tests.
FUNCTION( GLSL_COMPILE )
  
  CMAKE_POLICY( SET CMP0057 NEW )

  # Test configurations.
  SET( VARIABLES 
        TARGET
        INCLUDE_DIR
     )
  
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

    IF( TARGET )
      IF( COMPILE_GLSL )
        FIND_PACKAGE( Vulkan REQUIRED )

        MESSAGE( INFO "glslangValidator -V -o ${SPIRV_DIR}/glsl/${TARGET}.glsl.h --vn shader ${TARGET}" )
        FILE( COPY ${TARGET} DESTINATION ${SPIRV_DIR}/glsl )
        ADD_CUSTOM_COMMAND(
          POST_BUILD
          OUTPUT ${TARGET}_compilation
          COMMAND glslangValidator -I${GLSL_INCLUDE_DIR} -V -o ${SPIRV_DIR}/glsl/${TARGET}.glsl.h --vn shader ${TARGET}
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    
        ADD_CUSTOM_TARGET(
          ${TARGET}_compile_flag ALL
          DEPENDS ${TARGET}_compilation
        )
      ENDIF()
    ENDIF()
ENDFUNCTION()