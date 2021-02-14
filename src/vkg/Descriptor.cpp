/*
 * Copyright (C) 2021 Jordan Hendl
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
 * File:   Descriptor.cpp
 * Author: Jordan Hendl
 * 
 * Created on January 31, 2021, 4:29 AM
 */

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS

#include "Descriptor.h"
#include "Device.h"
#include "library/Array.h"
#include "library/Image.h"
#include "Vulkan.h"
#include <nyxfile/NyxFile.h>
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace nyx
{
  namespace vkg
  {
    /** Function to convert a nyx array flag to a nyx uniform type.
     * @param flags The flag to convert
     * @return The uniform type equivalient
     */
    static nyx::UniformType convert( nyx::ArrayFlags flags ) ;
    
    /** Function to convert a nyx image usage flag to a nyx uniform type.
     * @param flags The flag to convert
     * @return The uniform type equivalient
     */
    static nyx::UniformType convert( nyx::ImageUsage flags ) ;
    
    /** Function to convert a nyx uniform type flag to a vulkan descriptor type.
     * @param flags The flag to convert
     * @return The vulkan flag equivalient
     */
    static vk::DescriptorType convert( nyx::UniformType flags ) ;

    struct DescriptorPoolData
    {
      struct Uniform
      {
        nyx::UniformType type    ;
        unsigned         binding ;
      };

      using UniformMap = std::unordered_map<std::string, Uniform> ;
      
      UniformMap              map       ;
      nyx::vkg::Device        device    ;
      unsigned                device_id ;
      unsigned                amount    ;
      vk::DescriptorPool      pool      ;
      vk::DescriptorSetLayout layout    ;
      
      /** Default constructor.
       */
      DescriptorPoolData() ;
    };

    struct DescriptorData
    {
      using UniformMap = std::unordered_map<std::string, DescriptorPoolData::Uniform> ;
      
      vk::DescriptorSet           set        ;
      vk::Device                  device     ;
      std::shared_ptr<UniformMap> parent_map ;
    };

    nyx::UniformType convert( nyx::ArrayFlags flags )
    {
      switch( flags.value() )
      {
        case nyx::ArrayFlags::StorageBuffer : return nyx::UniformType::SSBO ;
        case nyx::ArrayFlags::UniformBuffer : return nyx::UniformType::UBO  ;
        default : return nyx::UniformType::UBO ;
      }
    }
    
    nyx::UniformType convert( nyx::ImageUsage flags )
    {
      switch( flags )
      {
        default : return nyx::UniformType::SAMPLER ;
      }
    }
    
    vk::DescriptorType convert( nyx::UniformType flags )
    {
      switch( flags )
      {
        case nyx::UniformType::SAMPLER : return vk::DescriptorType::eCombinedImageSampler ;
        case nyx::UniformType::SSBO    : return vk::DescriptorType::eStorageBuffer        ;
        default : return vk::DescriptorType::eUniformBuffer ;
      }
    }
    
    DescriptorPoolData::DescriptorPoolData()
    {
      this->amount = 20      ;
      this->pool   = nullptr ;
    }
    
    Descriptor::Descriptor()
    {
      this->desc_data = new DescriptorData() ;
    }

    Descriptor::Descriptor( const Descriptor& desc )
    {
      this->desc_data = new DescriptorData() ;
      
      *this = desc ;
    }

    Descriptor::~Descriptor()
    {
      delete this->desc_data ;
    }

    Descriptor& Descriptor::operator=( const Descriptor& desc )
    {
      *this->desc_data = *desc.desc_data ;
      
      return *this ;
    }
    
    void Descriptor::initialize( const vkg::DescriptorPool& pool )
    {
      vk::DescriptorSetAllocateInfo info ;

      info.setDescriptorPool    ( pool.data().pool    ) ;
      info.setPSetLayouts       ( &pool.data().layout ) ;
      info.setDescriptorSetCount( 1                   ) ;

      auto result = pool.data().device.device().allocateDescriptorSets( info ) ;
      vkg::Vulkan::add( result.result ) ;
      
      data().device     = pool.data().device.device()                                         ;
      data().parent_map = std::make_shared<DescriptorPoolData::UniformMap>( pool.data().map ) ; 
      data().set        = result.value[ 0 ]                                                   ;
    }

    const vk::DescriptorSet& Descriptor::set() const
    {
      return data().set ;
    }
    
    void Descriptor::reset()
    {
      
    }

    void Descriptor::set( const char* name, const nyx::vkg::Buffer& buffer )
    {
      const auto iter = data().parent_map->find( name ) ;
      vk::DescriptorBufferInfo info  ;
      vk::WriteDescriptorSet   write ;
      
      if( iter != data().parent_map->end() )
      {
        info.setBuffer( buffer.buffer() ) ;
        info.setRange ( VK_WHOLE_SIZE   ) ;
        info.setOffset( 0               ) ;
        
        write.setDstSet         ( data().set                        ) ;
        write.setDstBinding     ( iter->second.binding              ) ;
        write.setDescriptorType ( vkg::convert( iter->second.type ) ) ;
        write.setDstArrayElement( 0                                 ) ;
        write.setDescriptorCount( 1                                 ) ;
        write.setPBufferInfo    ( &info                             ) ;
        
        data().device.updateDescriptorSets( 1, &write, 0, nullptr ) ;
      }
    }

    void Descriptor::set( const char* name, const nyx::vkg::Image& image )
    {
      const auto iter = data().parent_map->find( name ) ;
      vk::DescriptorImageInfo info  ;
      vk::WriteDescriptorSet  write ;

      if( iter != data().parent_map->end() )
      {
        info.setImageLayout( vkg::Vulkan::convert( image.layout() ) ) ;
        info.setSampler    ( image.sampler()                        ) ;
        info.setImageView  ( image.view()                           ) ;
        
        write.setDstSet         ( data().set                        ) ;
        write.setDstBinding     ( iter->second.binding              ) ;
        write.setDescriptorType ( vkg::convert( iter->second.type ) ) ;
        write.setDstArrayElement( 0                                 ) ;
        write.setDescriptorCount( 1                                 ) ;
        write.setPImageInfo     ( &info                             ) ;
        
        data().device.updateDescriptorSets( 1, &write, 0, nullptr ) ;
      }
    }

    DescriptorData& Descriptor::data()
    {
      return *this->desc_data ;
    }

    const DescriptorData& Descriptor::data() const
    {
      return *this->desc_data ;
    }
    
    DescriptorPool::DescriptorPool()
    {
      this->pool_data = new DescriptorPoolData() ;
    }

    DescriptorPool::~DescriptorPool()
    {
      delete this->pool_data ;
    }

    void DescriptorPool::initialize( const nyx::vkg::NyxShader& shader, unsigned amount )
    {
      unsigned index ;
      
      this->setAmount( amount ) ;
      
      if( shader.file().size() != 0 )
      {
        for( const auto shader : shader.file() )
        {
          for( index = 0; index < shader.numUniforms(); index++ )
          {
            data().map[ shader.uniformName( index ) ] = { shader.uniformType( index ), shader.uniformBinding( index ) } ;
          }
        }
      }
      
      data().device_id = shader.device()                   ;
      data().device    = Vulkan::device( shader.device() ) ;
      data().layout    = shader.layout()                   ;

      this->initialize() ;
    }

    void DescriptorPool::initialize()
    {
      const auto flag = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet ;

      vk::DescriptorPoolCreateInfo        info  ;
      std::vector<vk::DescriptorPoolSize> sizes ;
      vk::DescriptorPoolSize              size  ;
      
      size .setDescriptorCount( data().amount     ) ;
      sizes.reserve           ( data().map.size() ) ;

      for( const auto& uniform : data().map )
      {
        size.setType( nyx::vkg::convert( uniform.second.type ) ) ;
        sizes.push_back( size ) ;
      }
      
      info.setPoolSizeCount( sizes.size()  ) ;
      info.setPPoolSizes   ( sizes.data()  ) ;
      info.setMaxSets      ( data().amount ) ;
      info.setFlags        ( flag          ) ;
      
      auto result = data().device.device().createDescriptorPool( info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      data().pool = result.value ;
    }
    
    void DescriptorPool::addArrayInput( const char* name, unsigned binding, const nyx::ArrayFlags& type )
    {
      data().map[ name ] = { nyx::vkg::convert( type ), binding } ;
    }

    void DescriptorPool::addImageInput( const char* name, unsigned binding, nyx::ImageUsage usage )
    {
      data().map[ name ] = { nyx::vkg::convert( usage ), binding } ;
    }

    void DescriptorPool::setLayout( const vk::DescriptorSetLayout& layout )
    {
      data().layout = layout ;
    }

    void DescriptorPool::setAmount( unsigned amount )
    {
      data().amount = amount ;
    }

    nyx::vkg::Descriptor DescriptorPool::make()
    {
      vkg::Descriptor descriptor ;
      
      descriptor.initialize( *this ) ;
      return descriptor ;
    }

    DescriptorPoolData& DescriptorPool::data()
    {
      return *this->pool_data ;
    }

    const DescriptorPoolData& DescriptorPool::data() const
    {
      return *this->pool_data ;
    }
  }
}
