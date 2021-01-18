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
 * File:   Device.cpp
 * Author: jhendl
 * 
 * Created on December 23, 2020, 8:03 PM
 */

#include "Device.h"
#include "Queue.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <limits>
#include <tuple>

namespace kgl
{
  namespace vkg
  {
    struct QueueCount
    {
      /** The vector of queues associated with this object.
       */
      std::vector<std::tuple<unsigned, vk::QueueFamilyProperties*>> queues ;
      
      /** The amount of queues allowed.
       */
      unsigned count ;
      
      /** Default constreuctor.
       */
      QueueCount() ;
      
      /** Method to retrieve a queue.
       * @return The family of the queue gotten.
       */
      unsigned getQueue() ;
      
      /** Method to retrieve the index of the input family.
       * @param family The family to retrieve an index of.
       * @return The index of the specified family.
       */
      unsigned index( unsigned family )    ;
    };
    
    /** Structure to manage vulkan queue families.
     */
    struct QueueFamilies
    {
      typedef unsigned                            Queue       ; ///< TODO
      typedef unsigned                            MaxCount    ; ///< TODO
      typedef std::map<Queue, std::vector<float>> PriorityMap ; ///< TODO
      
      std::vector<unsigned>                  total_families ; ///< TODO
      QueueCount                             graphics       ; ///< TODO
      QueueCount                             compute        ; ///< TODO
      QueueCount                             present        ; ///< TODO
      QueueCount                             transfer       ; ///< TODO
      PriorityMap                            priority_map   ; ///< TODO
      
      /** Default constructor.
       */
      QueueFamilies() = default ;
      
      /** Assignment operator.
       * @param family The object to assign to this one.
       * @return Reference after assignment.
       */
      QueueFamilies& operator=( const QueueFamilies& family ) ;
    };

    struct DeviceData
    {
      typedef std::vector<std::string    > List       ; ///< TODO
      typedef std::vector<kgl::vkg::Queue> QueueList  ; ///< TODO
      typedef std::vector<const char*    > CharList   ; ///< TODO
      typedef std::vector<float          > Priorities ; ///< TODO
      
      ::vk::Device                           gpu                 ; ///< TODO
      ::vk::PhysicalDevice                   physical_device     ; ///< TODO
      ::vk::SurfaceKHR                       surface             ; ///< TODO
      ::vk::PhysicalDeviceFeatures           features            ; ///< TODO
      std::vector<vk::QueueFamilyProperties> properties          ; ///< TODO
      List                                   extension_list      ; ///< TODO
      List                                   layer_list          ; ///< TODO
      QueueList                              queues              ; ///< TODO
      QueueFamilies                          queue_families      ; ///< TODO
      Priorities                             graphics_priorities ; ///< TODO
      Priorities                             compute_priorities  ; ///< TODO
      Priorities                             present_priorities  ; ///< TODO
      Priorities                             transfer_priorities ; ///< TODO

      /** Default Constructor
       */
      DeviceData() ;
      
      /** Assignment operator.
       * @return Reference to this object after assignment.
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
      List filterExtensions() const ;
      
      /** Method to filter out layers requested to only ones provided by the system.
       * @return The list of layers requested that are present on the system.
       */
      List filterLayers() const ;
      
      /** Method to convert a list of strings to list of char* for vulkan objects.
       * @param list The list to convert.
       * @return A List of const char* pointing to string list's strings.
       */
      CharList listToCharList( List& list ) ;
    };
    
    QueueCount::QueueCount()
    {
      this->count = 0 ;
    }

    unsigned QueueCount::index( unsigned family )
    {
      if( family != UINT32_MAX )
      {
        return this->count++ ;
      }
      return 0 ;
    }

    unsigned QueueCount::getQueue()
    {
      
      for( auto tup : this->queues )
      {
        auto family = std::get<0>( tup ) ;
        auto prop   = std::get<1>( tup ) ;

        if( prop->queueCount != 0 )
        {
          return family ;
        }
      }

      return UINT32_MAX ;
    }
    
    QueueFamilies& QueueFamilies::operator=( const QueueFamilies& family )
    {
      this->total_families = family.total_families ;
      this->graphics       = family.graphics       ;
      this->compute        = family.compute        ;
      this->present        = family.present        ;
      this->transfer       = family.transfer       ;
      this->priority_map   = family.priority_map   ;
      
      return *this ;
    }

