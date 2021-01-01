/*
 * Copyright (C) 2020 Jordan Hendl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Cuda.h"
#define VULKAN_HPP_NO_EXCEPTIONS
#include <algorithm>
#include <iostream>

namespace kgl
{
  namespace cg
  {
    /** Function to handle the input cuda error.
     * @param error The error to handle by the library.
     */
    static void handleError( cudaError_t error ) ;
    
    
    void handleError( cudaError_t error )
    {
      switch( error )
      {
        case cudaSuccess                           :
        case cudaErrorInvalidValue                 :
        case cudaErrorMemoryAllocation             :
        case cudaErrorInitializationError          :
        case cudaErrorCudartUnloading              :
        case cudaErrorProfilerDisabled             :
        case cudaErrorProfilerNotInitialized       :
        case cudaErrorProfilerAlreadyStarted       :
        case cudaErrorProfilerAlreadyStopped       :
        case cudaErrorInvalidConfiguration         :
        case cudaErrorInvalidPitchValue            :
        case cudaErrorInvalidSymbol                :
        case cudaErrorInvalidHostPointer           :
        case cudaErrorInvalidDevicePointer         :
        case cudaErrorInvalidTexture               :
        case cudaErrorInvalidTextureBinding        :
        case cudaErrorInvalidChannelDescriptor     :
        case cudaErrorInvalidMemcpyDirection       :
        case cudaErrorAddressOfConstant            :
        case cudaErrorTextureFetchFailed           :
        case cudaErrorTextureNotBound              :
        case cudaErrorSynchronizationError         :
        case cudaErrorInvalidFilterSetting         :
        case cudaErrorInvalidNormSetting           :
        case cudaErrorMixedDeviceExecution         :
        case cudaErrorNotYetImplemented            :
        case cudaErrorMemoryValueTooLarge          :
        case cudaErrorInsufficientDriver           :
        case cudaErrorInvalidSurface               :
        case cudaErrorDuplicateVariableName        :
        case cudaErrorDuplicateTextureName         :
        case cudaErrorDuplicateSurfaceName         :
        case cudaErrorDevicesUnavailable           :
        case cudaErrorIncompatibleDriverContext    :
        case cudaErrorMissingConfiguration         :
        case cudaErrorPriorLaunchFailure           :
        case cudaErrorLaunchMaxDepthExceeded       :
        case cudaErrorLaunchFileScopedTex          :
        case cudaErrorLaunchFileScopedSurf         :
        case cudaErrorSyncDepthExceeded            :
        case cudaErrorLaunchPendingCountExceeded   :
        case cudaErrorInvalidDeviceFunction        :
        case cudaErrorNoDevice                     :
        case cudaErrorInvalidDevice                :
        case cudaErrorStartupFailure               :
        case cudaErrorInvalidKernelImage           :
        case cudaErrorDeviceUninitialized          :
        case cudaErrorMapBufferObjectFailed        :
        case cudaErrorUnmapBufferObjectFailed      :
        case cudaErrorArrayIsMapped                :
        case cudaErrorAlreadyMapped                :
        case cudaErrorNoKernelImageForDevice       :
        case cudaErrorAlreadyAcquired              :
        case cudaErrorNotMapped                    :
        case cudaErrorNotMappedAsArray             :
        case cudaErrorNotMappedAsPointer           :
        case cudaErrorECCUncorrectable             :
        case cudaErrorUnsupportedLimit             :
        case cudaErrorDeviceAlreadyInUse           :
        case cudaErrorPeerAccessUnsupported        :
        case cudaErrorInvalidPtx                   :
        case cudaErrorInvalidGraphicsContext       :
        case cudaErrorNvlinkUncorrectable          :
        case cudaErrorJitCompilerNotFound          :
        case cudaErrorInvalidSource                :
        case cudaErrorFileNotFound                 :
        case cudaErrorSharedObjectSymbolNotFound   :
        case cudaErrorSharedObjectInitFailed       :
        case cudaErrorOperatingSystem              :
        case cudaErrorInvalidResourceHandle        :
        case cudaErrorIllegalState                 :
        case cudaErrorSymbolNotFound               :
        case cudaErrorNotReady                     :
        case cudaErrorIllegalAddress               :
        case cudaErrorLaunchOutOfResources         :
        case cudaErrorLaunchTimeout                :
        case cudaErrorLaunchIncompatibleTexturing  :
        case cudaErrorPeerAccessAlreadyEnabled     :
        case cudaErrorPeerAccessNotEnabled         :
        case cudaErrorSetOnActiveProcess           :
        case cudaErrorContextIsDestroyed           :
        case cudaErrorAssert                       :
        case cudaErrorTooManyPeers                 :
        case cudaErrorHostMemoryAlreadyRegistered  :
        case cudaErrorHostMemoryNotRegistered      :
        case cudaErrorHardwareStackError           :
        case cudaErrorIllegalInstruction           :
        case cudaErrorMisalignedAddress            :
        case cudaErrorInvalidAddressSpace          :
        case cudaErrorInvalidPc                    :
        case cudaErrorLaunchFailure                :
        case cudaErrorCooperativeLaunchTooLarge    :
        case cudaErrorNotPermitted                 :
        case cudaErrorNotSupported                 :
        case cudaErrorSystemNotReady               :
        case cudaErrorSystemDriverMismatch         :
        case cudaErrorCompatNotSupportedOnDevice   :
        case cudaErrorStreamCaptureUnsupported     :
        case cudaErrorStreamCaptureInvalidated     :
        case cudaErrorStreamCaptureMerge           :
        case cudaErrorStreamCaptureUnmatched       :
        case cudaErrorStreamCaptureUnjoined        :
        case cudaErrorStreamCaptureIsolation       :
        case cudaErrorStreamCaptureImplicit        :
        case cudaErrorCapturedEvent                :
        case cudaErrorStreamCaptureWrongThread     :
        case cudaErrorTimeout                      :
        case cudaErrorGraphExecUpdateFailure       :
        case cudaErrorUnknown                      :
        case cudaErrorApiFailureBase               :
          
          std::cout << "Error handling for Cuda Library not implemented yet." << std::endl ;
          break ;
      }
    }

    void Cuda::initialize()
    {

    }

    unsigned Cuda::convertError( unsigned error )
    {
      return 0 ;
    }

    void Cuda::copyToDevice( const void* src, Cuda::Memory& dst, Cuda::Device& gpu, unsigned amt )
    {
      cg::handleError( cudaSetDevice( gpu )                                ) ;
      cg::handleError( cudaMemcpy( dst, src, amt, cudaMemcpyHostToDevice ) ) ;
    }
    
    void Cuda::copyToHost( const Cuda::Memory& src, Cuda::Data dst, Cuda::Device& gpu, unsigned amt )
    {
      cg::handleError( cudaSetDevice( gpu )                                ) ;
      cg::handleError( cudaMemcpy( dst, src, amt, cudaMemcpyDeviceToHost ) ) ;
    }
    
    void Cuda::free( Cuda::Memory& mem, Cuda::Device& gpu )
    {
      cg::handleError( cudaSetDevice( gpu ) ) ;
      cg::handleError( cudaFree( mem )      ) ;
    }
    
    Cuda::Memory Cuda::createMemory( unsigned size, const Cuda::Device& gpu, Cuda::MemoryFlags flags )
    {
      return Cuda::createMemory( size, gpu ) ;
    }
    
    Cuda::Memory Cuda::createMemory( unsigned size, const Cuda::Device& gpu )
    {
      unsigned char* data ;

      cg::handleError( cudaSetDevice( gpu )         ) ;
      cg::handleError( cudaMalloc   ( &data, size ) ) ;
      
      return data ;
    }
  }
}
