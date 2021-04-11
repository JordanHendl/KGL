/*
 * Copyright (C) 2021 Jordan Hendl
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

/* 
 * File:   Chain.cpp
 * Author: jhendl
 * 
 * Created on March 19, 2021, 9:42 PM
 */

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS

#include "Chain.h"
#include "CommandBuffer.h"
#include "Queue.h"
#include "Vulkan.h"
#include "Renderer.h"
#include "library/Image.h"
#include "library/Memory.h"
#include <library/Array.h>
#include <library/Chain.h>
#include <vulkan/vulkan.hpp>
#include <mutex>
#include <map>
#include <algorithm>

namespace nyx
{
  namespace vkg
  {
    static constexpr unsigned COMMAND_BUFFER_COUNT = 4        ;
    static constexpr unsigned MAX_STAGING_BUFFERS  = 4        ;
    static constexpr unsigned STAGING_BUFFER_SIZE  = 10000000 ;

    struct StagingBuffer
    {
      nyx::Array<Vulkan, unsigned char> buffer ;
      std::mutex                        mutex  ;

      bool lock() ;
      void unlock() ;
    };
    
    struct ChainData
    {
      vkg::Queue             queue   ;
      vkg::CommandBuffer     parent  ;
      vkg::CommandBuffer     cmd     ;
      const vkg::RenderPass* pass    ;
      unsigned               current ;
      
      ChainData() ;
      void record( bool use_render_pass = false ) ;
      
      StagingBuffer* findStaging() const ;
    };
    
    using StagingBuffers = std::map<unsigned, StagingBuffer*> ;
    
    static StagingBuffers staging_buffers ;
    
    bool StagingBuffer::lock()
    {
      return this->mutex.try_lock() ;
    }
    
    void StagingBuffer::unlock()
    {
      this->mutex.unlock() ;
    }

    void ChainData::record( bool use_render_pass )
    {
      if( !this->cmd.recording() )
      {
        if( this->pass != nullptr && this->pass->initialized() && use_render_pass )
        {
          this->cmd.record( *this->pass ) ;
        }
        else
        {
          this->cmd.record() ;
        }
      }
    }
    
    StagingBuffer* ChainData::findStaging() const
    {
      static unsigned id = 0       ;
      StagingBuffer* tmp = nullptr ;

      StagingBuffers::iterator iter = staging_buffers.begin() ;
      if( iter == staging_buffers.end() )
      {
        staging_buffers.insert( { id++, new StagingBuffer() } ) ;
        iter = staging_buffers.begin() ;
        iter->second->buffer.initialize( this->queue.device(), sizeof( unsigned char ) * STAGING_BUFFER_SIZE, true, nyx::ArrayFlags::TransferDst | nyx::ArrayFlags::TransferSrc ) ;
      }
      
      while( iter != staging_buffers.end() )
      {
        if( iter->second->lock() ) return iter->second ;
        iter++ ;
        
        if( iter == staging_buffers.end() && staging_buffers.size() < MAX_STAGING_BUFFERS )
        {
          staging_buffers.insert( { id++, new StagingBuffer() } ) ;
          iter = staging_buffers.find( id - 1 ) ;
          iter->second->buffer.initialize( this->queue.device(), sizeof( unsigned char ) * STAGING_BUFFER_SIZE, true, nyx::ArrayFlags::TransferDst | nyx::ArrayFlags::TransferSrc ) ;
        }
        else if( iter == staging_buffers.end() )
        {
          iter = staging_buffers.begin() ;
        }
      }
      
      return tmp ;
    }

    ChainData::ChainData()
    {
      this->pass    = nullptr ;
      this->current = 0       ;
    }

    Chain::Chain()
    {
      this->chain_data = new ChainData() ;
    }

    Chain::~Chain()
    {
      delete this->chain_data ;
    }

    void Chain::initialize( unsigned gpu, unsigned window_id )
    {
      if( Vulkan::hasWindow( window_id ) )
      {
        data().queue = Vulkan::presentQueue( window_id, gpu ) ;
        data().cmd.initialize( data().queue, COMMAND_BUFFER_COUNT ) ;
      }
    }

    void Chain::initialize( unsigned gpu, ChainType type )
    {
      this->reset() ;
      switch( type )
      {
        case nyx::ChainType::Graphics : data().queue = Vulkan::graphicsQueue( gpu ) ; break ;
        case nyx::ChainType::Compute  : data().queue = Vulkan::computeQueue ( gpu ) ; break ;
//        case nyx::ChainType::Transfer : data().queue = Vulkan::tranferQueue ( gpu ) ; break
        default : data().queue = Vulkan::computeQueue( gpu ) ;
      }

      data().cmd.initialize( data().queue, COMMAND_BUFFER_COUNT ) ;
    }
    
