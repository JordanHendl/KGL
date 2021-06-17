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
#include "Pipeline.h"
#include "library/Image.h"
#include "library/Memory.h"
#include <library/Array.h>
#include <library/Chain.h>
#include <vulkan/vulkan.hpp>
#include <mutex>
#include <map>
#include <algorithm>
#include <limits>

namespace nyx
{
  namespace vkg
  {
    static constexpr unsigned COMMAND_BUFFER_COUNT = 4          ;
    static constexpr unsigned MAX_STAGING_BUFFERS  = 4          ;
//    static constexpr unsigned STAGING_BUFFER_SIZE  = 10000000 ;
    static constexpr unsigned STAGING_BUFFER_SIZE  = 536870912 ;

    struct StagingBuffer
    {
      nyx::Array<Vulkan, unsigned char> buffer ;
      std::mutex                        mutex  ;
      
      bool lock() ;
      void unlock() ;
    };
    
    struct ChainData
    {
      std::unordered_map<unsigned, vk::ImageMemoryBarrier > image_barriers  ;
      std::unordered_map<unsigned, vk::BufferMemoryBarrier> buffer_barriers ;
      
      vkg::Queue                 queue      ;
      mutable vkg::CommandBuffer cmd        ;
      vkg::Chain*                parent     ;
      const vkg::RenderPass*     pass       ;
      unsigned                   subpass_id ;
      std::mutex                 mutex      ;
      bool                       has_record ;
      unsigned                   num_cmd    ;
      mutable unsigned           current    ;
      bool                       dirty      ;
      vk::Semaphore              signal     ;
      vk::Semaphore              wait       ;
      bool                       first      ;
      
      ChainData() ;
      
      inline void record( bool use_render_pass = false ) const ;
      
      inline StagingBuffer* findStaging() const ;
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

    void ChainData::record( bool use_render_pass ) const
    {
      if( !this->cmd.recording() )
      {
        if( this->pass != nullptr && this->pass->initialized() && use_render_pass )
        {
          if( this->parent != nullptr )
          {
            for( unsigned index = 0; index < this->num_cmd; index++ )
            {
              this->cmd.record( *this->pass, this->subpass_id ) ;
              this->cmd.advance() ;
            }
          }
          else
          {
            for( unsigned index = 0; index < this->num_cmd; index++ )
            {
              this->cmd.record( *this->pass ) ;
              this->cmd.advance() ;
            }
          }
        }
        else
        {
          for( unsigned index = 0; index < this->num_cmd; index++ )
          {
            this->cmd.record() ;
            this->cmd.advance() ;
          }
        }
      }
      
      this->cmd.setActive( this->current ) ;
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
      this->wait       = nullptr    ;
      this->first      = true       ;
      this->has_record = false      ;
      this->subpass_id = UINT32_MAX ;
      this->parent     = nullptr    ;
      this->pass       = nullptr    ;
      this->num_cmd    = 1          ;
      this->current    = 0          ;
      this->dirty      = false      ;
    }

    Chain::Chain()
    {
      this->chain_data = new ChainData() ;
    }

    Chain::~Chain()
    {
      delete this->chain_data ;
    }

    void Chain::advance()
    {
      data().mutex.lock() ;
      data().cmd.advance() ;
      data().mutex.unlock() ;
    }
    
    void Chain::initialize( const Chain& parent, unsigned subpass_id )
    {
      data().pass       = parent.data().pass            ;
      data().parent     = const_cast<Chain*>( &parent ) ;
      data().subpass_id = subpass_id                    ;

      data().cmd.initialize( parent.data().cmd ) ;
      data().signal = Vulkan::device( parent.device() ).device().createSemaphore( {}, nullptr ).value ;
    }
    
