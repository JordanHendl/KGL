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

#pragma once

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
#include "Descriptor.h"
#include "Renderer.h"
#include "Chain.h"

typedef unsigned VkFlags              ;
typedef VkFlags  VkImageUsageFlags    ;
typedef VkFlags  VkShaderStageFlags   ;
typedef VkFlags  VkPipelineStageFlags ;

/** Forward declared vulkan-specific objects.
 */
namespace vk
{
       template <typename BitType>
       class Flags ;
       
       class SurfaceKHR                                    ;
       class DeviceMemory                                  ;
       class Instance                                      ;
       class AttachmentDescription                         ;
  enum class ImageLayout                                   ;
  enum class MemoryPropertyFlagBits : VkFlags              ;
  enum class ImageUsageFlagBits     : VkImageUsageFlags    ;
  enum class ShaderStageFlagBits    : VkShaderStageFlags   ;
  enum class PipelineStageFlagBits  : VkPipelineStageFlags ;
  enum class ImageType                                     ;
  enum class Format                                        ;
  enum class Result                                        ;
  
  using ImageUsageFlags    = Flags<ImageUsageFlagBits >   ;
  using ShaderStageFlags   = Flags<ShaderStageFlagBits>   ;
  using PipelineStageFlags = Flags<PipelineStageFlagBits> ;
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
  
  /** Forward declared enum for a pipeline stage.
   */
  enum class PipelineStage : unsigned ;

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
  template<typename Framework>
  class Image ;
  
  /** Forward declared generic window object.
   */
  template<typename OS, typename Framework>
  class BaseWindow ;

  /** Forward declared SDL window
   */
  namespace sdl
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
      
        /** Wrapper class for a vulkan device memory.
     */
    class Memory
    {
      public:
        
        /** Default constructor.
         */
        Memory() ;
        
        /** Default deconstructor.
         */
        ~Memory() ;

        /** Copy constructor. Copies value into this.
         * @param val The value to copy to this object.
         */
        Memory( vk::DeviceMemory& val ) ;
        
        /** Assignment operator. Assigns the input into this object
         * @param val The value to assign this object to.
         * @return Reference to this object after assignment.
         */
        Memory& operator=( vk::DeviceMemory& val ) ;
        
        /** Assignment operator. Assigns the input into this object
         * @param val The value to assign this object to.
         * @return Reference to this object after assignment.
         */
        Memory& operator=( void* val ) ;
        
        /** Conversion operator for a boolean value.
         * @return Whether or not this object has a address stored.
         */
        operator bool() const ;

        /** Conversion operator of this object to a vulkan device memory handle.
         * @return The vulkan device memory handle representation of this object.
         */
        operator vk::DeviceMemory() ;
        
        /** Conversion operator of this object to a vulkan device memory handle.
         * @return The vulkan device memory handle representation of this object.
         */
        operator vk::DeviceMemory() const ;
        
      private:
        
        /** The underlying handle for this object's data.
         */
        void* val ;
    };
    
    class Surface
    {
      public:
        Surface() ;
        Surface( const Surface& surface ) ;
        ~Surface() ;
        Surface& operator=( const Surface& surface ) ;
        const vk::SurfaceKHR& surface() const ;
      private: 
        friend class vkg::Vulkan ;
        struct SurfaceData* surface_data ;
        SurfaceData& data() ;
        const SurfaceData& data() const ;
    };
    
    /** Class that implements Vulkan functionality.
     */
    class Vulkan
    {
      public:
        using Buffer          = nyx::vkg::Buffer             ; ///< The object to handle vulkan buffer creation.
        using CommandRecord   = nyx::vkg::CommandBuffer      ; ///< The object to handle recording of vulkan commands.
        using Context         = nyx::vkg::Surface            ; ///< The object to handle a window's context.
        using Descriptor      = nyx::vkg::Descriptor         ; ///< The object to manage data access in shaders.
        using DescriptorPool  = nyx::vkg::DescriptorPool     ; ///< The object to manage creating Descriptors.
        using Device          = nyx::vkg::Device             ; ///< The object to manage a hardware-accelerated device.
        using DeviceAddress   = unsigned long long           ; ///< The type of device address this library uses.
        using RenderPass      = nyx::vkg::RenderPass         ; ///< The object to manage a render pass.
        using Instance        = nyx::vkg::Instance           ; ///< The object to manage vulkan instance creation.
        using Texture         = nyx::vkg::Image              ; ///< The object to handle all image creation/memory management.
        using Memory          = nyx::vkg::Memory             ; ///< The Framework-specific handle for Device Memory.
        using Pipeline        = nyx::vkg::Pipeline           ; ///< The object to manage pipeline creation & handling.
        using Queue           = nyx::vkg::Queue              ; ///< The object to manage vulkan queues.
        using Shader          = nyx::vkg::NyxShader          ; ///< The object to manage an individual vulkan shader.
        using Swapchain       = nyx::vkg::Swapchain          ; ///< The object to manage a window's framebuffers.
        using Synchronization = nyx::vkg::Synchronization    ; ///< The object used to manage synchronization in this library.
        using Renderer        = nyx::vkg::Renderer           ;
        using Image           = nyx::Image<nyx::vkg::Vulkan> ;
        using Chain           = nyx::vkg::Chain              ;

