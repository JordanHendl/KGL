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

#include "Device.h"
#include "Instance.h"
#include "Buffer.h"
#include "Image.h"
#include "Queue.h"
#include "CommandBuffer.h"
#include "Synchronization.h"
#include "NyxShader.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "Swapchain.h"

typedef unsigned VkFlags            ;
typedef VkFlags  VkImageUsageFlags  ;
typedef VkFlags  VkShaderStageFlags ;

/** Forward declared vulkan-specific objects.
 */
namespace vk
{
       template <typename BitType>
       class Flags ;
       
       class SurfaceKHR                                  ;
       class DeviceMemory                                ;
       class Instance                                    ;
  enum class ImageLayout                                 ;
  enum class MemoryPropertyFlagBits : VkFlags            ;
  enum class ImageUsageFlagBits     : VkImageUsageFlags  ;
  enum class ShaderStageFlagBits    : VkShaderStageFlags ;
  enum class ImageType                                   ;
  enum class Format                                      ;
  enum class Result                                      ;
  
  using ImageUsageFlags  = Flags<ImageUsageFlagBits > ;
  using ShaderStageFlags = Flags<ShaderStageFlagBits> ;
}

/** Operator definition for OR'ing a memory property flag bit and an unsigned integer.
 */
unsigned operator|( unsigned first, vk::MemoryPropertyFlagBits second ) ;

namespace nyx
{
  /** Forward declared reflective enumeration.
   */
  class MemoryFlags ;

  /** Forward declared enum for image format.
   */
  enum class ImageFormat : unsigned ;
  
  /** Forward decalred enum for image layout.
   */
  enum class ImageLayout : unsigned ;

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
  template<typename IMPL, ImageFormat FORMAT>
  
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
  
    enum class PipelineStage
  {
    Vertex,
    Fragment,
    Compute,
    TessC,
  };
  
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
        using Buffer          = nyx::vkg::Buffer                        ; ///< The object to handle vulkan buffer creation.
        using CommandRecord   = nyx::vkg::CommandBuffer                 ; ///< The object to handle recording of vulkan commands.
        using Context         = vk::SurfaceKHR                          ; ///< The object to handle a window's context.
        using Device          = nyx::vkg::Device                        ; ///< The object to manage a hardware-accelerated device.
        using DeviceAddress   = unsigned long long                      ; ///< The type of device address this library uses.
        using RenderPass      = nyx::vkg::RenderPass                    ; ///< The object to manage a render pass.
        using Instance        = nyx::vkg::Instance                      ; ///< The object to manage vulkan instance creation.
        using Texture         = nyx::vkg::Image                         ; ///< The object to handle all image creation/memory management.
        using Memory          = vk::DeviceMemory                        ; ///< The Framework-specific handle for Device Memory.
        using Pipeline        = nyx::vkg::Pipeline                      ; ///< The object to manage pipeline creation & handling.
        using Queue           = nyx::vkg::Queue                         ; ///< The object to manage vulkan queues.
        using Shader          = nyx::vkg::NyxShader                     ; ///< The object to manage an individual vulkan shader.
        using Swapchain       = nyx::vkg::Swapchain                     ; ///< The object to manage a window's framebuffers.
        using Synchronization = nyx::vkg::Synchronization               ; ///< The object used to manage synchronization in this library.

        template<typename TYPE>
        using Array  = nyx::Array <nyx::vkg::Vulkan, TYPE> ;
    
        template<nyx::ImageFormat FORMAT>
        using Image = nyx::Image<nyx::vkg::Vulkan, FORMAT> ;
        
        /** Static method to convert a library format to the implementation-specific format.
         * @param stage The library stage to convert.
         * @return The vulkan-library specific version.
         */
        static vk::ShaderStageFlags convert( nyx::PipelineStage stage ) ;

        /** Static method to convert a library format to the implementation-specific format.
         * @param format The library format to convert.
         * @return The implementation-specific format.
         */
        static vk::Format convert( nyx::ImageFormat format ) ;
        
        /** Static method to convert a library format to the implementation-specific format.
         * @param format The library format to convert.
         * @return The implementation-specific format.
         */
        static nyx::ImageFormat convert( vk::Format format ) ;

        /** Static method to convert a vulkan image layout to an implementation layout.
         * @param layout The layout to convert.
         * @return The implementation-specific layout converted.
         */
        static nyx::ImageLayout convert( vk::ImageLayout layout ) ;
        
        /** Static method to convert an implementation image layout to a Vulkan one.
         * @param layout The layout to convert.
         * @return The converted layout.
         */
        static vk::ImageLayout convert( nyx::ImageLayout layout ) ;
        
        /** Static method to convert a vulkan image layout to an implementation layout.
         * @param layout The layout to convert.
         * @return The implementation-specific layout converted.
         */
        static nyx::ImageUsage convert( vk::ImageUsageFlagBits usage ) ;
        
        /** Static method to convert an implementation image layout to a Vulkan one.
         * @param layout The layout to convert.
         * @return The converted layout.
         */
        static vk::ImageUsageFlagBits convert( nyx::ImageUsage usage ) ;
        
        /** Static method to convert a vulkan image layout to an implementation layout.
         * @param layout The layout to convert.
         * @return The implementation-specific layout converted.
         */
        static nyx::ImageType convert( vk::ImageType layout ) ;
        
        /** Static method to convert an implementation image layout to a Vulkan one.
         * @param layout The layout to convert.
         * @return The converted layout.
         */
        static vk::ImageType convert( nyx::ImageType layout ) ;

        /** Static method to convert a vulkan error to an error of the library.
         * @param error An error defined by vulkan.
         * @return An error defined by the library.
         */
        static Error convert( vk::Result error ) ;

        /** Static method to initialize this implementation with a vulkan instance.
         * @param instance
         */
        static void initialize( const ::vk::Instance& instance ) ;
        
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
        Memory createMemory( const Vulkan::Device& gpu, unsigned size, nyx::MemoryFlags mem_flags, unsigned filter = 0xFFFFFFF ) ;
    };
  }
}

#endif

