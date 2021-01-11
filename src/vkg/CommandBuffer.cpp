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

/* 
 * File:   CommandBuffer.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 1:42 PM
 */

#include "CommandBuffer.h"
#include "Queue.h"
#include "Device.h"
#include "RenderPass.h"
#include <vulkan/vulkan.hpp>
#include <map>
#include <vector>

namespace kgl
{
  namespace vkg
  {
    typedef unsigned QueueFamily ;
    typedef std::map<QueueFamily, vk::CommandPool> PoolMap ;
    static PoolMap pool_map ;

    struct CommandBufferData
    {
      typedef std::vector<vk::CommandBuffer> CmdBuffers ;
      
      kgl::vkg::Queue            queue               ;
      vk::CommandBufferBeginInfo begin_info          ;
      CommandBuffer::Level       level               ;
      CmdBuffers                 cmd_buffers         ;
      bool                       started_render_pass ;
      
      /** Default constructor.
       */
      CommandBufferData() ;
      
      /** Method to retrieve the map of command pools from a queue family.
       * @note creates pool if it is not found.
       * @param queue_family
       * @return A Reference to the created Command Pool.
       */
      vk::CommandPool& pool( QueueFamily queue_family ) ;
    };
    
    
    CommandBufferData::CommandBufferData()
    {
      this->level               = CommandBuffer::Level::Primary ;
      this->started_render_pass = false                         ;
    }
    
    vk::CommandPool& CommandBufferData::pool( QueueFamily queue_family )
    {
      const PoolMap::iterator          iter = pool_map.find( queue_family ) ;
      const vk::CommandPoolCreateFlags flags ; //= vk::CommandPoolCreateFlagBits::eResetCommandBuffer ; // TODO make this configurable.
      const vk::Device                 device = this->queue.device().device() ;

      vk::CommandPoolCreateInfo info ;
      vk::CommandPool           pool ;

      if( iter == pool_map.end() )
      {
        info.setFlags           ( flags        ) ;
        info.setQueueFamilyIndex( queue_family ) ;
        
        return pool_map.insert( iter, { queue_family, device.createCommandPool( info, nullptr ) } )->second ;
      }
      
      return iter->second ;
    }

    CommandBuffer::CommandBuffer()
    {
      this->cmd_data = new CommandBufferData() ;
    }

    CommandBuffer::CommandBuffer( const CommandBuffer& cmd )
    {
      this->cmd_data = new CommandBufferData() ;
      *this = cmd ;
    }

    CommandBuffer::~CommandBuffer()
    {
      delete this->cmd_data ;
    }

    CommandBuffer& CommandBuffer::operator=( const CommandBuffer& cmd )
    {
      *this->cmd_data = *cmd.cmd_data ;
      
      return *this ;
    }

    void CommandBuffer::initialize( const kgl::vkg::Queue& queue, unsigned count, CommandBuffer::Level level ) 
    {
      vk::CommandBufferAllocateInfo info      ;
      vk::CommandBufferLevel        cmd_level ;
      vk::CommandPool               pool      ;
      vk::Device                    device    ;
      data().queue = queue ;
      data().level = level ;
      
      device       = queue.device().device()                                                                         ;
      pool         = data().pool( data().queue.family() )                                                            ;
      cmd_level    = level == Level::Primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary ;
      
      info.setCommandBufferCount( count     ) ;
      info.setLevel             ( cmd_level ) ;
      info.setCommandPool       ( pool      ) ;
      
      data().cmd_buffers.resize( count ) ;
      device.allocateCommandBuffers( &info, data().cmd_buffers.data() ) ;
    }

    void CommandBuffer::combine( const CommandBuffer& cmd )
    {
      if( data().level == Level::Primary && cmd.data().level == Level::Secondary )
      {
        ; //Combine
      }
    }

    unsigned CommandBuffer::size() const
    {
      return data().cmd_buffers.size() ;
    }

    const kgl::vkg::Queue& CommandBuffer::queue() const
    {
      return data().queue ;
    }

    const kgl::vkg::Device& CommandBuffer::device() const
    {
      return data().queue.device() ;
    }
    
    CommandBuffer::Level CommandBuffer::level() const
    {
      return data().level ;
    }

    vk::CommandBuffer& CommandBuffer::buffer( unsigned idx )
    {
      static vk::CommandBuffer dummy ;
      if( idx < data().cmd_buffers.size() ) return data().cmd_buffers[ idx ] ;
      
      return dummy ;
    }
    
    const vk::CommandBuffer* CommandBuffer::pointer() const
    {
      return data().cmd_buffers.data() ;
    }

//    void CommandBuffer::draw( const kgl::vkg::Buffer& buffer, unsigned offset )
//    {
//    
//    }
//
//    void CommandBuffer::drawInstanced( const kgl::vkg::Buffer& buffer, unsigned instance_count, unsigned offset, unsigned first )
//    {
//    
//    }
//
//    void CommandBuffer::drawIndexed( const kgl::vkg::Buffer& indices, const kgl::vkg::Buffer& vertices )
//    {
//    
//    }

    void CommandBuffer::record( const kgl::vkg::RenderPass& render_pass )
    {
      const vk::SubpassContents flags = vk::SubpassContents::eInline ;

      vk::RenderPassBeginInfo info ;
      info.setClearValueCount( 1                          ) ;
      info.setPClearValues   ( &render_pass.clearColors() ) ;
      info.setRenderArea     ( render_pass.area()         ) ;
      info.setRenderPass     ( render_pass.pass()         ) ;
      
      
      for( auto &cmd_buff : data().cmd_buffers )
      {
        cmd_buff.begin          ( &data().begin_info ) ;
        cmd_buff.beginRenderPass( &info, flags       ) ;
      }
      
      data().started_render_pass = true ;
    }

    void CommandBuffer::record()
    {
      for( auto &cmd_buff : data().cmd_buffers )
      {
        cmd_buff.begin( &data().begin_info ) ;
      }
    }

    void CommandBuffer::stop()
    {
      for( auto &cmd_buff : data().cmd_buffers )
      {
        cmd_buff.end() ;
        
        if( data().started_render_pass )
        {
          cmd_buff.endRenderPass() ;
        }
      }
      data().started_render_pass = false ;
    }

    void CommandBuffer::reset()
    {
      const vk::CommandPool pool   = data().pool( data().queue.family() ) ;
      const vk::Device      device = data().queue.device().device()       ;
      
      device.freeCommandBuffers( pool, data().cmd_buffers.size(), data().cmd_buffers.data() ) ;
      data().cmd_buffers.clear() ;
    }

    CommandBufferData& CommandBuffer::data()
    {
      return *this->cmd_data ;
    }

    const CommandBufferData& CommandBuffer::data() const
    {
      return *this->cmd_data ;
    }
  }
}