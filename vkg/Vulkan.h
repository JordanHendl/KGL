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

#ifndef KGL_VULKAN_H
#define KGL_VULKAN_H

/** Forward declared vulkan-specific objects.
 */
namespace vk
{
       template <typename BitType, typename MaskType>
       class Flags ;

       class Buffer                 ;
       class DeviceMemory           ;
       class CommandBuffer          ;
       class Instance               ;
  enum class MemoryPropertyFlagBits ;
}

/** Operator definition for OR'ing a memory property flag bit and an unsigned integer.
 */
unsigned operator|( unsigned first, vk::MemoryPropertyFlagBits second ) ;

namespace kgl
{
  
  /** Forward declared Memory object for friendship.
   */
  template<typename IMPL>
  class Memory ;
  
  template<typename IMPL, typename TYPE>
  class Array ;

  namespace vkg
  {
    /** Forward declared device declaration.
     */
    class Device ;
    
    /** Forward declared buffer declaration.
     */
    class Buffer ;

    /** Class that implements Vulkan functionality.
     */
    class Vulkan
    {
      public:
        using Device         = ::kgl::vkg::Device                                  ;
        using Buffer         = ::kgl::vkg::Buffer                                  ;
        using Memory         = ::vk::DeviceMemory                                  ;
        using CommandRecord  = ::vk::CommandBuffer                                 ;
        using MemoryPropFlag = ::vk::Flags<::vk::MemoryPropertyFlagBits, unsigned> ;

        /** Static method to initialize this implementation with a vulkan instance.
         * @param instance
         */
        static void initialize( const ::vk::Instance& instance ) ;
        
        /** Static method to convert a vulkan error to an error of the library.
         * @param error An error defined by vulkan.
         * @return An error defined by the library.
         */
        static unsigned convertError( unsigned error ) ;

      private:
        
        /** Class for capturing a vulkan memory property flag.
         */
        class MemoryFlags
        {
          public:
            
            /** Default constructor.
             */
            MemoryFlags() ;
            
            /** Constructor from an unsigned integer.
             */
            MemoryFlags( unsigned flags ) ;
            
            /** Constructor from an unsigned integer.
             */
            MemoryFlags( ::vk::MemoryPropertyFlagBits flags ) ;
            
            /** Constructor from an unsigned integer.
             */
            MemoryFlags( MemoryPropFlag flags ) ;
            
            /** Method to retrieve the value of this object.
             */
            MemoryPropFlag val() ;
            
            /** Method to assign this object to a flag.
             */
            MemoryFlags& operator=( unsigned flags ) ;
            
            /** Method to assign this object to a flag.
             */
            MemoryFlags& operator=( MemoryPropFlag flags ) ;

          private:

            /** The internal Vulkan Memory Property Flag represented as an unsigned integer.
             */
            unsigned flag ;
        };
        
        /** Typedef to avoid using void* directly.
         */
        typedef void* Data ;
        
        /** Friend declaration of memory object to use this implementation for.
         */
        friend class ::kgl::Memory<Vulkan> ;
  
        /** Default constructor.
         */
        Vulkan() = default ;
  
        /** Default Deconstructor.
         */
        ~Vulkan() = default ;
  
        /** Method to copy data from the host ( RAM ) to the GPU ( VRAM ).
         * @param src The source data on the host.
         * @param dst The memory handle to write to on the GPU
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyTo( const void* src, Memory& dst, Vulkan::Device& gpu, unsigned amt ) ;
  
        /** Method to copy data from the GPU ( VRAM ) to the host ( RAM ).
         * @param src The source memory handle on the GPU.
         * @param dst The pointer to write to on the host.
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyTo( const Memory& src, Data dst, Vulkan::Device& gpu, unsigned amt ) ;
  
        /** Method to release the input memory handle.
         * @param mem The memory object to release.
         * @param gpu The device the memory object was allocated on.
         */
        void free( Memory& mem, Vulkan::Device& gpu ) ;
  
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( unsigned size, const Vulkan::Device& gpu ) ;
        
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @param mem_flags The memory property flags to use for creation of this memory object.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( unsigned size, const Vulkan::Device& gpu, Vulkan::MemoryFlags mem_flags ) ;
    };
  }
  
  /** Aliases for parent types.
   */
  template<typename TYPE>
  using VkArray  = kgl::Array <kgl::vkg::Vulkan, TYPE> ;
  using VkMemory = kgl::Memory<kgl::vkg::Vulkan      > ;
}

#endif