        template<typename Type>
        using Array  = nyx::Array <nyx::vkg::Vulkan, Type> ;
    
        
        /** Reflective enumeration for a library error severity.
         */
        class Severity
        {
          public:

            /** The type of error.
             */
            enum
            {
              None,
              Info,
              Warning,
              Fatal,
            };

            /** Default constructor.
             */
            Severity() ;

            /** Copy constructor. Copies the input.
             * @param severity The input to copy.
             */
            Severity( const Severity& severity ) ;

            /** Copy constructor. Copies the input.
             * @param error The input to copy.
             */
            Severity( unsigned error ) ;

            /** Method to convert this error into a string.
             * @return The C-string representation of this severity.
             */
            const char* toString() const ;

            /** Method to retrieve the error associated with this object.
             * @return The severity associated with this object.
             */
            unsigned severity() const ;

            /** Conversion operator of this object to an unsigned integer representing the error.
             * @return The severity of this object.
             */
            operator unsigned() const ;

          private:

            /** Underlying variable holding the severity of this object.
             */
            unsigned sev ; 
        };

        /** Reflective enumeration for a library error.
         */
        class Error
        {
          public:

            /** The type of error.
             */
            enum
            {
              None,                  ///< No Error.
              Success,               ///< Success. Nothing to do.
              DeviceNotFound,        ///< The requested device is not found.
              Not_Ready,             ///< Device is not yet ready to do the operation.
              Incomplete,            ///< The operation was incomplete.
              OutOfHostMemory,       ///< Host memory has been depleted.
              OutOfDeviceMemory,     ///< Device memory has been depleted.
              DeviceLost,            ///< Device has been lost aka shits fucked up.
              FeatureNotPresent,     ///< The feature is not present in this system.
              ExtensionNotPresent,   ///< The requested extension is not present in this system ( should be fine though ).
              LayerNotPresent,       ///< The requested validation layer is not present in this system.
              Unknown,               ///< Unknown error.
              Fragmentation,         ///< TODO
              InvalidExternalHandle, ///< TODO
              InitializationFailed,  ///< TODO
              IncompatibleSurface,   ///< TODO
              SurfaceLost,           ///< TODO
              NativeWindowInUse,     ///< TODO
              RecreateSwapchain,     ///< An error to represent a swapchain should be recreated.
              SuboptimalKHR,         ///< TODO
              OutOfDataKHR,
              MemoryMapFailed,       ///< TODO
              ValidationFailed,      ///< TODO
              InvalidDevice,         ///< TODO
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
            
            /** Method to retrieve the severity of this error.
             * @return Method to retrieve the severity of this error.
             */
            Vulkan::Severity severity() const ;

            /** Method to convert this error into a string.
             * @return The C-string representation of this error.
             */
            const char* toString() const ;

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

        /** Abstract class for an error handler.
         * If you wish to have an object handle errors, inherit this class and implement the handleError() function.
         */
        class ErrorHandler
        {
          public:

            /** Virtual method to handle a library error.
             * @param error The error to handle.
             */
            virtual void handleError( nyx::vkg::Vulkan::Error error ) = 0 ;

            /** Virtual deconstructor for inheritance.
             */
            virtual ~ErrorHandler() ;
        };
      
        /** Static method to push an error onto this library.
         * @param error The error to handle by this library
         */
        static void add( Vulkan::Error error ) ;
        
        /** Static method to push an error onto this library.
         * @param error The error to handle by this library
         */
        static void add( vk::Result error ) ;
        
        /** Method to set the name of this application.
         * @param application_name The name of this application.
         */
        static void setApplicationName( const char* application_name ) ;
        
