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
    BUILD_TEST( TARGET kgl_cg )

    INSTALL( FILES  ${KGL_CUDA_HEADERS} DESTINATION ${INCLUDE_DIR}/cg )
    INSTALL( TARGETS kgl_cg EXPORT kgl 
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}/cg
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}/cg
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}/cg
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
ENDIF()
