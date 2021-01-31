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
namespace nyx
{
  namespace vkg
  {
    static const vk::Semaphore dummy ; ///< A Dummy semaphore to return on bad accesses.
    
    /** Structure containing a vkg::synchronization's internal data.
     */
    struct SynchronizationData
    {
      typedef std::vector<vk::Semaphore> SemList   ; ///< TODO
      typedef std::vector<vk::Fence    > FenceList ; ///< TODO
      
      vk::Device         device            ; ///< TODO
      vk::PhysicalDevice p_device          ; ///< TODO
      vk::Fence          signal_fence      ; ///< TODO
      SemList            signal_sems       ; ///< TODO
      FenceList          wait_fences       ; ///< TODO
      SemList            wait_sems         ; ///< TODO
      bool               should_make_fence ;

      /** Default constructor.
       */
      SynchronizationData() ;
    };
    
    SynchronizationData::SynchronizationData()
    {
      this->should_make_fence = true ;
    }

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
    
    void Synchronization::setMakeFence( bool value )
    {
      data().should_make_fence = value ;
    }
    
    void Synchronization::initialize( const nyx::vkg::Device& device, unsigned num_sems )
    {
      vk::SemaphoreCreateInfo sem_info   ;
      vk::FenceCreateInfo     fence_info ;

      data().device   = device.device()         ;
      data().p_device = device.physicalDevice() ;
      
      data().signal_sems  .resize( num_sems   ) ;
      
      fence_info.setFlags( vk::FenceCreateFlagBits::eSignaled ) ;
      
      for( auto &sem : data().signal_sems )
      {
        sem = data().device.createSemaphore( sem_info, nullptr ) ;
      }
      
      if( data().should_make_fence )
      {
        data().signal_fence = data().device.createFence( fence_info, nullptr ) ;
      }
    }
    
    void Synchronization::resetFence()
    {
      data().device.resetFences( 1, &data().signal_fence ) ;
    }

    void Synchronization::waitOn( const nyx::vkg::Synchronization& sync )
    {
      for( auto &sem : sync.data().signal_sems )
      {
        data().wait_sems.push_back( sem ) ;
      }
      
      if( sync.data().signal_fence ) data().wait_fences.push_back( sync.data().signal_fence ) ;
    }

    unsigned Synchronization::numFences() const
    {
      return 1 ;
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

    const vk::Fence& Synchronization::signalFence() const
    {
      return data().signal_fence ;
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
      return &data().signal_fence ;
    }
    
    void Synchronization::waitOnFences()
    {
      data().device.waitForFences( 1, &data().signal_fence, true, UINT64_MAX ) ;
      data().device.resetFences  ( 1, &data().signal_fence                   ) ;
    }
    
    void Synchronization::clear()
    {
      data().wait_fences.clear() ;
      data().wait_sems  .clear() ;
    }
    
    void Synchronization::reset()
    {
      for( auto& sem : data().signal_sems )
      {
        data().device.destroy( sem ) ;
      }

      data().device.destroy( data().signal_fence ) ;
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

