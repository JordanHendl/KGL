FIND_PACKAGE( CUDA )

IF( ${CUDA_FOUND} )
    SET( NYX_CUDA_SOURCES 
         Cuda.cu
       )

    SET( NYX_CUDA_HEADERS
         Cuda.h
       )

    CUDA_ADD_LIBRARY( nyx_cg SHARED ${NYX_CUDA_SOURCES} ${NYX_CUDA_HEADERS} )

    # If running tests, make and run them.
    BUILD_TEST( TARGET nyx_cg )

    INSTALL( FILES  ${NYX_CUDA_HEADERS} DESTINATION ${INCLUDE_DIR}/cuda )
    INSTALL( TARGETS nyx_cg EXPORT Nyx 
             ARCHIVE  DESTINATION ${EXPORT_LIB_DIR}/cuda
             RUNTIME  DESTINATION ${EXPORT_LIB_DIR}/cuda
             LIBRARY  DESTINATION ${EXPORT_LIB_DIR}/cuda
             INCLUDES DESTINATION ${EXPORT_INCLUDE_DIR} )
ENDIF()