        /** Method to check whether the device is available on this system.
         * @param id The id of device to check.
         * @return Whether or not the device exists on the system.
         */
        static bool hasDevice( unsigned id ) ;

        /** Static method to add a validation layer to this vulkan library's runtime.
         * @param layer_name The name to associate with the layer.
         */
        static void addValidationLayer( const char* layer_name ) ;
        
        /** Static method to add an extension to this library's instance, if the extension is available.
         * @param extension The extension name to load.
         */
        static void addInstanceExtension( const char* extension ) ;
        
        /** Static method to add a device extension to this library. 
         * @param extension The extension to enable, if applicable.
         * @param idx The id of device to apply the extension to. By default applies to all devices.
         */
        static void addDeviceExtension( const char* extension, unsigned idx = 100 ) ;
        
        /** Method to synchronize with all gpu operation on the input device.
         * @param gpu The device to wait for all operations to complete on.
         */
        static void deviceSynchronize( unsigned gpu = 0 ) ;

        /** Static method to retrieve a graphics queue from this library.
         * @param gpu The gpu to generate the queue on.
         * @return A Queue capable of doing graphics.
         */
        static vkg::Queue graphicsQueue( unsigned gpu = 0 ) ;
        
        /** Static method to retrieve a graphics queue from this library.
         * @param gpu The gpu to generate the queue on.
         * @return A Queue capable of doing graphics.
         */
        static vkg::Queue computeQueue( unsigned gpu = 0 ) ;

        /** Static method to retrieve a graphics queue from this library.
         * @param gpu The gpu to generate the queue on.
         * @return A Queue capable of doing graphics.
         */
        static vkg::Queue presentQueue( unsigned window_id, unsigned gpu = 0 ) ;

        /** Static method to allow a custom error handler to be set for this library.
         * @param error_handler The error handler to be used by this library.
         */
        static void setErrorHandler( void ( *error_handler )( Vulkan::Error ) ) ;
        
        /** Static method to allow a custom error handler to be set for this library.
         * @param handler The error handler to be used by this library.
         */
        static void setErrorHandler( Vulkan::ErrorHandler* handler ) ;

        /** Static method to check if this library is initialized of not.
         * @return Whether or not this library is initialized.
         */
        static bool initialized() ;

        /** Static method to initialize this implementation with a vulkan instance.
         */
        static void initialize() ;
        
        /** Method to retrieve the platform-specific instance extension names for the surface of this system.
         * @return String names of the platform-specific extensions needed by this system for a vulkan surface.
         */
        static const char* platformSurfaceInstanceExtensions() ;
        
        /** Method to add a window to the implementation.
         * @param id The ID to associate with the window. If a conflict occurs, nothing happens.
         * @param title The title to associate with the window.
         * @param width The width of the window in pixels.
         * @param height The height of the window in pixels.
         */
        static void addWindow( unsigned id, const char* title, unsigned width, unsigned height ) ;
        
        /** Method to handle a window's events.
         *  For seeing how to recieve window events, use @EventManager.
         * @param id The id of window to publish events of.
         */
        static void handleWindowEvents( unsigned id ) ;
        
        /** Method to retrieve whether a window of the input ID exists.
         * @param id The id associated with the window.
         * @return Whether or not a window with the specified ID exists.
         */
        static bool hasWindow( unsigned id ) ;
        
        /** Method to set the title of a window.
         * @param id The id associated with the window.
         * @param title The C-String containing the title to set the window to.
         */
        static void setWindowTitle( unsigned id, const char* title ) ;
        
        /** Method to set whether a window is resizable or not.
         * @param id The id of the window to set.
         * @param value Whether it is resizable or not.
         */
        static void setWindowResizable( unsigned id, bool value ) ;

        /** Method to set whether to capture the mouse on the window.
         * @param id The window id.
         * @param value Whether or not to capture the mouse.
         */
        static void setWindowMouseCapture( unsigned id, bool value ) ;

        /** Method to set the width of a window.
         * @param id The id associated with the window.
         * @param width The width of the window in pixels.
         */
        static void setWindowWidth( unsigned id, unsigned width ) ;
        
        /** Method to set the height of a window.
         * @param id The id associated with the window.
         * @param height The height of the window in pixels.
         */
        static void setWindowHeight( unsigned id, unsigned height ) ;
        
