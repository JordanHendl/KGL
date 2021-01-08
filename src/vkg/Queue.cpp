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
#include "Device.h"
#include <vulkan/vulkan.hpp>
#include <limits>

namespace kgl
{
  namespace vkg
  {
    struct QueueData
    {
      vk::Queue        queue  ;
      kgl::vkg::Device device ;
      unsigned         family ;
      unsigned         id     ;
      
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

    const vk::Queue& Queue::queue() const
    {
      return data().queue ;
    }

    const kgl::vkg::Device& Queue::device() const
    {
      return data().device ;
    }

    void Queue::submit( const vk::SubmitInfo& info )
    {
      static vk::Fence dummy ;
      data().queue.submit( 1, &info, dummy ) ;
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
