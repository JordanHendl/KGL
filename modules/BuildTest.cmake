# Helper function to configure the test maker's global state.
FUNCTION( CONFIGURE_TEST )
  BUILD_TEST( ${ARGV} )
ENDFUNCTION()

# Function to help manage building tests.
FUNCTION( BUILD_TEST )
  
  # Global configurations.
  SET( CONFIGS
        RUN_TESTS
        TEST_LIBRARY
     )
  
  # Test configurations.
  SET( VARIABLES 
        TARGET
        LANGUAGE
     )
  
  # For each argument provided.
  FOREACH( ARG ${ARGV} )

    # If argument is one of the variables, set it.
    IF( "${ARG}" IN_LIST VARIABLES OR "${ARG}" IN_LIST CONFIGS )
      SET( STATE ${ARG} )
    ENDIF()
    
    # Add Test executable.
    SET                  ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/${TEST_DIR} )
    ADD_EXECUTABLE       ( ${TARGET}_test Test.cpp                                 )
    TARGET_LINK_LIBRARIES( ${TARGET}_test ${TARGET} ${TEST_LIBRARY}                )

    # If we should run tests, add custom command to run them after the fact.
    IF( RUN_TESTS )
      ADD_CUSTOM_COMMAND(
        POST_BUILD
        OUTPUT ${TARGET}_test_execution
        COMMAND ${TARGET}_test
        WORKING_DIRECTORY ${BUILD_DIR}/${TEST_DIR}
      )

      ADD_CUSTOM_TARGET(
        ${TARGET}_test_flag ALL
        DEPENDS ${TARGET}_test_exec
      )
    ENDIF()
  ENDFOREACH()
ENDFUNCTION()

