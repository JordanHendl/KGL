/*
 * The MIT License
 *
 * Copyright 2020 jhendl.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   Device.cpp
 * Author: jhendl
 * 
 * Created on December 23, 2020, 8:03 PM
 */

#include "Device.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <limits>

namespace kgl
{
  namespace vkg
  {
    struct QueueFamilies
    {
      typedef unsigned                            Queue       ;
      typedef unsigned                            MaxCount    ;
      typedef std::map<Queue, std::vector<float>> PriorityMap ;
      
      std::vector<unsigned>     total_families  ;
      PriorityMap               priority_map    ;
      unsigned                  compute_family  ;
      unsigned                  graphics_family ;
      unsigned                  present_family  ;
      unsigned                  transfer_family ;
      
      /** Default constructor.
       */
      QueueFamilies() ;

      /** Method to retrieve whether all families have been found.
       * @return Whether or not all families have been found.
       */
      bool isComplete() const ;
    };

    struct DeviceData
    {
      typedef std::vector<std::string> ExtList        ;
      typedef std::vector<const char*> CharExtList    ;
      typedef std::vector<float>       Priorities     ;
      
      ::vk::Device                 gpu                 ;
      ::vk::PhysicalDevice         physical_device     ;
      ::vk::SurfaceKHR             surface             ;
      ::vk::PhysicalDeviceFeatures features            ;
      ExtList                      extension_list      ;
      QueueFamilies                queue_families      ;
      Priorities                   graphics_priorities ;
      Priorities                   compute_priorities  ;
      Priorities                   present_priorities  ;
      Priorities                   transfer_priorities ;

      /** Default Constructor
       */
      DeviceData() ;

      /** Copy operator
       */
      DeviceData& operator=( const DeviceData& data ) ;
      
      /** Method to generate the virtual Vulkan device.
       */
      void generateDevice() ;
      
      /** Method to find queue families of the input physical device for logical device creation.
       */
      void findQueueFamilies() ;
      
      /** Method to filter out extensions requested to only ones provided by the system.
       * @return The list of extensions requested that are present on the system.
       */
      CharExtList filterExtensions() const ;
    };

    QueueFamilies::QueueFamilies()
    {
      this->compute_family  = UINT32_MAX ;
      this->graphics_family = UINT32_MAX ;
      this->present_family  = UINT32_MAX ;
      this->transfer_family = UINT32_MAX ;
    }
    
    bool QueueFamilies::isComplete() const
    {
      return this->compute_family  != UINT32_MAX &&
             this->graphics_family != UINT32_MAX && 
             this->present_family  != UINT32_MAX && 
             this->transfer_family != UINT32_MAX   ;
    }
    
    DeviceData::DeviceData()
    {
      this->graphics_priorities = { 1.0f } ;
      this->compute_priorities  = { 1.0f } ;
      this->transfer_priorities = { 1.0f } ;
      this->present_priorities  = { 1.0f } ;
    }
    
    DeviceData& DeviceData::operator=(const DeviceData& data) 
    {
      this->gpu                 = data.gpu                 ;
      this->physical_device     = data.physical_device     ;
      this->surface             = data.surface             ;
      this->features            = data.features            ;
      this->extension_list      = data.extension_list      ;
      this->queue_families      = data.queue_families      ;
      this->graphics_priorities = data.graphics_priorities ;
      this->compute_priorities  = data.compute_priorities  ;
      this->present_priorities  = data.present_priorities  ;
      this->transfer_priorities = data.transfer_priorities ;
      
      return *this ;
    }

    void DeviceData::generateDevice()
    {
      typedef std::vector<::vk::DeviceQueueCreateInfo> CreateInfos ;
      
      ::vk::DeviceCreateInfo       info        ;
      DeviceData::CharExtList      ext_list    ;
      CreateInfos                  queue_infos ;
      unsigned                     queue_id    ;
      unsigned                     num_queues  ;
      
      ext_list = this->filterExtensions() ;
      queue_infos.resize( this->queue_families.total_families.size() ) ;

      for( unsigned i = 0; i < this->queue_families.total_families.size(); i++ )
      {
        auto priorities = this->queue_families.priority_map  [ queue_id ] ;
        queue_id        = this->queue_families.total_families[ i        ] ;
        num_queues      = priorities.size()                               ;

        queue_infos[ i ].setQueueFamilyIndex( queue_id          ) ;
        queue_infos[ i ].setQueueCount      ( num_queues        ) ;
        queue_infos[ i ].setPQueuePriorities( priorities.data() ) ;
      }
      
      info.setQueueCreateInfoCount   ( queue_infos.size() ) ;
      info.setPQueueCreateInfos      ( queue_infos.data() ) ;
      info.setEnabledExtensionCount  ( ext_list.size()    ) ;
      info.setPpEnabledExtensionNames( ext_list.data()    ) ;
      info.setPEnabledFeatures       ( &this->features    ) ;

      this->physical_device.createDevice( &info, nullptr, &this->gpu ) ;
    }
    
