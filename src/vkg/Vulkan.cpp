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
#define VULKAN_HPP_NO_EXCEPTIONS

#ifdef __WIN32__
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows/Window.h>
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

     unsigned Vulkan::convertError( unsigned error )
     {
       return 0 ;
     }

     void Vulkan::copyToDevice( const void* src, Vulkan::Memory& dst, Vulkan::Device& gpu, unsigned amt )
     {
       const auto device = gpu.device() ;

       ::vk::DeviceSize     offset ;
       ::vk::DeviceSize     amount ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = 0   ;
       amount = amt ;
       
       device.mapMemory  ( dst, offset, amount, flag, &mem      ) ;
       std::memcpy       ( mem, src, static_cast<size_t>( amt ) ) ;
       device.unmapMemory( dst                                  ) ;
     }
     
     void Vulkan::copyToHost( const Vulkan::Memory& src, Vulkan::Data dst, Vulkan::Device& gpu, unsigned amt )
     {
       const auto device = gpu.device() ;
       ::vk::DeviceSize     offset ;
       ::vk::DeviceSize     amount ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = 0   ;
       amount = amt ;
       
       device.mapMemory  ( src, offset, amount, flag, &mem      ) ;
       std::memcpy       ( dst, mem, static_cast<size_t>( amt ) ) ;
       device.unmapMemory( src                                  ) ;
     }
     
     void Vulkan::free( Vulkan::Memory& mem, Vulkan::Device& gpu )
     {
       const auto device = gpu.device() ;
       
       device.free ( mem ) ;
     }
     
     Vulkan::Memory Vulkan::createMemory( unsigned size, const Vulkan::Device& gpu, Vulkan::MemoryFlags flags )
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
     
     Vulkan::Memory Vulkan::createMemory( unsigned size, const Vulkan::Device& gpu )
     {
       Vulkan::MemoryFlags flags ;
       
       flags = static_cast<unsigned>( ::vk::MemoryPropertyFlagBits::eDeviceLocal ) ;

       return this->createMemory( size, gpu, flags ) ;
     }
     
     
     const char* Vulkan::platformSurfaceInstanceExtensions()
     {
       #ifdef __WIN32__
//       return VK_KHR_WIN ;
       return "" ;
       #elif __linux__
       return VK_KHR_XCB_SURFACE_EXTENSION_NAME ;
       #endif 
     }

     #ifdef __WIN32__
     Vulkan::Context Vulkan::contextFromBaseWindow( const kgl::win32::Window& window )
     {
       
     }
     #elif __linux__
     Vulkan::Context Vulkan::contextFromBaseWindow( const kgl::lx::Window& window )
     {
       VkXcbSurfaceCreateInfoKHR info         ;
       vk::SurfaceKHR            vk_surface   ;
       VkSurfaceKHR              surface      ;
       VkResult                  result       ;
       PFN_vkCreateXcbSurfaceKHR function_ptr ;
       
       info.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR ;
       info.pNext      = nullptr                                       ;
       info.flags      = 0                                             ;
       info.connection = window.connection()                           ;
       info.window     = window.window()                               ;
       
       
       if( kgl::vkg::vk_instance )
       {
         result = vkCreateXcbSurfaceKHR( kgl::vkg::vk_instance, &info, nullptr, &surface ) ;
         vk_surface = surface ;
       }

       return vk_surface ;
     }
     #endif  
   }
 }