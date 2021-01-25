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

#ifndef NYX_VULKAN_H
#define NYX_VULKAN_H

typedef unsigned VkFlags ;

/** Forward declared vulkan-specific objects.
 */
namespace vk
{
       template <typename BitType>
       class Flags ;
       
       class SurfaceKHR                       ;
       class Buffer                           ;
       class DeviceMemory                     ;
       class CommandBuffer                    ;
       class Instance                         ;
  enum class MemoryPropertyFlagBits : VkFlags ;
  enum class ImageLayout                      ;
  enum class Format                           ;
  enum class Result                           ;
}

/** Operator definition for OR'ing a memory property flag bit and an unsigned integer.
 */
unsigned operator|( unsigned first, vk::MemoryPropertyFlagBits second ) ;

namespace nyx
{
  
  /** Forward declared Memory object for friendship.
   */
  template<typename IMPL>
  class Memory ;
  
  /** Forward declared generic Array object.
   */
  template<typename IMPL, typename TYPE>
  class Array ;
  
  /** Forward declared generic image object.
   */
  template<typename IMPL, typename TYPE>
  
  class Image ;
  
  /** Forward declared Linux window
   */
  namespace lx
  {
    class Window ;
  }
  
  /** Forward declared Win32 window.
   */
  namespace win32
  {
    class Window ;
  }
  
  namespace vkg
  {
    /** Forward declared device declaration.
     */
    class Device ;
    
    /** Forward declared buffer declaration.
     */
    class Buffer ;
    
    /** Forward declared image declaration.
     */
    class Image ;
    
    /** Reflective enumeration for a library error.
     */
    class Error
    {
      public:
        
        /** The type of error.
         */
        enum
        {
          None,
          Success,
          Not_Ready,
          Incomplete,
          OutOfHostMemory,
          DeviceLost,
          ExtensionNotPresent,
          LayerNotPresent,
          Unknown,
          SuboptimalSwapchain,
          Fragmentation,
          InvalidExternalHandle,
          SurfaceLost,
          NativeWindowInUse,
          SuboptimalKHR,
          MemoryMapFailed,
          ValidationFailed,
          InvalidDevice,
        };
        
        /** Default constructor.
         */
        Error() ;
        
        /** Copy constructor. Copies the input.
         * @param error The input to copy.
         */
        Error( const Error& error ) ;
        
        /** Copy constructor. Copies the input.
         * @param error The input to copy.
         */
        Error( unsigned error ) ;
        
        /** Method to retrieve the error associated with this object.
         * @return The error associated with this object.
         */
        unsigned error() const ;
        
        /** Conversion operator of this object to an unsigned integer representing the error.
         * @return The error of this object.
         */
        operator unsigned() const ;

      private:
        
        /** Underlying variable holding the error of this object.
         */
        unsigned err ; 
    };

    /** Class that implements Vulkan functionality.
     */
    class Vulkan
    {
      public:
        using Device         = ::nyx::vkg::Device                        ; ///< TODO
        using Buffer         = ::nyx::vkg::Buffer                        ; ///< TODO
        using Image          = ::nyx::vkg::Image                         ; ///< TODO
        using Memory         = ::vk::DeviceMemory                        ; ///< TODO
        using CommandRecord  = ::vk::CommandBuffer                       ; ///< TODO
        using MemoryPropFlag = ::vk::Flags<::vk::MemoryPropertyFlagBits> ; ///< TODO
        using ImageLayout    = ::vk::ImageLayout                         ; ///< TODO
        using ImageFormat    = ::vk::Format                              ; ///< TODO
        using Context        = ::vk::SurfaceKHR                          ; ///< TODO

        /** Static method to initialize this implementation with a vulkan instance.
         * @param instance
         */
        static void initialize( const ::vk::Instance& instance ) ;
        
        /** Static method to convert a vulkan error to an error of the library.
         * @param error An error defined by vulkan.
         * @return An error defined by the library.
         */
        static Error convertError( vk::Result error ) ;

        /** Static method for retrieving a vulkan surface from a window's window.
         * @param window The Win32 window to get a surface from.
         * @return A Valid vulkan surface.
         */
        static Vulkan::Context contextFromBaseWindow( const nyx::win32::Window& window ) ;
        
        /** Static method for retrieving a vulkan surface from a linux window.
         * @param window The Linux window to get a surface from.
         * @return A Valid vulkan surface.
         */
        static Vulkan::Context contextFromBaseWindow( const nyx::lx::Window& window ) ;

        /** Method to retrieve the platform-specific instance extension names for the surface of this system.
         * @return String names of the platform-specific extensions needed by this system for a vulkan surface.
         */
        static const char* platformSurfaceInstanceExtensions() ;
        

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
        friend class ::nyx::Memory<Vulkan> ;
  
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
        void copyToDevice( const void* src, Memory& dst, Vulkan::Device& gpu, unsigned amt, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
  
        /** Method to copy data from the GPU ( VRAM ) to the host ( RAM ).
         * @param src The source memory handle on the GPU.
         * @param dst The pointer to write to on the host.
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyToHost( const Memory& src, Data dst, Vulkan::Device& gpu, unsigned amt, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
  
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
        Memory createMemory( const Vulkan::Device& gpu, unsigned size, unsigned filter = 0xFFFFFFF ) ;
        
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @param mem_flags The memory property flags to use for creation of this memory object.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( const Vulkan::Device& gpu, unsigned size, Vulkan::MemoryFlags mem_flags, unsigned filter = 0xFFFFFFF ) ;
    };

    /** Aliases for parent types.
     */
    template<typename TYPE>
    using VkArray  = nyx::Array <nyx::vkg::Vulkan, TYPE> ;

    template<typename TYPE>
    using VkImage = nyx::Image<nyx::vkg::Vulkan, TYPE> ;

    using VkMemory     = nyx::Memory<nyx::vkg::Vulkan              > ;
    using FloatVkImage = nyx::Image<nyx::vkg::Vulkan, float        > ;
    using CharVkImage  = nyx::Image<nyx::vkg::Vulkan, unsigned char> ;
  }
}

#endif