    void Chain::initialize( const RenderPass& pass, ChainType type )
    {
      const unsigned gpu = pass.device() ;
      this->reset() ;
      data().pass = &pass ;
      switch( type )
      {
        case nyx::ChainType::Graphics : data().queue = Vulkan::graphicsQueue( gpu ) ; break ;
        case nyx::ChainType::Compute  : data().queue = Vulkan::computeQueue ( gpu ) ; break ;
//        case nyx::ChainType::Transfer : data().queue = Vulkan::tranferQueue ( gpu ) ; break
        default : data().queue = Vulkan::computeQueue( gpu ) ;
      }

      data().cmd.initialize( data().queue, COMMAND_BUFFER_COUNT ) ;
    }
    
    void Chain::initialize( const RenderPass& pass, unsigned window_id )
    {
      this->reset() ;
      data().pass  = &pass ;
      data().queue = Vulkan::presentQueue( window_id, pass.device() ) ;
      this->initialize( pass.device(), window_id ) ;
    }

    unsigned Chain::device() const
    {
      return data().queue.device() ;
    }

    void Chain::transition( vkg::Image& image, nyx::ImageLayout layout )
    {
      vk::ImageMemoryBarrier    barrier    ;
      vk::ImageSubresourceRange range      ;
      vk::PipelineStageFlags    src        ;
      vk::PipelineStageFlags    dst        ;
      vk::DependencyFlags       dep_flags  ;
      vk::ImageLayout           new_layout ;
      vk::ImageLayout           old_layout ;
      
      new_layout = nyx::vkg::Vulkan::convert( layout         ) ;
      old_layout = nyx::vkg::Vulkan::convert( image.layout() ) ;

      range.setBaseArrayLayer( 0                               ) ;
      range.setBaseMipLevel  ( 0                               ) ;
      range.setLevelCount    ( 1                               ) ;
      range.setLayerCount    ( image.layers()                  ) ;
      range.setAspectMask    ( vk::ImageAspectFlagBits::eColor ) ;
      
      barrier.setOldLayout       ( old_layout                       ) ;
      barrier.setNewLayout       ( new_layout                       ) ;
      barrier.setImage           ( image.image()                    ) ;
      barrier.setSubresourceRange( range                            ) ;
      
      dep_flags = vk::DependencyFlagBits::eDeviceGroupKHR  ;
      src       = vk::PipelineStageFlagBits::eTopOfPipe    ;
      dst       = vk::PipelineStageFlagBits::eBottomOfPipe ;
      
      image.setLayout( layout ) ;
      data().record() ;
      if( new_layout != vk::ImageLayout::eUndefined )
      {
        data().cmd.buffer().pipelineBarrier( src, dst, dep_flags, 0, nullptr, 0, nullptr, 1, &barrier ) ;
      }
    }

    void Chain::synchronize()
    {
      data().queue.wait() ;
    }

    void Chain::submit()
    {
      if( data().cmd.recording() )
      {
        data().cmd.stop() ;
      }

      if( !data().parent.initialized() )
      {
        data().queue.submit( data().cmd ) ;
        data().cmd.advance() ;
      }
    }
    
    void Chain::reset()
    {
      data().cmd .reset() ;
      data().pass = nullptr ;
    }

