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
 * File:   Synchronization.cpp
 * Author: Jordan Hendl
 * 
 * Created on January 8, 2021, 7:17 PM
 */

#include "Synchronization.h"
#include "Device.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <limits.h>
namespace kgl
{
  namespace vkg
  {
    static const vk::Semaphore dummy ;
    struct SynchronizationData
    {
      typedef std::vector<vk::Semaphore> SemList   ;
      typedef std::vector<vk::Fence    > FenceList ;
      
      vk::Device         device        ;
      vk::PhysicalDevice p_device      ;
      FenceList          signal_fences ;
      SemList            signal_sems   ;
      FenceList          wait_fences   ;
      SemList            wait_sems     ;
      
      /** Default constructor.
       */
      SynchronizationData() = default ;
    };
    
    Synchronization::Synchronization()
    {
      this->sync_data = new SynchronizationData() ;
    }

    Synchronization::Synchronization( const Synchronization& sync )
    {
      this->sync_data = new SynchronizationData() ;
    
      *this = sync ;
    }

    Synchronization::~Synchronization()
    {
      delete this->sync_data ;
    }

    Synchronization& Synchronization::operator=( const Synchronization& sync )
    {
      *this->sync_data = *sync.sync_data ;
      
      return *this ;
    }

    const vk::Device& Synchronization::device() const
    {
      return data().device ;
    }

    const vk::PhysicalDevice& Synchronization::physicalDevice() const
    {
      return data().p_device ;
    }

    void Synchronization::initialize( const kgl::vkg::Device& device, unsigned num_sems, unsigned num_fences )
    {
      vk::SemaphoreCreateInfo sem_info   ;
      vk::FenceCreateInfo     fence_info ;

      data().device   = device.device()         ;
      data().p_device = device.physicalDevice() ;
      
      data().signal_sems  .resize( num_sems   ) ;
      data().signal_fences.resize( num_fences ) ;
      
      fence_info.setFlags( vk::FenceCreateFlagBits::eSignaled ) ;
      
      for( auto &sem : data().signal_sems )
      {
        sem = data().device.createSemaphore( sem_info, nullptr ) ;
      }
      
      for( auto &fence : data().signal_fences )
      {
        fence = data().device.createFence( fence_info, nullptr ) ;
      }
      
      data().device.resetFences( data().signal_fences.size(), data().signal_fences.data() ) ;
    }

    void Synchronization::waitOn( const kgl::vkg::Synchronization& sync )
    {
      for( auto &sem : sync.data().signal_sems )
      {
        data().wait_sems.push_back( sem ) ;
      }
      
      for( auto &fence : sync.data().signal_fences )
      {
        data().wait_fences.push_back( fence ) ;
      }
    }

    unsigned Synchronization::numFences() const
    {
      return data().signal_fences.size() ;
    }

    unsigned Synchronization::numSignals() const
    {
      return data().signal_sems.size() ;
    }

    unsigned Synchronization::numWaitSems() const
    {
      return data().wait_sems.size() ;
    }

    const vk::Semaphore& Synchronization::signal( unsigned idx ) const
    {
      if( idx < data().signal_sems.size() ) return data().signal_sems[ idx ] ;
      
      return dummy ;
    }

    const vk::Semaphore& Synchronization::wait( unsigned idx ) const
    {
      if( idx < data().wait_sems.size() ) return data().wait_sems[ idx ] ;
      
      return dummy ;
    }

    const vk::Fence& Synchronization::signalFence( unsigned idx ) const
    {
      static const vk::Fence dummy ;
      if( idx < data().signal_fences.size() ) return data().signal_fences[ idx ] ;
      
      return dummy ;
    }

    const vk::Fence& Synchronization::waitFence( unsigned idx ) const
    {
      static const vk::Fence dummy ;
      if( idx < data().wait_fences.size() ) return data().wait_fences[ idx ] ;
      
      return dummy ;
    }

    const vk::Semaphore* Synchronization::signals() const
    {
      return data().signal_sems.data() ;
    }

    const vk::Semaphore* Synchronization::waits() const
    {
      return data().wait_sems.data() ;
    }

    const vk::Fence* Synchronization::waitFences() const
    {
      return data().wait_fences.data() ;
    }

    const vk::Fence* Synchronization::signalFences() const
    {
      return data().signal_fences.data() ;
    }
    
    void Synchronization::wait()
    {
      data().device.waitForFences( data().wait_fences.size(), data().wait_fences.data(), true, UINT64_MAX ) ;
    }
    
    void Synchronization::clear()
    {
      data().wait_fences.clear() ;
      data().wait_sems  .clear() ;
    }

    SynchronizationData& Synchronization::data()
    {
      return *this->sync_data ;
    }

    const SynchronizationData& Synchronization::data() const
    {
      return *this->sync_data ;
    }
  }
}

