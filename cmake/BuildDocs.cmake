FIND_PACKAGE( Doxygen REQUIRED dot )

IF ( ${DOXYGEN_FOUND} AND BUILD_DOCS )
    # set input and output files
    SET( DOXYGEN_IN  ${DOXYGEN_DIR}/doxygen.cfg  )
    SET( DOXYGEN_OUT ${DOC_DIR}/doc/doxygen.cfg  )
    
    SET( DOC_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/ )

    # request to configure the file
    CONFIGURE_FILE( ${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY )

    MESSAGE( "Generating ${CMAKE_PROJECT_NAME} documentation." )

    # note the option ALL which allows to build the docs together with the application
    ADD_CUSTOM_TARGET( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )

ELSEIF( BUILD_DOCS )
  MESSAGE( "Doxygen need to be installed to generate documentation." )

ENDIF()