    DeviceData::DeviceData()
    {
      this->graphics_priorities = { 1.0f } ;
      this->compute_priorities  = { 1.0f } ;
      this->transfer_priorities = { 1.0f } ;
      this->present_priorities  = { 1.0f } ;
    }
    
    DeviceData& DeviceData::operator=( const DeviceData& data )
    {
      this->gpu                 = data.gpu                 ;
      this->physical_device     = data.physical_device     ;
      this->surface             = data.surface             ;
      this->features            = data.features            ;
      this->properties          = data.properties          ;
      this->extension_list      = data.extension_list      ;
      this->queues              = data.queues              ;
      this->queue_families      = data.queue_families      ;
      this->graphics_priorities = data.graphics_priorities ;
      this->compute_priorities  = data.compute_priorities  ;
      this->present_priorities  = data.present_priorities  ;
      this->transfer_priorities = data.transfer_priorities ;
      
      return *this ;
    }

    DeviceData::List DeviceData::filterLayers() const
    {
      List                               list             ;
      std::vector<::vk::LayerProperties> available_layers ;
      
      available_layers = this->physical_device.enumerateDeviceLayerProperties() ;
        
      for( const auto& prop : available_layers )
      {
        for( const auto& requested : this->layer_list )
        {
          if( std::string( prop.layerName.data() ) == std::string( requested ) )
          {
            list.push_back( std::string( prop.layerName.data() ) ) ;
          }
        }
      }
      return list ;
    }

    DeviceData::CharList DeviceData::listToCharList( List& list )
    {
      DeviceData::CharList char_list ;
      
      for( auto &str : list )
      {
        char_list.push_back( str.c_str() ) ;
      }
      
      return char_list ;
    }
        
    void DeviceData::generateDevice()
    {
      typedef std::vector<::vk::DeviceQueueCreateInfo> CreateInfos ;
      
      ::vk::DeviceCreateInfo info            ;
      DeviceData::List       ext_list        ;
      DeviceData::List       layer_list      ;
      DeviceData::CharList   ext_list_char   ;
      DeviceData::CharList   layer_list_char ;
      CreateInfos            queue_infos     ;
      unsigned               queue_id        ;
      unsigned               num_queues      ;
      
      ext_list        = this->filterExtensions()           ;
      layer_list      = this->filterLayers()               ;
      ext_list_char   = this->listToCharList( ext_list )   ;
      layer_list_char = this->listToCharList( layer_list ) ;

      queue_infos.resize( this->queue_families.total_families.size() ) ;

      for( unsigned i = 0; i < this->queue_families.total_families.size(); i++ )
      {
        queue_id        = this->queue_families.total_families[ i        ] ;
        auto priorities = this->queue_families.priority_map  [ queue_id ] ;
        num_queues      = priorities.size()                               ;

        queue_infos[ i ].setQueueFamilyIndex( queue_id          ) ;
        queue_infos[ i ].setQueueCount      ( num_queues        ) ;
        queue_infos[ i ].setPQueuePriorities( priorities.data() ) ;
      }
      
      info.setQueueCreateInfoCount   ( queue_infos.size()     ) ;
      info.setPQueueCreateInfos      ( queue_infos.data()     ) ;
      info.setEnabledExtensionCount  ( ext_list_char.size()   ) ;
      info.setPpEnabledExtensionNames( ext_list_char.data()   ) ;
      info.setEnabledLayerCount      ( layer_list_char.size() ) ;
      info.setPEnabledLayerNames     ( layer_list_char        ) ;
      info.setPEnabledFeatures       ( &this->features        ) ;
      
      this->physical_device.createDevice( &info, nullptr, &this->gpu ) ;
    }
    
