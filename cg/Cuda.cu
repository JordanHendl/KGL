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

#ifdef KGL_CUDA_FOUND // Won't get compiler errors when building this without vulkan.


 namespace kgl
 {
   namespace cg
   {
     void Cuda::initialize()
     {

     }

     unsigned Cuda::convertError( unsigned error )
     {
       return 0 ;
     }

     void Cuda::copyTo( const void* src, Cuda::Memory& dst, Cuda::Device& gpu, unsigned amt )
     {
       cudaSetDevice( gpu ) ;
       cudaMemcpy( dst, src, amt, cudaMemcpyHostToDevice ) ;
     }
     
     void Cuda::copyTo( const Cuda::Memory& src, Cuda::Data dst, Cuda::Device& gpu, unsigned amt )
     {
       cudaSetDevice( gpu ) ;
       cudaMemcpy( dst, src, amt, cudaMemcpyDeviceToHost ) ;
     }
     
     void Cuda::free( Cuda::Memory& mem, Cuda::Device& gpu )
     {
       cudaSetDevice( gpu ) ;
       cudaFree( mem ) ;
     }
     
     Cuda::Memory Cuda::createMemory( unsigned size, const Cuda::Device& gpu, Cuda::MemoryFlags flags )
     {
       return Cuda::createMemory( size, gpu ) ;
     }
     
     Cuda::Memory Cuda::createMemory( unsigned size, const Cuda::Device& gpu )
     {
       unsigned char* data ;

       cudaSetDevice( gpu         ) ;
       cudaMalloc   ( &data, size ) ;
       
       return data ;
     }
   }
 }
#endif