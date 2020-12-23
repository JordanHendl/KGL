#include "Vulkan.h"
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#ifdef KGL_VULKAN_FOUND // Won't get compiler errors when building this without vulkan.

 namespace kgl
 {
   namespace vkg
   {
     using CommandBuffer = ::vk::CommandBuffer ;
     
     /** The instance to use for all vulkan API calls.
      */
     static vk::Instance vk_instance ;

     uint32_t memType( uint32_t filter, ::vk::MemoryPropertyFlags flag, ::vk::PhysicalDevice device )
     {
       ::vk::PhysicalDeviceMemoryProperties mem_prop ;

       mem_prop = device.getMemoryProperties() ;
       for( unsigned i = 0; i < mem_prop.memoryTypeCount; i++ )
       {
         if( filter & ( 1 << i ) && (mem_prop.memoryTypes[ i ].propertyFlags & flag ) == flag )
         {
           return i ;
         }
       }
       return 0 ;
     }
         
     struct VulkanData
     {
       ::vk::Device         gpu             ;
       ::vk::PhysicalDevice physical_device ;
     };

     kgl::vkg::Vulkan::MemoryFlags::MemoryFlags()
     {
       using Flags = ::vk::MemoryPropertyFlagBits ;
       this->flag = static_cast<unsigned>( ( Flags::eHostCoherent | Flags::eHostVisible ) ) ;
     }

     unsigned kgl::vkg::Vulkan::MemoryFlags::val()
     {
       return this->flag ;
     }
     
     Vulkan::MemoryFlags& Vulkan::MemoryFlags::operator=( unsigned flags )
     {
       this->flag = ( flags ) ;
       
       return *this ;
     }
     
     Vulkan::BufferFlags::BufferFlags()
     {
       using Flags = ::vk::BufferUsageFlagBits ;
       this->flag = static_cast<unsigned>( ( Flags::eTransferSrc | Flags::eTransferDst ) ) ;
     }
     
     unsigned Vulkan::BufferFlags::val()
     {
       return this->flag ;
     }
     
     Vulkan::BufferFlags& Vulkan::BufferFlags::operator=( unsigned flags )
     {
       this->flag = flags ;
       
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

     Vulkan::Vulkan()
     {
       this->vulkan_data = new VulkanData() ;
     }

     Vulkan::~Vulkan()
     {
       delete this->vulkan_data ;
     }
     
     void Vulkan::copyTo( Vulkan::Data src, Vulkan::Memory* dst, ::vk::Device& gpu, unsigned amt )
     {
       const auto device = data().gpu ;

       ::vk::DeviceSize     offset ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = 0 ;

       device.mapMemory  ( *dst, offset, amt, flag, &mem        ) ;
       memcpy            ( mem, src, static_cast<size_t>( amt ) ) ;
       device.unmapMemory( *dst                                 ) ;
     }
     
     void Vulkan::copyTo( Vulkan::Memory* src, Vulkan::Data dst, ::vk::Device& gpu, unsigned amt )
     {
       const auto device = data().gpu ;
       ::vk::DeviceSize     offset ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = 0 ;
       device.mapMemory  ( *src, offset, amt, flag, &mem        ) ;
       memcpy            ( dst, mem, static_cast<size_t>( amt ) ) ;
       device.unmapMemory( *src                                 ) ;
     }
     
     void Vulkan::copyTo( Memory* src, Memory* dst, unsigned amt )
     {
//       const auto device = data().gpu ;
//       ::vk::DeviceSize     offset ;
//       ::vk::MemoryMapFlags flag   ;
//       void*                mem    ;
//       
//       offset = 0 ;
//       device.mapMemory  ( *src, offset, amt, flag, &mem        ) ;
//       memcpy            ( dst, mem, static_cast<size_t>( amt ) ) ;
//       device.unmapMemory( *src                                 ) ;
     }
     
     void Vulkan::copyTo( Memory* src, Memory* dst, CommandRecord& record, unsigned amt )
     {
//       const auto device = data().gpu ;
//       ::vk::DeviceSize     offset ;
//       ::vk::MemoryMapFlags flag   ;
//       void*                mem    ;
//       
//       offset = 0 ;
//       device.mapMemory  ( *src, offset, amt, flag, &mem        ) ;
//       memcpy            ( dst, mem, static_cast<size_t>( amt ) ) ;
//       device.unmapMemory( *src                                 ) ;
     }
     
     void Vulkan::free( Vulkan::Memory* mem, ::vk::Device& gpu )
     {
       const auto device = data().gpu ;
       
       device.free ( *mem  ) ;
       
       delete( mem ) ;
       
       mem  = nullptr ;
     }
     
     Vulkan::Memory* Vulkan::createMemory( unsigned size, ::vk::Device& gpu, MemoryFlags flags )
     {
       const auto                      device   = data().gpu                                            ;
       const auto                      p_device = data().physical_device                                ;
       const ::vk::MemoryPropertyFlags flag     = static_cast<::vk::MemoryPropertyFlags>( flags.val() ) ;
       Vulkan::Memory          *mem    ;
       ::vk::MemoryAllocateInfo info   ;

       mem    = new Vulkan::Memory() ;

       info.setAllocationSize ( size                                  ) ;
       info.setMemoryTypeIndex( memType( 0xFFFFFFFF, flag, p_device ) ) ;
       
       *mem = device.allocateMemory( info, nullptr ).value ;

       return mem ;
     }
     
     Vulkan::Memory* Vulkan::createMemory( unsigned size, ::vk::Device& gpu )
     {
       MemoryFlags flags ;
       
       return this->createMemory( size, gpu, flags ) ;
     }

     const VulkanData& Vulkan::data() const
     {
       return *this->vulkan_data ;
     }
     
     VulkanData& Vulkan::data()
     {
       return *this->vulkan_data ;
     }
   }
 }
#endif