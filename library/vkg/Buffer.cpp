#include "Buffer.h"
#include "Vulkan.h"
#include "Device.h"
#include "../Memory.h" //TODO fix this.
#include <vulkan/vulkan.hpp>

namespace kgl
{
  namespace vkg
  {
    using IMPL = kgl::vkg::Vulkan ;
    
    struct BufferData
    {
      kgl::Memory<IMPL>    internal_memory ;
      vk::BufferUsageFlags usage_flags     ;
      vk::Buffer           buffer          ;
    };

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

    void Buffer::initialize( kgl::Memory<kgl::vkg::Vulkan>& prealloc )
    {
    
    }

    void Buffer::initialize( const vk::Device& gpu, unsigned size, bool host_local )
    {
    
    }
    
    void Buffer::copy( const Buffer& buffer, ::vk::CommandBuffer cmd_buff )
    { 
      
    }
    
    kgl::Memory<kgl::vkg::Vulkan>& Buffer::memory()
    {
      return data().internal_memory ;
    }

    const kgl::Memory<kgl::vkg::Vulkan>& Buffer::memory() const
    {
      return data().internal_memory ;
    }

    void Buffer::setUsage( vk::BufferUsageFlagBits flag  )
    {
      
    }
    
    void Buffer::setUsage( Buffer::UsageFlags flag  )
    {
      
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