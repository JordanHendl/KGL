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
#include <algorithm>
#include <iostream>

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

 namespace kgl
 {
   namespace vkg
   {
     /** The instance to use for all vulkan API calls.
      */
     static vk::Instance vk_instance ;

     uint32_t memType( uint32_t filter, ::vk::MemoryPropertyFlags flag, ::vk::PhysicalDevice device )
     {
       ::vk::PhysicalDeviceMemoryProperties mem_prop ;

       mem_prop = device.getMemoryProperties() ;
       for( unsigned i = 0; i < mem_prop.memoryTypeCount; i++ )
       {
         if( filter & ( 1 << i ) && ( mem_prop.memoryTypes[ i ].propertyFlags & flag ) == flag )
         {
           return i ;
         }
       }
       return 0 ;
     }
     
     
     Error::Error()
     {
       this->err = Error::None ;
     }
     
     Error::Error( const Error& error )
     {
       this->err = error.err ;
     }
     
     Error::Error( unsigned error )
     {
       this->err = error ;
     }
     
     unsigned Error::error() const
     {
       return *this ;
     }
     
     Error::operator unsigned() const
     {
       return this->err ;
     }

     kgl::vkg::Vulkan::MemoryFlags::MemoryFlags()
     {
       using Flags = ::vk::MemoryPropertyFlagBits ;
       this->flag = static_cast<unsigned>( ( Flags::eHostCoherent | Flags::eHostVisible ) ) ;
     }
     
     kgl::vkg::Vulkan::MemoryFlags::MemoryFlags( unsigned flags )
     {
       this->flag = flags ;
     }
     
     kgl::vkg::Vulkan::MemoryFlags::MemoryFlags( Vulkan::MemoryPropFlag flags )
     {
       this->flag = static_cast<unsigned>( flags ) ;
     }
     
     kgl::vkg::Vulkan::MemoryFlags::MemoryFlags( ::vk::MemoryPropertyFlagBits flags )
     {
       this->flag = static_cast<unsigned>( flags ) ;
     }

     Vulkan::MemoryPropFlag kgl::vkg::Vulkan::MemoryFlags::val()
     {
       return static_cast<::vk::MemoryPropertyFlags>( this->flag ) ;
     }
     
     Vulkan::MemoryFlags& Vulkan::MemoryFlags::operator=( unsigned flags )
     {
       this->flag = ( flags ) ;
       
       return *this ;
     }

     void Vulkan::initialize( const vk::Instance& instance )
     {
       ::kgl::vkg::vk_instance = instance ;
     }

     vkg::Error Vulkan::convertError( vk::Result error )
     {
       switch( error )
       {
         case vk::Result::eErrorDeviceLost : return kgl::vkg::Error( vkg::Error::DeviceLost  ) ;
         default : return kgl::vkg::Error( vkg::Error::None ) ;
       }
     }

     void Vulkan::copyToDevice( const void* src, Vulkan::Memory& dst, Vulkan::Device& gpu, unsigned amt, unsigned src_offset, unsigned dst_offset )
     {
       const auto device = gpu.device() ;

       ::vk::DeviceSize     offset ;
       ::vk::DeviceSize     amount ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = dst_offset       ;
       amount = amt              ;

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
       
       device.free ( mem ) ;
     }
     
     Vulkan::Memory Vulkan::createMemory( const Vulkan::Device& gpu, unsigned size, Vulkan::MemoryFlags flags )
     {
       const auto                      device   = gpu.device()         ;
       const auto                      p_device = gpu.physicalDevice() ;
       const ::vk::MemoryPropertyFlags flag     = flags.val()          ;
       Vulkan::Memory           mem    ;
       ::vk::MemoryAllocateInfo info   ;

       info.setAllocationSize ( size                                  ) ;
       info.setMemoryTypeIndex( memType( 0xFFFFFFFF, flag, p_device ) ) ;
       
       mem = device.allocateMemory( info, nullptr ).value ;

       return mem ;
     }
     
     Vulkan::Memory Vulkan::createMemory( const Vulkan::Device& gpu, unsigned size )
     {
       Vulkan::MemoryFlags flags ;
       
       flags = static_cast<unsigned>( ::vk::MemoryPropertyFlagBits::eDeviceLocal ) ;

       return this->createMemory( gpu, size, flags ) ;
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
     Vulkan::Context Vulkan::contextFromBaseWindow( const kgl::win32::Window& window )
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

       if( kgl::vkg::vk_instance )
       {
         result = static_cast<vk::Result>( vkCreateWin32SurfaceKHR( kgl::vkg::vk_instance, &info, nullptr, &surface ) ) ;
         if( result != vk::Result::eSuccess )
         {
           std::cout << "Error creating surface: " << vk::to_string( result ) << "\n" ;
         }
         vk_surface = surface ;
       }
       
       return vk_surface ;
     }

     #elif __linux__
     Vulkan::Context Vulkan::contextFromBaseWindow( const kgl::lx::Window& window )
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
       
       if( kgl::vkg::vk_instance )
       {
         result = static_cast<vk::Result>( vkCreateXcbSurfaceKHR( kgl::vkg::vk_instance, &info, nullptr, &surface ) ) ;
         
         if( result != vk::Result::eSuccess )
         {
           std::cout << "Error creating surface: " << vk::to_string( result ) << "\n" ;
         }

         vk_surface = surface ;
       }
       
       return vk_surface ;
     }
     #endif  
   }
 }
