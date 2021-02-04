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
#include "Vulkan.h"
#include <vulkan/vulkan.hpp>
#include <limits>
#include <mutex>
#include <map>

namespace nyx
{
  namespace vkg
  {
    /** Static mutex map for whenever queues are accessed.
     */
    static std::unordered_map<vk::Queue, std::mutex> mutex_map ;
    
    /** Structure to encompass a Queue's internal data.
     */
    struct QueueData
    {
      vk::Queue        queue  ; ///< The underlying vulkan queue.
      nyx::vkg::Device device ; ///< The device associated with this queue.
      vk::SubmitInfo   submit ; ///< The submit structure created here for cacheing.
      vk::QueueFlags   mask   ; ///< The ID associated with this queue.
      unsigned         family ; ///< The queue family associated with this queue.
      
      /** Default constructor.
       */
      QueueData() ;
    };

    QueueData::QueueData()
    {
      this->family = UINT32_MAX ;
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
      return this->valid() ;
    }
    
    Queue& Queue::operator=( const Queue& queue )
    {
      *this->queue_data = *queue.queue_data ;
      
      return *this ;
    }
    
    bool Queue::valid() const
    {
      if( data().queue && data().family != UINT32_MAX )
      {
        return true ;
      }

      return false ;
    }
    
    bool Queue::graphics() const
    {
      if( data().mask & ::vk::QueueFlagBits::eGraphics ) return true ;
      return false ;
    }
    
    bool Queue::compute() const
    {
      if( data().mask & ::vk::QueueFlagBits::eCompute ) return true ;
      return false ;
    }
    
    bool Queue::present() const
    {
      if( data().mask & ::vk::QueueFlagBits::eGraphics ) return true ;
      return false ;
    }
    
    unsigned Queue::family() const
    {
      return data().family ;
    }
    
    void Queue::wait() const
    {
      data().queue.waitIdle() ;
    }

    const vk::Queue& Queue::queue() const
    {
      return data().queue ;
    }

    unsigned Queue::device() const
    {
      return data().device ;
    }
    
    void Queue::submit( const nyx::vkg::CommandBuffer& cmd_buff, bool sync )
    {
      vk::FenceCreateInfo fence_info  ;
      vk::Fence           fence       ;
      unsigned            fence_count ;
      data().submit = vk::SubmitInfo() ;
      
      data().submit.setCommandBufferCount( cmd_buff.size()    ) ;
      data().submit.setPCommandBuffers   ( cmd_buff.pointer() ) ;
      data().submit.setWaitSemaphoreCount( 0 ) ;
      data().submit.setSignalSemaphoreCount( 0 ) ;
      
      fence_count = 0 ;
      if( sync )
      {
        fence       = data().device.device().createFence( fence_info, nullptr ) ;
        fence_count = 1                                                         ;
      }

      if( cmd_buff.level() == nyx::vkg::CommandBuffer::Level::Primary )
      {
        mutex_map[ data().queue ].lock() ;
        vkg::Vulkan::add( data().queue.submit( 1, &data().submit, fence ) ) ;

        // No synchronization given, must wait.
        vkg::Vulkan::add( data().device.device().waitForFences( fence_count, &fence, true, UINT64_MAX ) ) ;
        mutex_map[ data().queue ].unlock() ;
      }
      
      if( sync ) data().device.device().destroy( fence ) ;
    }
    
    void Queue::submit( const nyx::vkg::CommandBuffer& cmd_buff, const nyx::vkg::Synchronization& sync )
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

      if( cmd_buff.level() == nyx::vkg::CommandBuffer::Level::Primary )
      {
        mutex_map[ data().queue ].lock() ;
        vkg::Vulkan::add( data().queue.submit( 1, &data().submit, sync.signalFence() ) ) ;
        mutex_map[ data().queue ].unlock() ;
      }
    }
    
    unsigned Queue::submit( const nyx::vkg::Swapchain& swapchain, unsigned img_index )
    {
      vk::PresentInfoKHR info ;
      
      info.setPImageIndices     ( &img_index             ) ;
      info.setSwapchainCount    ( 1                      ) ;
      info.setPSwapchains       ( &swapchain.swapchain() ) ;
      info.setWaitSemaphoreCount( 0                      ) ;
      info.setPWaitSemaphores   ( 0                      ) ;
      
      mutex_map[ data().queue ].lock() ;
      auto result = data().queue.presentKHR( &info ) ;
      mutex_map[ data().queue ].unlock() ;
      
      return static_cast<unsigned>( Vulkan::convert( result ) ) ;
    }

    unsigned Queue::submit( const nyx::vkg::Swapchain& swapchain, unsigned img_index, const nyx::vkg::Synchronization& sync )
    {
      vk::PresentInfoKHR info ;
      
      info.setPImageIndices     ( &img_index             ) ;
      info.setSwapchainCount    ( 1                      ) ;
      info.setPSwapchains       ( &swapchain.swapchain() ) ;
      info.setWaitSemaphoreCount( sync.numWaitSems()     ) ;
      info.setPWaitSemaphores   ( sync.waits()           ) ;
      
      mutex_map[ data().queue ].lock() ;
      auto result = data().queue.presentKHR( &info ) ;
      mutex_map[ data().queue ].unlock() ;
      
      return static_cast<unsigned>( Vulkan::convert( result ) ) ;
    }
    
    void Queue::submit( const vk::CommandBuffer& cmd_buff )
    {
      static const vk::Fence dummy ;

      data().submit = vk::SubmitInfo() ;
      
      data().submit.setCommandBufferCount  ( 1         ) ;
      data().submit.setPCommandBuffers     ( &cmd_buff ) ;

      mutex_map[ data().queue ].lock() ;
      vkg::Vulkan::add( data().queue.submit( 1, &data().submit, dummy ) ) ;

      // No synchronization given, must wait.
      data().queue.waitIdle() ; 
      mutex_map[ data().queue ].unlock() ;
    }
    
    void Queue::submit( const vk::CommandBuffer& cmd_buff, const nyx::vkg::Synchronization& sync )
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

      mutex_map[ data().queue ].lock() ;
      vkg::Vulkan::add( data().queue.submit( 1, &data().submit, sync.signalFence() ) ) ;
      mutex_map[ data().queue ].unlock() ;
    }

    void Queue::initialize( const nyx::vkg::Device& device, const vk::Queue& queue, unsigned queue_family, unsigned mask )
    {
      data().device = device                              ;
      data().queue  = queue                               ;
      data().family = queue_family                        ;
      data().mask   = static_cast<vk::QueueFlags>( mask ) ;
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