    void Chain::copy( const vkg::Image& src, vkg::Image& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
    {
      const auto src_layout = Vulkan::convert( src.layout() ) ;
      const auto dst_layout = Vulkan::convert( dst.layout() ) ;
      
      vk::ImageCopy region ;
      vk::Extent3D  extent ;
      
      amt = amt ;
      extent.setWidth ( dst.width()  ) ;
      extent.setHeight( dst.height() ) ;
      extent.setDepth ( dst.layers() ) ;
      
      region.setExtent        ( extent            ) ;
      region.setSrcOffset     ( src_offset        ) ;
      region.setDstOffset     ( dst_offset        ) ;
      region.setSrcSubresource( src.subresource() ) ;
      region.setDstSubresource( dst.subresource() ) ;
      
      data().record() ;
      data().cmd.buffer().copyImage( src.image(), src_layout, dst.image(), dst_layout, 1, &region ) ;
    }
    
    void Chain::copy( const vkg::Buffer& src, vkg::Buffer& dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset )
    {
      vk::BufferCopy    region  ;
      vk::MemoryBarrier barrier ;
      
      barrier.setSrcAccessMask( vk::AccessFlagBits::eMemoryRead  ) ;
      barrier.setDstAccessMask( vk::AccessFlagBits::eMemoryWrite ) ;
      
      region.setSize     ( copy_amt * element_size ) ;
      region.setSrcOffset( src_offset              ) ;
      region.setDstOffset( dst_offset              ) ;
      
      data().record() ;
      data().cmd.buffer().copyBuffer( src.buffer(), dst.buffer(), 1, &region ) ;
    }
    
    void Chain::copy( const void* src, vkg::Buffer& dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset )
    {
      const unsigned byte_size = copy_amt * element_size ;

      StagingBuffer*       buffer = data().findStaging() ;
      vkg::Buffer&         tmp    = buffer->buffer       ;
      nyx::Memory<Vulkan>& memory = tmp.memory()         ;
      
      memory.copyToDevice( src, byte_size, src_offset, 0 ) ;
      this->copy( buffer->buffer, dst, copy_amt, element_size, 0, dst_offset ) ;
      buffer->unlock() ;
    }
    
    void Chain::copy( const void* src, vkg::Image& dst )
    {
      const unsigned byte_size = dst.byteSize() ;
      
      StagingBuffer*       buffer = data().findStaging() ;
      vkg::Buffer&         tmp    = buffer->buffer       ;
      nyx::Memory<Vulkan>& memory = tmp.memory()         ;
      
      memory.copyToDevice( src, byte_size, 0, 0 ) ;
      this->copy( buffer->buffer, dst, byte_size, 1, 0, 0 ) ;
      buffer->unlock() ;
    }
    
    void Chain::copy( const vkg::Buffer& src, void* dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset )
    {
      const unsigned byte_size = copy_amt * element_size ;

      StagingBuffer*       buffer   = data().findStaging()                                   ;
      vkg::Buffer&         tmp      = buffer->buffer                                         ;
      nyx::Memory<Vulkan>& memory   = tmp.memory()                                           ;
      const unsigned char* host_mem = static_cast<const unsigned char*>( memory.hostData() ) ;
      
      
      this->copy( src, buffer->buffer, copy_amt, element_size, src_offset, dst_offset ) ;
      Vulkan::deviceSynchronize( src.device() ) ;
      memory.syncToHost() ;
      std::copy( host_mem, host_mem + byte_size, static_cast<unsigned char*>( dst ) ) ;
      buffer->unlock() ;
    }
    
    void Chain::copy( const vkg::Image& src, void* dst )
    {
      const unsigned byte_size = src.byteSize() ;
      
      StagingBuffer*       buffer   = data().findStaging()                                   ;
      vkg::Buffer&         tmp      = buffer->buffer                                         ;
      nyx::Memory<Vulkan>& memory   = tmp.memory()                                           ;
      const unsigned char* host_mem = static_cast<const unsigned char*>( memory.hostData() ) ;
      
      this->copy( src, buffer->buffer ) ;
      Vulkan::deviceSynchronize( data().cmd.device() ) ;
      memory.syncToHost() ;
      std::copy( host_mem, host_mem + byte_size, static_cast<unsigned char*>( dst ) ) ;
      buffer->unlock() ;
    }
    
    void Chain::copy( const vkg::Image& src, vkg::Buffer& dst, unsigned copy_amt, unsigned src_offset, unsigned dst_offset )
    {
      vk::BufferImageCopy info   ;
      vk::Extent3D        extent ;
      
      extent.setWidth ( src.width () ) ;
      extent.setHeight( src.height() ) ;
      extent.setDepth ( src.layers() ) ;
      
      info.setImageExtent      ( extent            ) ;
      info.setBufferImageHeight( 0                 ) ;
      info.setBufferRowLength  ( 0                 ) ;
      info.setImageOffset      ( dst_offset        ) ;
      info.setImageSubresource ( src.subresource() ) ;
      
      // In case we need in the future.
      src_offset   = src_offset   ;
      copy_amt     = copy_amt     ;

      data().record() ;
      data().cmd.buffer().copyImageToBuffer( src.image(), vk::ImageLayout::eTransferSrcOptimal, dst.buffer(), 1, &info ) ;
    }
    
    void Chain::copy( const vkg::Buffer& src, vkg::Image& dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset )
    {
      vk::BufferImageCopy info       ;
      vk::Extent3D        extent     ;
      nyx::ImageLayout    old_layout ;
      
      old_layout = dst.layout() ;
      
      extent.setWidth ( dst.width () ) ;
      extent.setHeight( dst.height() ) ;
      extent.setDepth ( dst.layers() ) ;
      
      info.setImageExtent      ( extent            ) ;
      info.setBufferImageHeight( 0                 ) ;
      info.setBufferRowLength  ( 0                 ) ;
      info.setImageOffset      ( dst_offset        ) ;
      info.setImageSubresource ( dst.subresource() ) ;
      
      // In case we need in the future.
      src_offset   = src_offset   ;
      copy_amt     = copy_amt     ;
      element_size = element_size ;

      this->transition ( dst, nyx::ImageLayout::TransferDst ) ;
      data().cmd.buffer().copyBufferToImage( src.buffer(), dst.image(), vk::ImageLayout::eTransferDstOptimal, 1, &info ) ;
      this->transition ( dst, old_layout ) ;
    }
    
    void Chain::drawBase( const vkg::Renderer& renderer, const vkg::Buffer& vertices, unsigned count, unsigned offset )
    {
      data().record( true ) ;
      data().cmd.bind    ( renderer.pipeline()     ) ;
      data().cmd.bind    ( renderer.descriptor()   ) ;
      data().cmd.drawBase( vertices, count, offset ) ;
    }

    void Chain::pushBase( const Renderer& pipeline, const void* value, unsigned byte_size, unsigned offset )
    {
      data().record( true ) ;
      data().cmd.bind( pipeline.pipeline()                  ) ;
      data().cmd.pushConstantBase( value, byte_size, offset ) ;
    }

    const ChainData& Chain::data() const
    {
      return *this->chain_data ;
    }

    ChainData& Chain::data()
    {
      return *this->chain_data ;
    }
  }
}