    void DeviceData::findQueueFamilies()
    {
      typedef std::vector<::vk::QueueFamilyProperties> FamilyPropList ;

      FamilyPropList list    ;
      unsigned       id      ;
      ::vk::Bool32   support ;

      list = this->physical_device.getQueueFamilyProperties() ;
      id   = 0                                                ;
      
      for( const auto& queue : list )
      {
        if( this->surface )
        {
          this->physical_device.getSurfaceSupportKHR( id, this->surface, &support ) ;
          
          if( support )
          {
            this->present_priorities.resize( std::min( static_cast<unsigned>( this->present_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.present_family     = id                       ;
            this->queue_families.priority_map[ id ] = this->present_priorities ;
          }
        }

        if( queue.queueFlags & ::vk::QueueFlagBits::eGraphics )
        {
            this->graphics_priorities.resize( std::min( static_cast<unsigned>( this->graphics_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.present_family     = id                        ;
            this->queue_families.priority_map[ id ] = this->graphics_priorities ;
        }
        
        if( queue.queueFlags & ::vk::QueueFlagBits::eCompute )
        {
            this->compute_priorities.resize( std::min( static_cast<unsigned>( this->compute_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.present_family     = id                       ;
            this->queue_families.priority_map[ id ] = this->compute_priorities ;
        }
        
        if( queue.queueFlags & ::vk::QueueFlagBits::eTransfer )
        {
            this->transfer_priorities.resize( std::min( static_cast<unsigned>( this->transfer_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.present_family     = id                        ;
            this->queue_families.priority_map[ id ] = this->transfer_priorities ;
        }
        
        this->queue_families.total_families.push_back( id ) ;

        if( this->queue_families.isComplete() )
        {
          return ;
        }
        
        id++ ;
      }
    }

    DeviceData::CharExtList DeviceData::filterExtensions() const
    {
      CharExtList                            list                 ;
      std::vector<::vk::ExtensionProperties> available_extentions ;
      
      available_extentions = this->physical_device.enumerateDeviceExtensionProperties() ;
      
      for( const auto& ext : available_extentions )
      {
        for( const auto& requested : this->extension_list )
        {
          if( std::string( ext.extensionName ) == std::string( requested ) )
          {
            list.push_back( ext.extensionName ) ;
          }
        }
      }
      
      return list ;
    }

    Device::Device()
    {
      this->device_data = new DeviceData() ;
    }

    Device::Device( const Device& src )
    {
      this->device_data = new DeviceData() ;
      
      *this = src ;
    }

    Device::~Device()
    {
      delete this->device_data ;
    }

    Device::operator const ::vk::Device&() const
    {
      return data().gpu ;
    }
    
    Device::operator ::vk::Device() const
    {
      return data().gpu ;
    }
    
    Device::operator const ::vk::Device&()
    {
      return data().gpu ;
    }
    
    Device::operator ::vk::Device()
    {
      return data().gpu ;
    }
    
    Device& Device::operator=( const Device& src )
    {
      *this->device_data = *src.device_data ;
      
      return *this ;
    }

    void Device::setSamplerAnisotropy( bool value )
    {
      data().features.setSamplerAnisotropy( value ) ;
    }
    
    void Device::setDualSrcBlend( bool value )
    {
      data().features.setDualSrcBlend( value ) ;
    }

    void Device::setRobustBufferAccess( bool value )
    {
      data().features.setRobustBufferAccess( value ) ;
    }

    void Device::initialize( const ::vk::PhysicalDevice& physical_device )
    {
      data().physical_device = physical_device ;
      
      data().findQueueFamilies() ;
      data().generateDevice   () ;
    }

    void Device::initialize( const ::vk::PhysicalDevice& physical_device, const ::vk::SurfaceKHR& surface )
    {
      data().physical_device = physical_device ;
      data().surface         = surface         ;
      
      this->initialize( physical_device ) ;
    }

    void Device::addExtension( const char* extension_name )
    {
      data().extension_list.push_back( extension_name ) ;
    }
    
    void Device::setGraphicsQueueCount( unsigned amt )
    {
      data().graphics_priorities.resize( amt ) ;
    }
    
    void Device::setComputeQueueCount( unsigned amt )
    {
      data().compute_priorities.resize( amt ) ;
    }
    
    void Device::setTransferQueueCount( unsigned amt )
    {
      data().transfer_priorities.resize( amt ) ;
    }
    
    void Device::setGraphicsPriority( unsigned queue_index, float priority )
    {
      if( queue_index < data().graphics_priorities.size() ) data().graphics_priorities[ queue_index ] = priority ;
    }
    
    void Device::setComputePriority( unsigned queue_index, float priority )
    {
      if( queue_index < data().compute_priorities.size() ) data().compute_priorities[ queue_index ] = priority ;
    }
    
    void Device::setTransferPriority( unsigned queue_index, float priority )
    {
      if( queue_index < data().transfer_priorities.size() ) data().transfer_priorities[ queue_index ] = priority ;
    }
    
    void Device::setPresentPriority( unsigned queue_index, float priority )
    {
      if( queue_index < data().present_priorities.size() ) data().present_priorities[ queue_index ] = priority ;
    }

    const ::vk::Device& Device::device() const
    {
      return data().gpu ;
    }
    
    const ::vk::PhysicalDevice& Device::physicalDevice() const
    {
      return data().physical_device ;
    }

    DeviceData& Device::data()
    {
      return *this->device_data ;
    }

    const DeviceData& Device::data() const
    {
      return *this->device_data ;
    }
  }
}

