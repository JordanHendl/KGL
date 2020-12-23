#include "Vulkan.h"
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#ifdef KGL_VULKAN_FOUND // Won't get compiler errors when building this without vulkan.


unsigned operator|( unsigned first, vk::MemoryPropertyFlagBits second )
{
  return first | static_cast<unsigned>( second ) ;
}

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

     Vulkan::Vulkan()
     {
       this->vulkan_data = new VulkanData() ;
     }

     Vulkan::~Vulkan()
     {
       delete this->vulkan_data ;
     }
     
     void Vulkan::copyTo( const Vulkan::Data src, Vulkan::Memory& dst, ::vk::Device& gpu, unsigned amt )
     {
       const auto device = data().gpu ;

       ::vk::DeviceSize     offset ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = 0 ;

       device.mapMemory  ( dst, offset, amt, flag, &mem         ) ;
       memcpy            ( mem, src, static_cast<size_t>( amt ) ) ;
       device.unmapMemory( dst                                  ) ;
     }
     
     void Vulkan::copyTo( const Vulkan::Memory& src, Vulkan::Data dst, ::vk::Device& gpu, unsigned amt )
     {
       const auto device = data().gpu ;
       ::vk::DeviceSize     offset ;
       ::vk::MemoryMapFlags flag   ;
       void*                mem    ;
       
       offset = 0 ;
       device.mapMemory  ( src, offset, amt, flag, &mem         ) ;
       memcpy            ( dst, mem, static_cast<size_t>( amt ) ) ;
       device.unmapMemory( src                                  ) ;
     }
     
     void Vulkan::copyTo( const Memory& src, Memory& dst, unsigned amt )
     {
       // Stub for vulkan. This does nothing as Buffers are needed for GPU -> GPU copy.
     }
     
     void Vulkan::copyTo( const Memory& src, Memory& dst, CommandRecord& record, unsigned amt )
     {
     }
     
     void Vulkan::free( Vulkan::Memory& mem, ::vk::Device& gpu )
     {
       const auto device = data().gpu ;
       
       device.free ( mem ) ;
     }
     
     Vulkan::Memory Vulkan::createMemory( unsigned size, ::vk::Device& gpu, Vulkan::MemoryFlags flags )
     {
       const auto                      device   = data().gpu             ;
       const auto                      p_device = data().physical_device ;
       const ::vk::MemoryPropertyFlags flag     = flags.val()            ;
       Vulkan::Memory           mem    ;
       ::vk::MemoryAllocateInfo info   ;

       info.setAllocationSize ( size                                  ) ;
       info.setMemoryTypeIndex( memType( 0xFFFFFFFF, flag, p_device ) ) ;
       
       mem = device.allocateMemory( info, nullptr ).value ;

       return mem ;
     }
     
     Vulkan::Memory Vulkan::createMemory( unsigned size, ::vk::Device& gpu )
     {
       Vulkan::MemoryFlags flags ;
       
       flags = static_cast<unsigned>( ::vk::MemoryPropertyFlagBits::eDeviceLocal ) ;

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