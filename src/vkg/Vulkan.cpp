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

#include "Vulkan.h"
#include "Device.h"
#include "library/Image.h"
#include <algorithm>
#include <iostream>
#include <library/Memory.h>

#define VULKAN_HPP_NO_EXCEPTIONS

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <win32/Window.h>
#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR
#include <linux/Window.h>
#endif
#include <vulkan/vulkan.hpp>


unsigned operator|( unsigned first, vk::MemoryPropertyFlagBits second )
{
  return first | static_cast<unsigned>( second ) ;
}

 namespace nyx
 {
   namespace vkg
   {
     
    #if defined ( __unix__ ) || defined( _WIN32 )
      constexpr const char* END_COLOR    = "\x1B[m"     ;
      constexpr const char* COLOR_RED    = "\u001b[31m" ;
      constexpr const char* COLOR_GREEN  = "\u001b[32m" ;
      constexpr const char* COLOR_YELLOW = "\u001b[33m" ;
      constexpr const char* COLOR_GREY   = "\x1B[1;30m" ;
      constexpr const char* UNDERLINE    = "\u001b[4m"  ;
    #else
      constexpr const char* END_COLOR    = "" ;
      constexpr const char* COLOR_GREEN  = "" ;
      constexpr const char* COLOR_YELLOW = "" ;
      constexpr const char* COLOR_GREY   = "" ;
      constexpr const char* COLOR_RED    = "" ;
      constexpr const char* COLOR_WHITE  = "" ;
    #endif

     /** Static function for getting a color code from a vulkan severity.
      * @param severity The severity to determing the color of.
      * @return The color code of the severity.
      */
     static const char* colorFromSeverity( Vulkan::Severity severity ) ;

     /** Static function to find the appropriate memory given the input flags and filter.
      * @param filter The mask to filter results.
      * @param flag The memory flag to look for.
      * @param device The device to look for memory on.
      * @return The mask of memory type found.
      */
     static uint32_t memType( uint32_t filter, ::vk::MemoryPropertyFlags flag, ::vk::PhysicalDevice device ) ;
     
     /** Static function to act as a default memory handler.
      * @param error The error to handle.
      */
     static void defaultHandler( nyx::vkg::Vulkan::Error error ) ;

     /** The structure to contain all of the global vkg library data.
      */
     struct VulkanData
     {
       typedef void ( *Callback )( Vulkan::Error ) ;
       
       Callback              error_cb ;
       Vulkan::ErrorHandler* handler  ;
       vk::Instance          instance ;
       
       /** Default constructor.
        */
       VulkanData() ;
     };
     
     /** Static container for all of the vulkan library's global data.
      */
     static VulkanData data ;
     
     const char* colorFromSeverity( Vulkan::Severity severity )
     {
       switch ( severity )
       {
         case Vulkan::Severity::Info    : return vkg::COLOR_GREY   ;
         case Vulkan::Severity::Warning : return vkg::COLOR_YELLOW ;
         case Vulkan::Severity::Fatal   : return vkg::COLOR_RED    ;
         default : return vkg::COLOR_GREY ;
       }
     }

     uint32_t memType( uint32_t filter, ::vk::MemoryPropertyFlags flag, ::vk::PhysicalDevice device )
     {
       ::vk::PhysicalDeviceMemoryProperties mem_prop ;

       mem_prop = device.getMemoryProperties() ;
       for( unsigned index = 0; index < mem_prop.memoryTypeCount; index++ )
       {
         if( filter & ( 1 << index ) && ( mem_prop.memoryTypes[ index ].propertyFlags & flag ) == flag )
         {
           return index ;
         }
       }
       return 0 ;
     }

     void defaultHandler( nyx::vkg::Vulkan::Error error )
     {
       auto severity = error.severity() ;

       std::cout << colorFromSeverity( severity ) << "-- " << severity.toString() << " | " << "Nyx::vkg Error: " << error.toString() << "." << vkg::END_COLOR << std::endl ;
       if( severity == Vulkan::Severity::Fatal ) exit( -1 ) ;
     }

     VulkanData::VulkanData()
     {
       this->error_cb = &vkg::defaultHandler ;
       this->instance = nullptr ;
       this->handler  = nullptr ;
     }

     vkg::Vulkan::Severity::Severity()
     {
       this->sev = Severity::None ;
     }
     
     vkg::Vulkan::Severity::Severity( const vkg::Vulkan::Severity& severity )
     {
       this->sev = severity.sev ;
     }
     
     vkg::Vulkan::Severity::Severity( unsigned error )
     {
       this->sev = error ;
     }
     
     unsigned vkg::Vulkan::Severity::severity() const
     {
       return *this ;
     }
     
     vkg::Vulkan::Severity::operator unsigned() const
     {
       return this->sev ;
     }
     
     const char* vkg::Vulkan::Severity::toString() const
     {
       switch( this->sev )
       {
         case Severity::Fatal : return "Fatal" ;
         default : return "Unknown Severity" ;
       }
     }
     
     vkg::Vulkan::Error::Error()
     {
       this->err = Error::None ;
     }
     
     vkg::Vulkan::Error::Error( const vkg::Vulkan::Error& error )
     {
       this->err = error.err ;
     }
     
     vkg::Vulkan::Error::Error( unsigned error )
     {
       this->err = error ;
     }
     
     unsigned vkg::Vulkan::Error::error() const
     {
       return *this ;
     }
     
     vkg::Vulkan::Error::operator unsigned() const
     {
       return this->err ;
     }
     
     const char* vkg::Vulkan::Error::toString() const
     {
       switch( this->err )
       {
         case Error::DeviceLost : return "Device Lost" ;
         default : return "Unknown Error" ;
       }
     }
     
     Vulkan::Severity vkg::Vulkan::Error::severity() const
     {
       switch( this->err )
       {
         case Error::DeviceLost : return Severity::Fatal ;
         default : return Severity::None ;
       }
     }

     void Vulkan::add( nyx::vkg::Vulkan::Error error )
     {
       if( error != Vulkan::Error::Success )
       {
         if( data.error_cb != nullptr )
         {
           ( data.error_cb )( error ) ;
         }
         
         if( data.handler != nullptr )
         {
           data.handler->handleError( error ) ;
         }
       }
     }
     
     void Vulkan::add( vk::Result error )
     {
       Vulkan::add( Vulkan::convert( error ) ) ;
     }
     
     void Vulkan::setErrorHandler( void ( *error_handler )( nyx::vkg::Vulkan::Error ) )
     {
       data.error_cb = error_handler ;
     }
     
     void Vulkan::setErrorHandler( nyx::vkg::Vulkan::ErrorHandler* handler )
     {
       data.handler = handler ;
     }
        
     vk::ShaderStageFlags Vulkan::convert( nyx::PipelineStage stage )
     {
       switch( stage )
       {
         case nyx::PipelineStage::Vertex   : return vk::ShaderStageFlagBits::eVertex              ;
         case nyx::PipelineStage::Fragment : return vk::ShaderStageFlagBits::eFragment            ;
         case nyx::PipelineStage::Compute  : return vk::ShaderStageFlagBits::eCompute             ;
         case nyx::PipelineStage::TessC    : return vk::ShaderStageFlagBits::eTessellationControl ;
         default : return vk::ShaderStageFlagBits::eVertex ;
       }
     }
     
     vk::Format Vulkan::convert( nyx::ImageFormat format )
     {
       switch( format )
       {
         case nyx::ImageFormat::R8      : return vk::Format::eR8Srgb             ;
         case nyx::ImageFormat::RGB8    : return vk::Format::eR8G8B8Srgb         ;
         case nyx::ImageFormat::BGR8    : return vk::Format::eB8G8R8Srgb         ;
         case nyx::ImageFormat::RGBA8   : return vk::Format::eR8G8B8A8Srgb       ;
         case nyx::ImageFormat::BGRA8   : return vk::Format::eB8G8R8A8Srgb       ;
         case nyx::ImageFormat::R32I    : return vk::Format::eR32Sint            ;
         case nyx::ImageFormat::RGB32I  : return vk::Format::eR32G32B32Sint      ;
         case nyx::ImageFormat::RGBA32I : return vk::Format::eR32G32B32A32Sint   ;
         case nyx::ImageFormat::R32F    : return vk::Format::eR32Sfloat          ;
         case nyx::ImageFormat::RGB32F  : return vk::Format::eR32G32B32Sfloat    ;
         case nyx::ImageFormat::RGBA32F : return vk::Format::eR32G32B32A32Sfloat ;
         default : return vk::Format::eUndefined ;
       }
     }

     nyx::ImageFormat Vulkan::convert( vk::Format format )
     {
       switch( format )
       {
         case vk::Format::eR8Srgb             : return nyx::ImageFormat::R8      ;
         case vk::Format::eR8G8B8Srgb         : return nyx::ImageFormat::RGB8    ;
         case vk::Format::eB8G8R8Srgb         : return nyx::ImageFormat::BGR8    ;
         case vk::Format::eR8G8B8A8Srgb       : return nyx::ImageFormat::RGBA8   ;
         case vk::Format::eB8G8R8A8Srgb       : return nyx::ImageFormat::BGRA8   ;
         case vk::Format::eR32Sint            : return nyx::ImageFormat::R32I    ;
         case vk::Format::eR32G32B32Sint      : return nyx::ImageFormat::RGB32I  ;
         case vk::Format::eR32G32B32A32Sint   : return nyx::ImageFormat::RGBA32I ;
         case vk::Format::eR32Sfloat          : return nyx::ImageFormat::R32F    ;
         case vk::Format::eR32G32B32Sfloat    : return nyx::ImageFormat::RGB32F  ;
         case vk::Format::eR32G32B32A32Sfloat : return nyx::ImageFormat::RGBA32F ;
         default : return nyx::ImageFormat::RGB8 ;
       }
     }

     nyx::ImageLayout Vulkan::convert( vk::ImageLayout layout )
     {
       switch( layout )
       {
         case vk::ImageLayout::eUndefined               : return nyx::ImageLayout::Undefined       ;
         case vk::ImageLayout::eGeneral                 : return nyx::ImageLayout::General         ;
         case vk::ImageLayout::eColorAttachmentOptimal  : return nyx::ImageLayout::ColorAttachment ;
         case vk::ImageLayout::eShaderReadOnlyOptimal   : return nyx::ImageLayout::ShaderRead      ;
         case vk::ImageLayout::eTransferSrcOptimal      : return nyx::ImageLayout::TransferSrc     ;
         case vk::ImageLayout::eTransferDstOptimal      : return nyx::ImageLayout::TransferDst     ;
         case vk::ImageLayout::ePresentSrcKHR           : return nyx::ImageLayout::PresentSrc      ;
         case vk::ImageLayout::eDepthReadOnlyOptimalKHR : return nyx::ImageLayout::DepthRead       ;
         
         default : return nyx::ImageLayout::Undefined ;
       };
     }
     
     vk::ImageLayout Vulkan::convert( nyx::ImageLayout layout )
     {
       switch( layout )
       {
         case nyx::ImageLayout::Undefined       : return vk::ImageLayout::eUndefined               ;
         case nyx::ImageLayout::General         : return vk::ImageLayout::eGeneral                 ;
         case nyx::ImageLayout::ColorAttachment : return vk::ImageLayout::eColorAttachmentOptimal  ;
         case nyx::ImageLayout::ShaderRead      : return vk::ImageLayout::eShaderReadOnlyOptimal   ;
         case nyx::ImageLayout::TransferSrc     : return vk::ImageLayout::eTransferSrcOptimal      ;
         case nyx::ImageLayout::TransferDst     : return vk::ImageLayout::eTransferDstOptimal      ;
         case nyx::ImageLayout::PresentSrc      : return vk::ImageLayout::ePresentSrcKHR           ;
         case nyx::ImageLayout::DepthRead       : return vk::ImageLayout::eDepthReadOnlyOptimalKHR ;
         
         default : return vk::ImageLayout::eUndefined ;
       };
     }

     nyx::ImageUsage Vulkan::convert( vk::ImageUsageFlagBits usage )
     {
       switch( static_cast<vk::ImageUsageFlagBits>( usage ) )
       {
         case vk::ImageUsageFlagBits::eTransferSrc            : return nyx::ImageUsage::TransferSrc          ;
         case vk::ImageUsageFlagBits::eTransferDst            : return nyx::ImageUsage::TransferDst          ;
         case vk::ImageUsageFlagBits::eSampled                : return nyx::ImageUsage::Sampled              ;
         case vk::ImageUsageFlagBits::eStorage                : return nyx::ImageUsage::Storage              ;
         case vk::ImageUsageFlagBits::eColorAttachment        : return nyx::ImageUsage::ColorAttachment      ;
         case vk::ImageUsageFlagBits::eDepthStencilAttachment : return nyx::ImageUsage::DepthStencil         ;
         case vk::ImageUsageFlagBits::eInputAttachment        : return nyx::ImageUsage::Input                ;
         case vk::ImageUsageFlagBits::eShadingRateImageNV     : return nyx::ImageUsage::ShadingRate          ;
         case vk::ImageUsageFlagBits::eFragmentDensityMapEXT  : return nyx::ImageUsage::VKEXTFragmentDensity ;
         default: return nyx::ImageUsage::Input ;
       };
     }
     
     vk::ImageUsageFlagBits Vulkan::convert( nyx::ImageUsage usage )
     {
       switch( usage )
       {
         case nyx::ImageUsage::TransferSrc          : return vk::ImageUsageFlagBits::eTransferSrc            ;
         case nyx::ImageUsage::TransferDst          : return vk::ImageUsageFlagBits::eTransferDst            ;
         case nyx::ImageUsage::Sampled              : return vk::ImageUsageFlagBits::eSampled                ;
         case nyx::ImageUsage::Storage              : return vk::ImageUsageFlagBits::eStorage                ;
         case nyx::ImageUsage::ColorAttachment      : return vk::ImageUsageFlagBits::eColorAttachment        ;
         case nyx::ImageUsage::DepthStencil         : return vk::ImageUsageFlagBits::eDepthStencilAttachment ;
         case nyx::ImageUsage::Input                : return vk::ImageUsageFlagBits::eInputAttachment        ;
         case nyx::ImageUsage::ShadingRate          : return vk::ImageUsageFlagBits::eShadingRateImageNV     ;
         case nyx::ImageUsage::VKEXTFragmentDensity : return vk::ImageUsageFlagBits::eFragmentDensityMapEXT  ;
         default: return vk::ImageUsageFlagBits::eInputAttachment ;
       };
     }
     
     nyx::ImageType Vulkan::convert( vk::ImageType type )
     {
       switch( type )
       {
         case vk::ImageType::e1D : return nyx::ImageType::n1D ;
         case vk::ImageType::e2D : return nyx::ImageType::n2D ;
         case vk::ImageType::e3D : return nyx::ImageType::n3D ;
         default : return nyx::ImageType::n2D ;
       };
     }
     
     vk::ImageType Vulkan::convert( nyx::ImageType type )
     {
       switch( type )
       {
         case nyx::ImageType::n1D : return vk::ImageType::e1D ;
         case nyx::ImageType::n2D : return vk::ImageType::e2D ;
         case nyx::ImageType::n3D : return vk::ImageType::e3D ;
         default : return vk::ImageType::e2D ;
       }; 
     }
        
     void Vulkan::initialize( const vk::Instance& instance )
     {
       data.instance = instance ;
     }

     vkg::Vulkan::Error Vulkan::convert( vk::Result error )
     {
       switch( error )
       {
         case vk::Result::eErrorDeviceLost : return Vulkan::Error::DeviceLost ;
         case vk::Result::eSuccess         : return Vulkan::Error::Success    ;
         default : return Vulkan::Error::None ;
       }
     }

     void Vulkan::copyToDevice( const void* src, Vulkan::Memory& dst, Vulkan::Device& gpu, unsigned amt, unsigned src_offset, unsigned dst_offset )
     {
       const auto device = gpu.device() ;

       ::vk::DeviceSize     offset ;
       ::vk::DeviceSize     amount ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = dst_offset ;
       amount = amt        ;

       src    = static_cast<const void*>( reinterpret_cast<const unsigned char*>( src ) + src_offset ) ;

       auto result = ( device.mapMemory  ( dst, offset, amount, flag, &mem  ) ) ;
                     std::memcpy       ( mem, src, static_cast<size_t>( amt ) ) ;
                     device.unmapMemory( dst                                  ) ;
                     
       if( result != vk::Result::eSuccess )
       {
         std::cout << "Error: " << vk::to_string( result ) << "\n" ;
       }
     }
     
     void Vulkan::copyToHost( const Vulkan::Memory& src, Vulkan::Data dst, Vulkan::Device& gpu, unsigned amt, unsigned src_offset, unsigned dst_offset )
     {
       const auto device = gpu.device() ;
       ::vk::DeviceSize     offset ;
       ::vk::DeviceSize     amount ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = src_offset       ;
       amount = amt              ;

       dst    = static_cast<void*>( reinterpret_cast<unsigned char*>( dst ) + dst_offset ) ;
       
       auto result = device.mapMemory  ( src, offset, amount, flag, &mem      ) ;
                     std::memcpy       ( dst, mem, static_cast<size_t>( amt ) ) ;
                     device.unmapMemory( src                                  ) ;
                     
       if( result != vk::Result::eSuccess )
       {
         std::cout << "Error: " << vk::to_string( result ) << "\n" ;
       }
     }
     
     void Vulkan::free( Vulkan::Memory& mem, Vulkan::Device& gpu )
     {
       const auto device = gpu.device() ;
       if( mem && gpu.initialized() )
       {
         device.free ( mem ) ;
       }
     }
     
     Vulkan::Memory Vulkan::createMemory( const Vulkan::Device& gpu, unsigned size, nyx::MemoryFlags flags, unsigned filter )
     {
       const auto                      device   = gpu.device()                                                                                ;
       const auto                      p_device = gpu.physicalDevice()                                                                        ;
       const ::vk::MemoryPropertyFlags flag     = static_cast<vk::MemoryPropertyFlags>( static_cast<VkMemoryPropertyFlags>( flags.value() ) ) ;
       Vulkan::Memory           mem    ;
       ::vk::MemoryAllocateInfo info   ;

       info.setAllocationSize ( size                              ) ;
       info.setMemoryTypeIndex( memType( filter, flag, p_device ) ) ;
       
       mem = device.allocateMemory( info, nullptr ).value ;

       return mem ;
     }
     
     Vulkan::Memory Vulkan::createMemory( const Vulkan::Device& gpu, unsigned size, unsigned filter )
     {
       nyx::MemoryFlags flags ;
       
       return this->createMemory( gpu, size, flags, filter ) ;
     }
     
     
     const char* Vulkan::platformSurfaceInstanceExtensions()
     {
       #ifdef WIN32
       return VK_KHR_WIN32_SURFACE_EXTENSION_NAME ;
       #elif __linux__
       return VK_KHR_XCB_SURFACE_EXTENSION_NAME ;
       #endif 
     }

     #ifdef WIN32
     Vulkan::Context Vulkan::contextFromBaseWindow( const nyx::win32::Window& window )
     {
       VkWin32SurfaceCreateInfoKHR info       ;
       VkSurfaceKHR                surface    ;
       vk::SurfaceKHR              vk_surface ;
       vk::Result                  result     ;

       info.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR ;
       info.pNext     = nullptr                                         ;
       info.flags     = 0                                               ;
       info.hinstance = window.instance()                               ;
       info.hwnd      = window.handle()                                 ;

       if( data.instance )
       {
         result = static_cast<vk::Result>( vkCreateWin32SurfaceKHR( data.instance, &info, nullptr, &surface ) ) ;
         if( result != vk::Result::eSuccess )
         {
           std::cout << "Error creating surface: " << vk::to_string( result ) << "\n" ; // TODO convert error.
         }
         vk_surface = surface ;
       }
       
       return vk_surface ;
     }

     #elif __linux__
     Vulkan::Context Vulkan::contextFromBaseWindow( const nyx::lx::Window& window )
     {
       VkXcbSurfaceCreateInfoKHR info         ;
       VkSurfaceKHR              surface      ;
       vk::SurfaceKHR            vk_surface   ;
       vk::Result                result       ;
       
       info.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR ;
       info.pNext      = nullptr                                       ;
       info.flags      = 0                                             ;
       info.connection = window.connection()                           ;
       info.window     = window.window()                               ;
       
       if( data.instance )
       {
         result = static_cast<vk::Result>( vkCreateXcbSurfaceKHR( data.instance, &info, nullptr, &surface ) ) ;
         
         if( result != vk::Result::eSuccess )
         {
           std::cout << "Error creating surface: " << vk::to_string( result ) << "\n" ; // TODO convert error.
         }

         vk_surface = surface ;
       }
       
       return vk_surface ;
     }
     #endif  
   }
 }
