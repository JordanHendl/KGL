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

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS

#include "Device.h"
#include "Queue.h"
#include "Vulkan.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <limits>
#include <tuple>
#include <memory>
#include <algorithm>

namespace nyx
{
  namespace vkg
  {
    static vk::PhysicalDeviceBufferDeviceAddressFeatures ext_buffer_address  ;
    
    /** Structure to manage vulkan queue families.
     */
    struct QueueFamily
    {
      typedef std::vector<vkg::Queue> QueueVector  ; ///< TODO
      typedef unsigned Family ;
      
      vk::QueueFamilyProperties* prop   ;
      Family                     family ;
      QueueVector                queues ;
      vk::PhysicalDevice         p_dev  ;

      /** Default constructor.
       */
      QueueFamily() = default ;
      
      /** Whether or not this object can do compute.
       * @return Whether or not this family can do compute
       */
      bool compute() ;
      
      /** Whether or not this object can do compute.
       * @return Whether or not this family can do compute
       */
      bool graphics() ;
      
      /** Whether or not this object can present to the input surface.
       * @return Whether or not this family can present to the input surface.
       */
      bool present( vk::SurfaceKHR surface ) ;
      
      /** Method to generate a queue for this 
       * @param device
       * @return 
       */
      vkg::Queue& makeQueue( const vkg::Device& device ) ;

      /** Assignment operator.
       * @param family The object to assign to this one.
       * @return Reference after assignment.
       */
      QueueFamily& operator=( const QueueFamily& family ) ;
    };

    struct DeviceData
    {
      typedef std::vector<std::string                 > Vector       ; ///< TODO
      typedef std::shared_ptr<std::vector<QueueFamily>> FamilyVector ; ///< TODO
      typedef std::vector<const char*                 > CharVector   ; ///< TODO
      
      ::vk::Device                           gpu             ; ///< TODO
      ::vk::PhysicalDevice                   physical_device ; ///< TODO
      ::vk::SurfaceKHR                       surface         ; ///< TODO
      ::vk::PhysicalDeviceFeatures           features        ; ///< TODO
      std::vector<vk::QueueFamilyProperties> properties      ; ///< TODO
      unsigned                               id              ; ///< TODO
      Vector                                 extension_list  ; ///< TODO
      Vector                                 layer_list      ; ///< TODO
      FamilyVector                           queues          ; ///< TODO

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
      
      /** Method to count all queues found for this device.
       * @return The amount of queuest found for this device.
       */
      unsigned totalQueues() const ;

      /** Method to filter out extensions requested to only ones provided by the system.
       * @return The list of extensions requested that are present on the system.
       */
      Vector filterExtensions() const ;
      
      /** Method to filter out layers requested to only ones provided by the system.
       * @return The list of layers requested that are present on the system.
       */
      Vector filterLayers() const ;
      
      /** Method to convert a list of strings to list of char* for vulkan objects.
       * @param list The list to convert.
       * @return A Vector of const char* pointing to string list's strings.
       */
      CharVector listToCharVector( Vector& list ) ;
    };
    
    bool QueueFamily::compute()
    {
      if( this->prop->queueFlags & vk::QueueFlagBits::eCompute ) return true ;
      return false ;
    }

    bool QueueFamily::graphics()
    {
      if( this->prop->queueFlags & vk::QueueFlagBits::eGraphics ) return true ;
      return false ;
    }
    
    bool QueueFamily::present( vk::SurfaceKHR surface )
    {
      vk::Bool32 support = false ;
      
      Vulkan::add( this->p_dev.getSurfaceSupportKHR( this->family, surface, &support ) ) ;
      return support ;
    }
    
    vkg::Queue& QueueFamily::makeQueue( const vkg::Device& device )
    {
      vkg::Queue queue ;
      if( this->queues.size() < this->prop->queueCount )
      {
        queue.initialize( device, device.device().getQueue( this->family, this->queues.size() ), this->family, static_cast<unsigned>( this->prop->queueFlags ) ) ;
        this->queues.push_back( queue ) ;
      }
      
      return this->queues[ 0 ] ;
    }

