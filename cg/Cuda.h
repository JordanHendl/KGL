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

#ifndef KGL_CUDA_H
#define KGL_CUDA_H



namespace kgl
{
  
  /** Forward declared Memory object for friendship.
   */
  template<typename IMPL>
  class Memory ;
  
  template<typename IMPL, typename TYPE>
  class Array ;

  namespace cg
  {
    class Cuda
    {
      public:
        using Device         = unsigned            ;
        using Buffer         = ::kgl::Memory<Cuda> ;
        using Memory         = unsigned char*      ;
        using CommandRecord  = unsigned            ;
        using MemoryFlags    = unsigned            ;

        /** Static method to initialize this implementation.
         */
        static void initialize() ;
        
        /** Static method to convert a cuda error to an error of the library.
         * @param error An error defined by cuda.
         * @return An error defined by the library.
         */
        static unsigned convertError( unsigned error ) ;

      private:
        
        
        /** Typedef to avoid using void* directly.
         */
        typedef void* Data ;
        
        /** Friend declaration of memory object to use this implementation for.
         */
        friend class ::kgl::Memory<Cuda> ;
  
        /** Default constructor.
         */
        Cuda() = default ;
  
        /** Default Deconstructor.
         */
        ~Cuda() = default ;
  
        /** Method to copy data from the host ( RAM ) to the GPU ( VRAM ).
         * @param src The source data on the host.
         * @param dst The memory handle to write to on the GPU
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyToDevice( const void* src, Memory& dst, Cuda::Device& gpu, unsigned amt ) ;
  
        /** Method to copy data from the GPU ( VRAM ) to the host ( RAM ).
         * @param src The source memory handle on the GPU.
         * @param dst The pointer to write to on the host.
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyToHost( const Memory& src, Data dst, Cuda::Device& gpu, unsigned amt ) ;
  
        /** Method to release the input memory handle.
         * @param mem The memory object to release.
         * @param gpu The device the memory object was allocated on.
         */
        void free( Memory& mem, Cuda::Device& gpu ) ;
  
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( unsigned size, const Cuda::Device& gpu ) ;
        
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @param mem_flags The memory property flags to use for creation of this memory object.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( unsigned size, const Cuda::Device& gpu, Cuda::MemoryFlags mem_flags ) ;
    };
  }
  
  /** Aliases for parent types.
   */
  template<typename TYPE>
  using CudaArray  = kgl::Array <kgl::cg::Cuda, TYPE> ;
  using CudaMemory = kgl::Memory<kgl::cg::Cuda      > ;
}

#endif

