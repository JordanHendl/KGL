FIND_PACKAGE( CUDA )

IF( ${CUDA_FOUND} )
    SET( KGL_CUDA_SOURCES 
         Cuda.cu
       )

    SET( KGL_CUDA_HEADERS
         Cuda.h
       )

    CUDA_ADD_LIBRARY     ( kgl_cg SHARED ${KGL_CUDA_SOURCES} ${KGL_CUDA_HEADERS} )
    CUDA_ADD_EXECUTABLE  ( kgl_cg_test Test.cpp                                  ) 
    TARGET_LINK_LIBRARIES( kgl_cg_test kgl_cg                                    )
ENDIF()