    DeviceData::DeviceData()
    {
      this->id = UINT32_MAX ;
//      this->extension_list = { "VK_KHR_buffer_device_address" } ;
    }

    DeviceData& DeviceData::operator=( const DeviceData& data )
    {
      this->id              = data.id                  ;
      this->gpu             = data.gpu                 ;
      this->physical_device = data.physical_device     ;
      this->surface         = data.surface             ;
      this->features        = data.features            ;
      this->properties      = data.properties          ;
      this->extension_list  = data.extension_list      ;
      this->queues          = data.queues              ;
      this->layer_list      = data.layer_list          ;

      return *this ;
    }
    
    unsigned DeviceData::totalQueues() const
    {
      return this->queues->size() ;
    }

    DeviceData::Vector DeviceData::filterLayers() const
    {
      Vector                             list             ;
      std::vector<::vk::LayerProperties> available_layers ;
      
      auto result = this->physical_device.enumerateDeviceLayerProperties() ;
      
      vkg::Vulkan::add( result.result ) ;
      
      available_layers = result.value ;
        
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

    DeviceData::CharVector DeviceData::listToCharVector( Vector& list )
    {
      DeviceData::CharVector char_list ;
      
      for( auto &str : list )
      {
        char_list.push_back( str.c_str() ) ;
      }
      
      return char_list ;
    }
        
    void DeviceData::generateDevice()
    {
      typedef std::vector<::vk::DeviceQueueCreateInfo> CreateInfos ;
      
      ::vk::DeviceCreateInfo      info            ;
      vk::PhysicalDeviceFeatures2 feat            ;
      DeviceData::CharVector      ext_list_char   ;
      DeviceData::CharVector      layer_list_char ;
      CreateInfos                 queue_infos     ;
      std::vector<float>          priorities      ;

      this->extension_list = this->filterExtensions()                     ;
      this->layer_list     = this->filterLayers()                         ;
      ext_list_char        = this->listToCharVector( this->extension_list ) ;
      layer_list_char      = this->listToCharVector( this->layer_list     ) ;

      queue_infos.resize( this->totalQueues() ) ;

      for( unsigned i = 0; i < this->totalQueues(); i++ )
      {
        queue_infos[ i ].setQueueFamilyIndex( i                                      ) ;
        queue_infos[ i ].setQueueCount      ( this->queues->at( i ).prop->queueCount ) ;
        priorities.clear() ;
        priorities.resize( this->queues->at( i ).prop->queueCount ) ;
        std::fill( priorities.begin(), priorities.end(), 1.0f ) ;
        queue_infos[ i ].setPQueuePriorities( priorities.data() ) ;
      }
      
      this->features.setShaderInt64( true ) ;
      info.setQueueCreateInfoCount   ( queue_infos.size()     ) ;
      info.setPQueueCreateInfos      ( queue_infos.data()     ) ;
      info.setEnabledExtensionCount  ( ext_list_char.size()   ) ;
      info.setPpEnabledExtensionNames( ext_list_char.data()   ) ;
      info.setEnabledLayerCount      ( layer_list_char.size() ) ;
      info.setPEnabledLayerNames     ( layer_list_char        ) ;
      feat.setFeatures( this->features ) ;
      nyx::vkg::ext_buffer_address.setBufferDeviceAddress( false ) ;
      
//      feat.setPNext( static_cast<void*>( &nyx::vkg::ext_buffer_address ) ) ;
      info.setPNext( static_cast<void*>( &feat                         ) ) ;
      
//      info.setPNext( static_cast<void*>( &nyx::vkg::ext_buffer_address ) ) ;

      vkg::Vulkan::add( this->physical_device.createDevice( &info, nullptr, &this->gpu ) ) ;
    }
    
    void DeviceData::findQueueFamilies()
    {
      unsigned id ;
      
      this->queues = std::make_shared<std::vector<QueueFamily>>()         ;
      this->properties = this->physical_device.getQueueFamilyProperties() ;
      id   = 0                                                            ;

      for( id = 0; id < this->properties.size(); id++ )
      {
        this->queues->push_back( QueueFamily() ) ;
        this->queues->at( id ).p_dev  = this->physical_device   ;
        this->queues->at( id ).prop   = &this->properties[ id ] ;
        this->queues->at( id ).family = id                      ;
      }
    }

    DeviceData::Vector DeviceData::filterExtensions() const
    {
      Vector                                   list               ;
      std::vector<::vk::ExtensionProperties> available_extentions ;
      
      auto result = this->physical_device.enumerateDeviceExtensionProperties() ;
      
      vkg::Vulkan::add( result.result ) ;
      
      available_extentions = result.value ;
      
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
    
    Device::operator unsigned() const
    {
      return data().id ;
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
    
    bool Device::initialized() const
    {
      return data().gpu ;
    }

    void Device::initialize( const ::vk::PhysicalDevice& physical_device, unsigned id )
    {
      data().physical_device = physical_device ;
      data().id              = id              ;
      
      data().findQueueFamilies() ;
      data().generateDevice   () ;
    }

    void Device::initialize( const ::vk::PhysicalDevice& physical_device, unsigned id, const ::vk::SurfaceKHR& surface )
    {
      data().physical_device = physical_device ;
      data().surface         = surface         ;
      data().id              = id              ;

      this->initialize( physical_device, id ) ;
    }
    
    void Device::reset()
    {
      data().gpu.destroy( nullptr ) ;
    }
    
    bool Device::hasExtension( const char* ext_name ) const
    {
      for( auto& ext : data().extension_list )
      {
        if( strcmp( ext.c_str(), ext_name ) == 0 ) return true ;
      }
      
      return false ;
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
    const nyx::vkg::Queue& Device::graphicsQueue() const
    {
      static const nyx::vkg::Queue dummy ;

      if( data().queues )
      for( auto& family : *data().queues )
      {
        if( family.graphics() )
        {
          return family.makeQueue( *this ) ;
        }
      }
      
      return dummy ;
    }
    
    const nyx::vkg::Queue& Device::presentQueue( unsigned long long surface ) const
    {
      static const nyx::vkg::Queue dummy ;
      const auto vk_surface = static_cast< vk::SurfaceKHR>( reinterpret_cast<VkSurfaceKHR>( surface ) ) ;
      if( data().queues )
      for( auto& family : *data().queues )
      {
        if( family.present( vk_surface ) && family.graphics() )
        {
          return family.makeQueue( *this ) ;
        }
      }
      
      return dummy ;
    }
    
    const nyx::vkg::Queue& Device::computeQueue() const
    {
      static vkg::Queue dummy ;
      
      if( data().queues )
      for( auto& family : *data().queues )
      {
        if( family.compute() )
        {
          return family.makeQueue( *this ) ;
        }
      }
      
      return dummy ;
    }
//    
//    const nyx::vkg::Queue& Device::transferQueue()
//    {
////      const unsigned family = data().queue_families.transfer.getQueue()      ;
////      const unsigned index  = data().queue_families.transfer.index( family ) ;
////      vk::Queue       vk_queue ;
////      nyx::vkg::Queue queue    ;
//      
////      vk_queue = data().gpu.getQueue( family, index ) ;
////      queue.initialize( *this, vk_queue, family, data().queues.size() ) ;
////      
////      data().queues.push_back( queue ) ;
////      
////      return data().queues.back() ;
//      
////      return queue ;
//    }

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
      if( data().gpu )
      {
        vkg::Vulkan::add( data().gpu.waitIdle() ) ;
      }
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

