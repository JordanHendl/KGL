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
 * File:   Queue.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 6:11 PM
 */

#include "Queue.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "Synchronization.h"
#include <vulkan/vulkan.hpp>
#include <limits>
#include <mutex>
#include <map>

namespace kgl
{
  namespace vkg
  {
    /** Static mutex map for whenever queues are accessed.
     */
    static std::map<unsigned, std::mutex> mutex_map ;
    
    /** Structure to encompass a Queue's internal data.
     */
    struct QueueData
    {
      vk::Queue        queue  ; ///< The underlying vulkan queue.
      kgl::vkg::Device device ; ///< The device associated with this queue.
      vk::SubmitInfo   submit ; ///< The submit structure created here for cacheing.
      unsigned         family ; ///< The queue family associated with this queue.
      unsigned         id     ; ///< The ID associated with this queue.
      
      /** Default constructor.
       */
      QueueData() ;
    };

    QueueData::QueueData()
    {
      this->family = UINT32_MAX ;
      this->id     = UINT32_MAX ;
    }

    Queue::Queue()
    {
      this->queue_data = new QueueData() ;
    }

    Queue::Queue( const Queue& queue )
    {
      this->queue_data = new QueueData() ;
      
      *this = queue ;
    }
 
    Queue::~Queue()
    {
      delete this->queue_data ;
    }
    
    Queue::operator bool() const
    {
      if( data().queue && data().family != UINT32_MAX )
      {
        return true ;
      }

      return false ;
    }
    
    Queue& Queue::operator=( const Queue& queue )
    {
      *this->queue_data = *queue.queue_data ;
      
      return *this ;
    }
    
    unsigned Queue::family() const
    {
      return data().family ;
    }

    const vk::Queue& Queue::queue() const
    {
      return data().queue ;
    }

    const kgl::vkg::Device& Queue::device() const
    {
      return data().device ;
    }
    
    void Queue::submit( const kgl::vkg::CommandBuffer& cmd_buff )
    {
      static const vk::Fence dummy ;

      data().submit = vk::SubmitInfo() ;
      
      data().submit.setCommandBufferCount( cmd_buff.size()    ) ;
      data().submit.setPCommandBuffers   ( cmd_buff.pointer() ) ;
      
      if( cmd_buff.level() == kgl::vkg::CommandBuffer::Level::Primary )
      {
        mutex_map[ data().family ].lock() ;
        data().queue.submit( 1, &data().submit, dummy ) ;

        // No synchronization given, must wait.
        data().queue.waitIdle() ; 
        mutex_map[ data().family ].unlock() ;
      }
    }
    
    void Queue::submit( const kgl::vkg::CommandBuffer& cmd_buff, const kgl::vkg::Synchronization& sync )
    {
      static const std::vector<::vk::PipelineStageFlags> flags( 100, ::vk::PipelineStageFlagBits::eAllCommands ) ;

      data().submit = vk::SubmitInfo() ;
      
      data().submit.setCommandBufferCount  ( cmd_buff.size()    ) ;
      data().submit.setPCommandBuffers     ( cmd_buff.pointer() ) ;
      data().submit.setPSignalSemaphores   ( sync.signals()     ) ;
      data().submit.setSignalSemaphoreCount( sync.numSignals()  ) ;
      data().submit.setWaitSemaphoreCount  ( sync.numWaitSems() ) ;
      data().submit.setPWaitSemaphores     ( sync.waits()       ) ;
      data().submit.setPWaitDstStageMask   ( flags.data()       ) ;

      if( cmd_buff.level() == kgl::vkg::CommandBuffer::Level::Primary )
      {
        mutex_map[ data().family ].lock() ;
        data().queue.submit( 1, &data().submit, sync.signalFence() ) ;
        mutex_map[ data().family ].unlock() ;
      }
    }
    
    void Queue::submit( const kgl::vkg::Swapchain& swapchain, unsigned img_index, const kgl::vkg::Synchronization& sync )
    {
      vk::PresentInfoKHR info ;
      
      info.setPImageIndices     ( &img_index             ) ;
      info.setSwapchainCount    ( 1                      ) ;
      info.setPSwapchains       ( &swapchain.swapchain() ) ;
      info.setWaitSemaphoreCount( sync.numWaitSems()     ) ;
      info.setPWaitSemaphores   ( sync.waits()           ) ;
      
      mutex_map[ data().family ].lock() ;
      data().queue.presentKHR( &info ) ;
      mutex_map[ data().family ].unlock() ;
    }
    
    void Queue::submit( const vk::CommandBuffer& cmd_buff )
    {
      static const vk::Fence dummy ;

      data().submit = vk::SubmitInfo() ;
      
      data().submit.setCommandBufferCount  ( 1         ) ;
      data().submit.setPCommandBuffers     ( &cmd_buff ) ;

      mutex_map[ data().family ].lock() ;
      data().queue.submit( 1, &data().submit, dummy ) ;

      // No synchronization given, must wait.
      data().queue.waitIdle() ; 
      mutex_map[ data().family ].unlock() ;
    }
    
    void Queue::submit( const vk::CommandBuffer& cmd_buff, const kgl::vkg::Synchronization& sync )
    {
      static constexpr vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eAllCommands ;
      
      data().submit = vk::SubmitInfo() ;
      data().submit.setCommandBufferCount  ( 1                  ) ;
      data().submit.setPCommandBuffers     ( &cmd_buff          ) ;
      data().submit.setPSignalSemaphores   ( sync.signals()     ) ;
      data().submit.setSignalSemaphoreCount( sync.numSignals()  ) ;
      data().submit.setWaitSemaphoreCount  ( sync.numWaitSems() ) ;
      data().submit.setPWaitSemaphores     ( sync.waits()       ) ;
      data().submit.setPWaitDstStageMask   ( &flags             ) ;

      mutex_map[ data().family ].lock() ;
      data().queue.submit( 1, &data().submit, sync.signalFence() ) ;
      mutex_map[ data().family ].unlock() ;
    }

    void Queue::initialize( const kgl::vkg::Device& device, const vk::Queue& queue, unsigned queue_family, unsigned queue_id )
    {
      data().device = device       ;
      data().queue  = queue        ;
      data().family = queue_family ;
      data().id     = queue_id     ;
    }

    QueueData& Queue::data()
    {
      return *this->queue_data ;
    }

    const QueueData& Queue::data() const
    {
      return *this->queue_data ;
    }
  }
}
