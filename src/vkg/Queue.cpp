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

#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS


#include "Queue.h"
#include "Device.h"
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
      vk::Fence        fence  ;
      vk::Queue        queue  ; ///< The underlying vulkan queue.
      unsigned         dev_id ;
      std::mutex      *mutex  ; 
      vk::Device       device ; ///< The device associated with this queue.
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
      Vulkan::deviceLock( data().dev_id ) ;
      vkg::Vulkan::add( data().queue.waitIdle() ) ; 
      Vulkan::deviceUnlock( data().dev_id ) ;
    }

    const vk::Queue& Queue::queue() const
    {
      return data().queue ;
    }

    unsigned Queue::device() const
    {
      return data().dev_id ;
    }
    
    bool Queue::initialized() const
    {
      return data().queue ;
    }

    void Queue::submit( const nyx::vkg::CommandBuffer& cmd_buff )
    {
      const auto cmd = cmd_buff.buffer() ;
      data().submit = vk::SubmitInfo() ;
      data().submit.setCommandBufferCount  ( 1                  ) ;
      data().submit.setPCommandBuffers     ( &cmd               ) ;
      data().submit.setWaitSemaphoreCount  ( 0                  ) ;
      data().submit.setSignalSemaphoreCount( 0                  ) ;
      
      vk::Fence fence = cmd_buff.fence() ;

      if( cmd_buff.level() == nyx::vkg::CommandBuffer::Level::Primary )
      {
        Vulkan::deviceLock( data().dev_id ) ;
        vkg::Vulkan::add( data().queue.submit( 1, &data().submit, fence ) ) ;
        Vulkan::deviceUnlock( data().dev_id ) ;
      }
      
      cmd_buff.advance() ;
    }
    
    void Queue::submit( const nyx::vkg::CommandBuffer& cmd_buff, const vk::Semaphore& wait_sem, const vk::Semaphore& signal_sem )
    {
      static const vk::PipelineStageFlags flag = vk::PipelineStageFlagBits::eAllCommands ;
      const auto cmd = cmd_buff.buffer() ;
      
      data().submit = vk::SubmitInfo() ;
      data().submit.setCommandBufferCount  ( 1                  ) ;
      data().submit.setPCommandBuffers     ( &cmd               ) ;
      data().submit.setWaitSemaphoreCount  ( wait_sem   ? 1 : 0 ) ;
      data().submit.setSignalSemaphoreCount( signal_sem ? 1 : 0 ) ;
      data().submit.setPWaitSemaphores     ( &wait_sem          ) ;
      data().submit.setPSignalSemaphores   ( &signal_sem        ) ;
      data().submit.setPWaitDstStageMask   ( &flag              ) ;
      vk::Fence fence = cmd_buff.fence() ;

      if( cmd_buff.level() == nyx::vkg::CommandBuffer::Level::Primary )
      {
        Vulkan::deviceLock( data().dev_id ) ;
        vkg::Vulkan::add( data().queue.submit( 1, &data().submit, fence ) ) ;
        Vulkan::deviceUnlock( data().dev_id ) ;
      }
      
      cmd_buff.advance() ;
    }
    
    void Queue::submit( const nyx::vkg::CommandBuffer& cmd_buff, const vk::Semaphore& signal_sem )
    {
      const auto cmd = cmd_buff.buffer() ;
      
      data().submit = vk::SubmitInfo() ;
      data().submit.setCommandBufferCount  ( 1                  ) ;
      data().submit.setPCommandBuffers     ( &cmd               ) ;
      data().submit.setWaitSemaphoreCount  ( 0                  ) ;
      data().submit.setSignalSemaphoreCount( 1                  ) ;
      data().submit.setPWaitSemaphores     ( nullptr            ) ;
      data().submit.setPSignalSemaphores   ( &signal_sem        ) ;
      vk::Fence fence = cmd_buff.fence() ;

      if( cmd_buff.level() == nyx::vkg::CommandBuffer::Level::Primary )
      {
        Vulkan::deviceLock( data().dev_id ) ;
        vkg::Vulkan::add( data().queue.submit( 1, &data().submit, fence ) ) ;
        Vulkan::deviceUnlock( data().dev_id ) ;
      }
      
      cmd_buff.advance() ;
    }
    
    unsigned Queue::submit( const nyx::vkg::Swapchain& swapchain, unsigned img_index )
    {
      vk::PresentInfoKHR info ;
      
      info.setPImageIndices     ( &img_index             ) ;
      info.setSwapchainCount    ( 1                      ) ;
      info.setPSwapchains       ( &swapchain.swapchain() ) ;
      info.setWaitSemaphoreCount( 0                      ) ;
      info.setPWaitSemaphores   ( 0                      ) ;
      
      Vulkan::deviceLock( data().dev_id ) ;
      auto result = data().queue.presentKHR( &info ) ;
      Vulkan::deviceUnlock( data().dev_id ) ;
      
      return static_cast<unsigned>( Vulkan::convert( result ) ) ;
    }

    unsigned Queue::submit( const nyx::vkg::Swapchain& swapchain, unsigned img_index, const vk::Semaphore& sync )
    {
      vk::PresentInfoKHR info ;
      
      info.setPImageIndices     ( &img_index             ) ;
      info.setSwapchainCount    ( 1                      ) ;
      info.setPSwapchains       ( &swapchain.swapchain() ) ;
      info.setWaitSemaphoreCount( 1                      ) ;
      info.setPWaitSemaphores   ( &sync                  ) ;
      
      Vulkan::deviceLock( data().dev_id ) ;
      auto result = data().queue.presentKHR( &info ) ;
      Vulkan::deviceUnlock( data().dev_id ) ;
      
      return static_cast<unsigned>( Vulkan::convert( result ) ) ;
    }
    
    void Queue::submit( const vk::CommandBuffer& cmd_buff )
    {
      static const vk::Fence dummy ;

      data().submit = vk::SubmitInfo() ;
      
      data().submit.setCommandBufferCount  ( 1         ) ;
      data().submit.setPCommandBuffers     ( &cmd_buff ) ;


      Vulkan::deviceLock( data().dev_id ) ;
      Vulkan::deviceLock( data().dev_id ) ;
      vkg::Vulkan::add( data().queue.submit( 1, &data().submit, dummy ) ) ;
      Vulkan::deviceUnlock( data().dev_id ) ;

      vkg::Vulkan::add( data().queue.waitIdle() ) ;
      // No synchronization given, must wait.
    }
    
    void Queue::initialize( const nyx::vkg::Device& device, const vk::Queue& queue, unsigned queue_family, unsigned mask )
    {
      vk::FenceCreateInfo fence_info  ;

      data().dev_id = device                                           ;
      data().device = device.device()                                  ;
      data().queue  = queue                                            ;
      data().family = queue_family                                     ;
      data().mask   = static_cast<vk::QueueFlags>( mask )              ;
      data().mutex  = &mutex_map[ queue ]                              ;
      
      auto result =  data().device.createFence( fence_info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      data().fence = result.value ;
      
      vkg::Vulkan::add( data().device.resetFences( 1, &data().fence ) ) ;
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
