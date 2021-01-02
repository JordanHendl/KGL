FIND_PACKAGE( CUDA )

IF( ${CUDA_FOUND} )
    SET( KGL_CUDA_SOURCES 
         Cuda.cu
       )

    SET( KGL_CUDA_HEADERS
         Cuda.h
       )

    CUDA_ADD_LIBRARY     ( kgl_cg SHARED ${KGL_CUDA_SOURCES} ${KGL_CUDA_HEADERS} )

    # If running tests, make and run them.
    IF( RUN_TESTS )
#      BUILD_TEST( TARGET kgl_cg )
    ENDIF()

    INSTALL( FILES  ${KGL_CUDA_HEADERS} DESTINATION ${INSTALL_INCLUDE_DIR}/cuda )
    INSTALL( TARGETS kgl_cg EXPORT kgl 
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}/cuda
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR}/cuda )
ENDIF()