    void Chain::initialize( unsigned gpu, unsigned window_id )
    {
      if( Vulkan::hasWindow( window_id ) )
      {
        data().queue = Vulkan::presentQueue( window_id, gpu ) ;
        data().cmd.initialize( data().queue, COMMAND_BUFFER_COUNT ) ;
        data().signal = Vulkan::device( gpu ).device().createSemaphore( {}, nullptr ).value ;
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
      data().signal = Vulkan::device( gpu ).device().createSemaphore( {}, nullptr ).value ;
    }
    
    void Chain::initialize( const RenderPass& pass, ChainType type, bool multi_pass )
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

      data().cmd.initialize( data().queue, COMMAND_BUFFER_COUNT, vkg::CommandBuffer::Level::Primary, multi_pass ) ;
      data().signal = Vulkan::device( gpu ).device().createSemaphore( {}, nullptr ).value ;
    }
    
    void Chain::initialize( const RenderPass& pass, unsigned window_id, bool multi_pass )
    {
      this->reset() ;
      data().pass  = &pass ;
      if( Vulkan::hasWindow( window_id ) )
      {
        data().queue = Vulkan::presentQueue( window_id, pass.device()                                             ) ;
        data().cmd.initialize( data().queue, COMMAND_BUFFER_COUNT, vkg::CommandBuffer::Level::Primary, multi_pass ) ;
        data().signal = Vulkan::device( pass.device() ).device().createSemaphore( {}, nullptr ).value ;
      }
    }
    
    bool Chain::initialized() const 
    {
      return data().cmd.initialized() ;
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
      
      barrier.setOldLayout          ( old_layout                       ) ;
      barrier.setNewLayout          ( new_layout                       ) ;
      barrier.setImage              ( image.image()                    ) ;
      barrier.setSubresourceRange   ( range                            ) ;
      barrier.setSrcQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED          ) ;
      barrier.setDstQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED          ) ;
      
      dep_flags = vk::DependencyFlags()                    ;
      src       = vk::PipelineStageFlagBits::eTopOfPipe    ;
      dst       = vk::PipelineStageFlagBits::eBottomOfPipe ;
      
      auto iter = data().image_barriers.end() ;
           iter = data().image_barriers.insert( iter, { data().image_barriers.size(), barrier } ) ;
      
      data().record() ;
      
      if( new_layout != vk::ImageLayout::eUndefined )
      {
        data().mutex.lock() ;
        
        for( unsigned index = 0; index < data().num_cmd; index++ )
        {
          data().cmd.buffer().pipelineBarrier( src, dst, dep_flags, 0, nullptr, 0, nullptr, 1, &iter->second ) ;
          data().cmd.advance() ;
        }
        
        data().dirty = true ;
        data().cmd.setActive( data().current ) ;
        data().mutex.unlock() ;
        image.setLayout( Vulkan::convert( new_layout )  ) ;
      }
    }

    void Chain::setMode( nyx::ChainMode mode )
    {
      if( mode == nyx::ChainMode::All ) data().num_cmd = COMMAND_BUFFER_COUNT ;
      else                              data().num_cmd = 1                    ;
    }

    void Chain::synchronize()
    {
      if( this->initialized() ) data().queue.wait() ;
    }

    void Chain::submit()
    {
      this->end() ;

      if( data().parent == nullptr && data().has_record && data().dirty )
      {
        data().mutex.lock() ;
        if( data().first )
        {
          data().queue.submit( data().cmd, data().signal ) ;
          data().first = false ;
          data().wait = data().signal ;
        }
        else
        {
          data().queue.submit( data().cmd, data().wait, data().signal ) ;
          data().wait = data().signal ;
        }

        data().current = data().cmd.current() ;
        data().mutex.unlock() ;
        data().dirty = false ;
        if( data().pass != nullptr ) data().pass->advance() ;
      }
      
      data().image_barriers .clear() ;
      data().buffer_barriers.clear() ;
    }
    
    void Chain::reset()
    {
      data().cmd .reset() ;
      data().pass = nullptr ;
    }
    