        /** Method to set whether a window is borderless or now.
         * @param id The id associated with the window.
         * @param value Whether or not the window should be borderless.
         */
        static void setWindowBorderless( unsigned id, bool value ) ;
        
      private:
        
        /** Method to retrieve an implementation context from a window id.
         * @param id The id of window to retrieve the context of.
         * @return The vulkan surface handle for the given window.
         */
        static vkg::Surface context( unsigned id ) ;

        /** Typedef to avoid using void* directly.
         */
        typedef void* Data ;

        /** Friend declaration of memory object to use this implementation for.
         */
        template<typename OS, typename Framework>
        friend class nyx::BaseWindow ;
        
        friend class nyx::Memory<Vulkan>         ;
        friend class nyx::vkg::Buffer            ;
        friend class nyx::vkg::CommandBuffer     ;
        friend class nyx::vkg::CommandBufferData ;
        friend class nyx::vkg::Descriptor        ;
        friend class nyx::vkg::DescriptorPool    ;
        friend class nyx::vkg::Device            ;
        friend class nyx::vkg::DeviceData        ;
        friend class nyx::vkg::RenderPass        ;
        friend class nyx::vkg::RenderPassData    ;
        friend class nyx::vkg::Instance          ;
        friend class nyx::vkg::Image             ;
        friend class nyx::vkg::Pipeline          ;
        friend class nyx::vkg::Queue             ;
        friend class nyx::vkg::NyxShader         ;
        friend class nyx::vkg::Swapchain         ;
        friend class nyx::vkg::SwapchainData     ;
        friend class nyx::vkg::Synchronization   ;
        friend class nyx::vkg::RendererImpl      ;
        friend class nyx::vkg::Chain             ;
        
        /** Default constructor.
         */
        Vulkan() = default ;
  
        /** Default Deconstructor.
         */
        ~Vulkan() = default ;

        /** Static method for retrieving a vulkan surface from a sdl window.
         * @param window The SDL window to get a surface from.
         * @return A Valid vulkan surface.
         */
        static Vulkan::Context contextFromBaseWindow( const nyx::sdl::Window& window ) ;

        /** Static method to convert a library format to the implementation-specific format.
         * @param stage The library stage to convert.
         * @return The vulkan-library specific version.
         */
        static vk::ShaderStageFlags convert( nyx::PipelineStage stage ) ;
        
        /** Static method to convert a library format to the implementation-specific format.
         * @param stage The library stage to convert.
         * @return The vulkan-library specific version.
         */
        static vk::PipelineStageFlags convert( nyx::GPUStages stage ) ;
        
        /** Static method to convert a library attachment to a vulkan attachment.
         * @param attachment The attachment to convert.
         * @return The converted attachment.
         */
        static vk::AttachmentDescription convert( const nyx::Attachment& attachment ) ;
        
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

        /** Method to copy data from the host ( RAM ) to the GPU ( VRAM ).
         * @param src The source data on the host.
         * @param dst The memory handle to write to on the GPU
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyToDevice( const void* src, Memory& dst, unsigned gpu, unsigned amt, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
  
        /** Method to copy data from the GPU ( VRAM ) to the host ( RAM ).
         * @param src The source memory handle on the GPU.
         * @param dst The pointer to write to on the host.
         * @param gpu The device to use for this operation.
         * @param amt The amount of data to copy.
         */
        void copyToHost( const Memory& src, Data dst, unsigned gpu, unsigned amt, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Static method to retrieve a device from this object.
         * @param id The id of device to grab.
         * @return A const-reference to the device object.
         */
        static const vkg::Device& device( unsigned id = 0 ) ;

        /** Method to release the input memory handle.
         * @param mem The memory object to release.
         * @param gpu The device the memory object was allocated on.
         */
        void free( Memory& mem, unsigned gpu ) ;
        
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( unsigned gpu, unsigned size, unsigned filter = 0xFFFFFFF ) ;
        
        /** Method to create & allocate memory on the GPU.
         * @param size The size of the memory to allocate.
         * @param gpu The GPU to allocate data on.
         * @param mem_flags The memory property flags to use for creation of this memory object.
         * @return Allocated memory on the GPU.
         */
        Memory createMemory( unsigned gpu, unsigned size, nyx::MemoryFlags mem_flags, unsigned filter = 0xFFFFFFF ) ;
        
        /** Static method to retrieve the instance of this library.
         * @return The instance associated with this library.
         */
        static const vkg::Instance& instance() ; 
    };
  }
}


