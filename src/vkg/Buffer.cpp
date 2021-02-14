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

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS
#include "Buffer.h"
#include "Vulkan.h"
#include "Device.h"
#include <library/Memory.h> 
#include <library/Array.h>
#include <vulkan/vulkan.hpp>
#include <string>

namespace nyx
{
  namespace vkg
  {
    using IMPL = nyx::vkg::Vulkan ;
    
    /** Structure to encompass a vulkan buffer's internal data.
     */
    struct BufferData
    {
      nyx::Memory<IMPL>      internal_memory ;
      ::nyx::vkg::Device     device          ;
      unsigned               device_id       ;
      vk::MemoryRequirements requirements    ;
      vk::DeviceAddress      address         ;
      vk::BufferUsageFlags   usage_flags     ;
      vk::Buffer             buffer          ;
      bool                   preallocated    ;
      bool                   host_local      ;
      bool                   initialized     ;
      unsigned               device_size     ;

      /** Helper method to create the device address, if available.
       */
      void makeDeviceAddress() ;

      /** Method to create a vulkan buffer and return it.
       * @param size The byte size of buffer to make.
       * @return A Buffer created to the input size.
       */
      ::vk::Buffer createBuffer( unsigned size ) ;
      
      /** Default constructor.
       */
      BufferData() ;
    };
    
    void BufferData::makeDeviceAddress()
    {
      vk::BufferDeviceAddressInfo info ;
      
      info.setBuffer( this->buffer ) ;
      
      if( this->device.hasExtension( VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME ) )
      {
        this->address = this->device.device().getBufferAddress( &info ) ;
      }
    }

    ::vk::Buffer BufferData::createBuffer( unsigned size )
    {
      ::vk::BufferCreateInfo info   ;
      ::vk::Buffer           buffer ;
      
      info.setSize       ( size                                                    ) ;
      if( this->device.hasExtension( VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME ) )
      {
        info.setUsage      ( this->usage_flags | vk::BufferUsageFlagBits::eShaderDeviceAddress ) ;
      }
      else
      {
        info.setUsage      ( this->usage_flags ) ;
      }
      info.setSharingMode( ::vk::SharingMode::eExclusive                                     ) ;
      
      auto result = this->device.device().createBuffer( info, nullptr ) ;
      
      vkg::Vulkan::add( result.result ) ;
      buffer = result.value ;
      return buffer ;
    }

    BufferData::BufferData()
    {
      this->preallocated = false                                                                             ;
      this->usage_flags  = ::vk::BufferUsageFlagBits::eTransferSrc | ::vk::BufferUsageFlagBits::eTransferDst ;
      this->device_size  = 0                                                                                 ;
      this->host_local   = false                                                                             ;
      this->initialized  = false                                                                             ;
    }
    
    Buffer::Buffer()
    {
      this->buffer_data = new BufferData() ;
    }

    Buffer::Buffer( const Buffer& src )
    {
      this->buffer_data = new BufferData() ;
      *this = src ;
    }

    Buffer::~Buffer()
    {
      delete this->buffer_data ;
    }

    Buffer& Buffer::operator=( const Buffer& src )
    {
      *this->buffer_data = *src.buffer_data ;
      
      return *this ;
    }
    
    void Buffer::reset()
    {
      if( data().buffer )
      {
        data().device.device().destroyBuffer( data().buffer ) ;
      }
      
      if( !data().preallocated )
      {
        data().internal_memory.deallocate() ;
      }
    }
    
    bool Buffer::initialized() const
    {
      return data().initialized ;
    }
    
    nyx::DeviceAddress Buffer::address( unsigned offset ) const
    {
      return data().address + offset ;
    }

    bool Buffer::initialize( nyx::Memory<nyx::vkg::Vulkan>& prealloc, unsigned size )
    {
      data().internal_memory = prealloc ;
      data().preallocated    = true     ;
      
      return this->initialize( prealloc.device(), size == 0 ? prealloc.size() : size ) ;
    }
    
    bool Buffer::initialize( unsigned gpu, unsigned size, bool host_local, nyx::ArrayFlags flags )
    {
      data().usage_flags = static_cast<vk::BufferUsageFlags>( static_cast<VkBufferUsageFlags>( flags.value() ) ) ;
      
      return this->initialize( gpu, size, host_local ) ;
    }

    bool Buffer::initialize( unsigned gpu, unsigned size, bool host_local )
    {
      unsigned needed_size ;
      
      Vulkan::initialize() ;

      data().device_id    = gpu                                                                 ;
      data().device       = Vulkan::device( gpu )                                               ;
      data().buffer       = data().createBuffer( size )                                         ;
      data().requirements = data().device.device().getBufferMemoryRequirements( data().buffer ) ; 
      data().host_local   = host_local                                                          ;

      if( !data().preallocated )
      {
        if( host_local )
          data().internal_memory.initialize( gpu, data().requirements.size, data().requirements.memoryTypeBits, host_local, nyx::MemoryFlags::HostVisible | nyx::MemoryFlags::HostCoherent ) ;
        else
          data().internal_memory.initialize( gpu, data().requirements.size, data().requirements.memoryTypeBits, host_local ) ;
      }

      needed_size = data().internal_memory.size() - data().internal_memory.offset() ;

      if( data().requirements.size <= needed_size )
      {
        vkg::Vulkan::add( data().device.device().bindBufferMemory( data().buffer, data().internal_memory.memory(), data().internal_memory.offset() ) ) ;
        data().makeDeviceAddress() ;
        data().initialized = true ;
        return true ;
      }
      else
      {
        return false ;
      }
    }
    
    const ::vk::Buffer& Buffer::buffer() const
    {
      return data().buffer ;
    }

    void Buffer::copy( const Buffer& buffer, unsigned size, const nyx::vkg::CommandBuffer& cmd_buff, unsigned srcoffset, unsigned dstoffset )
    { 
      ::vk::BufferCopy region ;
      
      region.setSize     ( size      ) ;
      region.setSrcOffset( srcoffset ) ;
      region.setDstOffset( dstoffset ) ;
      
      for( unsigned i = 0; i < cmd_buff.size(); i++ )
      {
        cmd_buff.buffer( i ).copyBuffer( buffer.buffer(), this->buffer(), 1, &region ) ;
      }
    }
    
    void Buffer::copyToDevice( const void* buffer, unsigned byte_size, unsigned srcoffset, unsigned dstoffset )
    { 
      if( data().host_local )
      {
        data().internal_memory.copyToDevice( buffer, byte_size, srcoffset, dstoffset ) ;
      }
    }
    
    unsigned Buffer::device() const
    {
      return data().device_id ;
    }

    unsigned Buffer::size() const 
    {
      return data().requirements.size ;
    }
    
    nyx::Memory<nyx::vkg::Vulkan>& Buffer::memory()
    {
      return data().internal_memory ;
    }

    const nyx::Memory<nyx::vkg::Vulkan>& Buffer::memory() const
    {
      return data().internal_memory ;
    }

    void Buffer::setUsage( nyx::ArrayFlags flag  )
    {
      data().usage_flags = static_cast<vk::BufferUsageFlagBits>( static_cast<VkBufferUsageFlagBits>( flag.value() ) ) ;
    }
    
    BufferData& Buffer::data()
    {
      return *this->buffer_data ;
    }

    const BufferData& Buffer::data() const
    {
      return *this->buffer_data ;
    }
  }
}