    void Chain::begin()
    {
      if( data().parent != nullptr )
      {
        data().cmd.advance() ;
        data().current = data().cmd.current() ;
      }

      data().record( data().pass != nullptr ) ;
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
      
      data().mutex.lock() ;
      data().record() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.buffer().copyImage( src.image(), src_layout, dst.image(), dst_layout, 1, &region ) ;
        data().cmd.advance() ;
      }
      
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
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

      data().mutex.lock() ;
      data().record() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.buffer().copyBuffer( src.buffer(), dst.buffer(), 1, &region ) ;
        data().cmd.advance() ;
      }
      
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
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

      data().mutex.lock() ;
      data().record() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.buffer().copyImageToBuffer( src.image(), vk::ImageLayout::eTransferSrcOptimal, dst.buffer(), 1, &info ) ;
        data().cmd.advance() ;
      }
      
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
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
      data().mutex.lock() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.buffer().copyBufferToImage( src.buffer(), dst.image(), vk::ImageLayout::eTransferDstOptimal, 1, &info ) ;
        data().cmd.advance() ;
      }
      
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
      this->transition ( dst, old_layout ) ;
    }
    
    void Chain::combine( const vkg::Chain& chain )
    {
      unsigned subpass = 0 ;

      if( data().parent == nullptr && chain.data().parent == this && !chain.data().cmd.recording() ) 
      {
        data().mutex.lock() ;
        data().has_record = true ;
      
        subpass = 0 ;
        for( unsigned index = 0; index < data().num_cmd; index++ )
        {
          data().cmd.combine( chain.data().cmd ) ;
          data().cmd.advance() ;
        }
        
        data().cmd.setActive( data().current ) ;
        subpass++ ;
        if( subpass < data().pass->subpassCount() )
        {
          for( unsigned index = 0; index < data().num_cmd; index++ )
          {
            data().cmd.nextSubpass() ;
            data().cmd.advance() ;
          }
        }
        
        data().cmd.setActive( data().current ) ;
        data().dirty = true ;
        data().mutex.unlock() ;
      }
    }

    void Chain::drawBase( const vkg::Pipeline& Pipeline, const vkg::Buffer& vertices, unsigned count, unsigned offset )
    {
      data().record( true ) ;
      data().has_record = true ;
      data().mutex.lock() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.bind    ( Pipeline                ) ;
        data().cmd.bind    ( Pipeline.descriptor()   ) ;
        data().cmd.drawBase( vertices, count, offset ) ;
        data().cmd.advance () ;
      }
      
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
    }
    
    void Chain::drawIndexedBase( const vkg::Pipeline& Pipeline, const vkg::Buffer& indices, unsigned index_count, const vkg::Buffer& vertices, unsigned vertex_count )
    {
      data().record( true ) ;
      data().has_record = true ;
      data().mutex.lock() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.bind    ( Pipeline                ) ;
        data().cmd.bind    ( Pipeline.descriptor()   ) ;
        data().cmd.drawIndexedBase( indices, vertices, index_count, vertex_count ) ;
        data().cmd.advance () ;
      }
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
    }
    
            
    void Chain::drawInstancedBase( unsigned instance_count, const vkg::Pipeline& Pipeline, const vkg::Buffer& indices, unsigned index_count, const vkg::Buffer& vertices, unsigned vertex_count )
    {
      data().record( true ) ;
      data().has_record = true ;
      data().mutex.lock() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.bind    ( Pipeline                ) ;
        data().cmd.bind    ( Pipeline.descriptor()   ) ;
        data().cmd.drawInstanced( indices, index_count, vertices, vertex_count, instance_count ) ;
        data().cmd.advance () ;
      }
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
    }
    
    void Chain::drawInstancedBase( unsigned instanced_count, const vkg::Pipeline& Pipeline, const vkg::Buffer& vertices, unsigned vertex_count )
    {
      data().record( true ) ;
      data().has_record = true ;
      data().mutex.lock() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.bind    ( Pipeline              ) ;
        data().cmd.bind    ( Pipeline.descriptor() ) ;
        data().cmd.drawInstanced( vertices, vertex_count, instanced_count ) ;
        data().cmd.advance () ;
      }
      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
    }

    void Chain::dispatch( const vkg::Pipeline& pipeline, unsigned x, unsigned y, unsigned z )
    {
      data().record( false ) ;
      data().has_record = true ;
      data().mutex.lock() ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.bind    ( pipeline              ) ;
        data().cmd.bind    ( pipeline.descriptor() ) ;
        data().cmd.dispatch( x, y, z               ) ;
        data().cmd.advance () ;
      }

      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
    }
    
    void Chain::end()
    {
      if( data().cmd.recording() )
      {
        data().mutex.lock() ;
        
        for( unsigned index = 0; index < data().num_cmd; index++ )
        {
          data().cmd.stop   () ;
          data().cmd.advance() ;
        }
        
        data().cmd.setActive( data().current ) ;
        
        data().has_record = true ;
        data().mutex.unlock() ;
      }
    }
    
    void Chain::memoryBarrier( const vkg::Buffer& src, const vkg::Buffer& dst )
    {
      const auto src_flag  = vk::PipelineStageFlagBits::eAllCommands ;
      const auto dst_flag  = vk::PipelineStageFlagBits::eAllCommands ;
      const auto dep_flags = vk::DependencyFlags()                   ;
      
      vk::BufferMemoryBarrier barrier ;
      
      dst.buffer() ;
      barrier.setBuffer       ( src.buffer()                     ) ;
      barrier.setSize         ( VK_WHOLE_SIZE                    ) ;
      barrier.setSrcAccessMask( vk::AccessFlagBits::eMemoryWrite ) ;
      barrier.setDstAccessMask( vk::AccessFlagBits::eMemoryRead  ) ;
      
      data().mutex.lock() ;
      data().record() ;
      
      auto iter = data().buffer_barriers.end()                                                      ;
           iter = data().buffer_barriers.insert( iter, { data().buffer_barriers.size(), barrier } ) ;

      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.buffer().pipelineBarrier( src_flag, dst_flag, dep_flags, 0, nullptr, 1, &iter->second, 0, nullptr ) ;
        data().cmd.advance() ;
      }

      data().dirty = true ;
      data().cmd.setActive( data().current ) ;
      data().mutex.unlock() ;
    }
    
    void Chain::pipelineBarrier( nyx::GPUStages src, nyx::GPUStages dst )
    {
      auto m_src = Vulkan::convert( src ) ;
      auto m_dst = Vulkan::convert( dst ) ;

      const auto dep_flags = vk::DependencyFlags() ;
      data().mutex.lock() ;
      data().record() ;
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.buffer().pipelineBarrier( m_src, m_dst, dep_flags, 0, nullptr, 0, nullptr, 0, nullptr ) ;
        data().cmd.advance() ;
      }

      data().dirty = true ;
      data().cmd.setActive( data().current ) ;
      data().mutex.unlock() ;
    }
    
    void Chain::memoryBarrier( const vkg::Buffer& src, const vkg::Image& dst )
    {
      data().mutex.lock() ;
      data().record() ;

      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.barrier( src, dst ) ;
        data().cmd.advance() ;
      }
      
      data().dirty = true ;
      data().cmd.setActive( data().current ) ;
      data().mutex.unlock() ;
    }
    
    const vk::Semaphore& Chain::signal() const
    {
      return data().signal ;
    }
    
    void Chain::setWait( const vk::Semaphore& wait )
    {
      data().wait = wait ;
    }

    void Chain::pushBase( const Pipeline& pipeline, const void* value, unsigned byte_size, unsigned offset )
    {
      data().mutex.lock() ;
      data().record( data().pass != nullptr ) ;
      
      for( unsigned index = 0; index < data().num_cmd; index++ )
      {
        data().cmd.bind( pipeline                             ) ;
        data().cmd.pushConstantBase( value, byte_size, offset ) ;
        data().cmd.advance() ;
      }

      data().cmd.setActive( data().current ) ;
      data().dirty = true ;
      data().mutex.unlock() ;
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