    void DeviceData::findQueueFamilies()
    {
      unsigned       id      ;
      ::vk::Bool32   support ;

      this->properties = this->physical_device.getQueueFamilyProperties() ;
      id   = 0                                                            ;
      
      for( auto& queue : this->properties )
      {
        if( this->surface )
        {
          this->physical_device.getSurfaceSupportKHR( id, this->surface, &support ) ;
          
          if( support )
          {
            this->present_priorities.resize( std::min( static_cast<unsigned>( this->present_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.present.queues.push_back( std::make_tuple( id, &queue ) ) ;
            this->queue_families.priority_map[ id ] = this->present_priorities             ;
          }
        }

        if( queue.queueFlags & ::vk::QueueFlagBits::eGraphics )
        {
            this->graphics_priorities.resize( std::min( static_cast<unsigned>( this->graphics_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.graphics.queues.push_back( std::make_tuple( id, &queue ) ) ;
            this->queue_families.priority_map[ id ] = this->graphics_priorities             ;
        }
        
        if( queue.queueFlags & ::vk::QueueFlagBits::eCompute )
        {
            this->compute_priorities.resize( std::min( static_cast<unsigned>( this->compute_priorities.size() ), queue.queueCount ) ) ;
            
            this->queue_families.compute.queues.push_back( std::make_tuple( id, &queue ) ) ;
            this->queue_families.priority_map[ id ] = this->compute_priorities             ;
        }
        
        if( queue.queueFlags & ::vk::QueueFlagBits::eTransfer )
        {
          this->transfer_priorities.resize( std::min( static_cast<unsigned>( this->transfer_priorities.size() ), queue.queueCount ) ) ;
            
          this->queue_families.transfer.queues.push_back( std::make_tuple( id, &queue ) ) ;
          this->queue_families.priority_map[ id ] = this->transfer_priorities             ;
        }
        
        this->queue_families.total_families.push_back( id ) ;

        id++ ;
      }
    }

    DeviceData::List DeviceData::filterExtensions() const
    {
      List                                   list                 ;
      std::vector<::vk::ExtensionProperties> available_extentions ;
      
      available_extentions = this->physical_device.enumerateDeviceExtensionProperties() ;
      
      for( const auto& ext : available_extentions )
      {
        for( const auto& requested : this->extension_list )
        {
          if( std::string( ext.extensionName.data() ) == std::string( requested ) )
          {
            list.push_back( std::string( ext.extensionName ) ) ;
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
    
    void Device::reset()
    {
      data().gpu.destroy( nullptr ) ;
    }
    
    void Device::addExtension( const char* extension_name )
    {
      data().extension_list.push_back( extension_name ) ;
    }
    
    void Device::addValidationLayer( const char* layer_name )
    {
      data().layer_list.push_back( layer_name ) ;
    }

    //TODO These getters for queues are very bad. Do better eventually.
    const kgl::vkg::Queue& Device::graphicsQueue()
    {
      const unsigned family = data().queue_families.graphics.getQueue() ;
      const unsigned index  = data().queue_families.graphics.index( family ) ;
      vk::Queue       vk_queue ;
      kgl::vkg::Queue queue    ;
      
      vk_queue = data().gpu.getQueue( family, index ) ;
      queue.initialize( *this, vk_queue, family, data().queues.size() ) ;
      
      data().queues.push_back( queue ) ;
      
      return data().queues.back() ;
    }
    
    const kgl::vkg::Queue& Device::presentQueue()
    {
      const unsigned family = data().queue_families.present.getQueue()      ;
      const unsigned index  = data().queue_families.present.index( family ) ;
      vk::Queue       vk_queue ;
      kgl::vkg::Queue queue    ;
      
      vk_queue = data().gpu.getQueue( family, index ) ;
      queue.initialize( *this, vk_queue, family, data().queues.size() ) ;
      
      data().queues.push_back( queue ) ;
      
      return data().queues.back() ;
    }
    
    const kgl::vkg::Queue& Device::computeQueue()
    {
      const unsigned family = data().queue_families.compute.getQueue()      ;
      const unsigned index  = data().queue_families.compute.index( family ) ;
      vk::Queue       vk_queue ;
      kgl::vkg::Queue queue    ;
      
      vk_queue = data().gpu.getQueue( family, index ) ;
      queue.initialize( *this, vk_queue, family, data().queues.size() ) ;
      
      data().queues.push_back( queue ) ;
      
      return data().queues.back() ;
    }
    
    const kgl::vkg::Queue& Device::transferQueue()
    {
      const unsigned family = data().queue_families.transfer.getQueue()      ;
      const unsigned index  = data().queue_families.transfer.index( family ) ;
      vk::Queue       vk_queue ;
      kgl::vkg::Queue queue    ;
      
      vk_queue = data().gpu.getQueue( family, index ) ;
      queue.initialize( *this, vk_queue, family, data().queues.size() ) ;
      
      data().queues.push_back( queue ) ;
      
      return data().queues.back() ;
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
    
    void Device::wait() const
    {
      data().gpu.waitIdle() ;
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

