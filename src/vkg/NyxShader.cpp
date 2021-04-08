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
 * File:   KgShader.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 2:26 PM
 */

#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS

#include "NyxShader.h"
#include "Device.h"
#include "Vulkan.h"
#include <nyxfile/NyxFile.h>
#include <library/Array.h>
#include <library/Image.h>
#include <vulkan/vulkan.hpp>
#include <map>
#include <vector>
#include <istream>
#include <fstream>

namespace nyx
{
  namespace vkg
  {
    static vk::DescriptorType convert( nyx::ImageUsage flags ) ;
    static vk::DescriptorType convert( nyx::ArrayFlags flags ) ;
    static vk::VertexInputRate convert( vkg::NyxShader::InputRate rate ) ;

    /**
     * @param desc_type
     * @param type
     */
    static inline void operator<<( ::vk::DescriptorType& desc_type, const nyx::UniformType& type ) ;
    
    /**
     * @param stage
     */
    static inline ::vk::ShaderStageFlagBits convert( const nyx::ShaderStage& stage ) ;
    
    /**
     * @param type
     * @return 
     */
    static inline unsigned numIterationsFromType( const char* type ) ;
    
    /**
     * @param type
     * @return 
     */
    static inline ::vk::Format formatFromAttributeType( const char* type ) ;
    
    /**
     * @param format
     * @return 
     */
    static inline vk::Format formatFromShaderFormat( nyx::vkg::NyxShader::Format format ) ;

    /**
     * @param flag
     * @return 
     */
    static inline nyx::ShaderStage nyxStageFromVulkan( const vk::ShaderStageFlagBits& flag ) ;

    struct NyxShaderData
    {
      using SPIRVMap      = std::map<vk::ShaderStageFlagBits, vk::ShaderModuleCreateInfo> ; ///< TODO
      using ShaderModules = std::map<nyx::ShaderStage, vk::ShaderModule>                  ; ///< TODO
      using Attributes    = std::vector<vk::VertexInputAttributeDescription>              ; ///< TODO
      using Bindings      = std::vector<vk::VertexInputBindingDescription>                ; ///< TODO
      using Infos         = std::vector<vk::PipelineShaderStageCreateInfo>                ; ///< TODO
      using Descriptors   = std::vector<vk::DescriptorSetLayoutBinding>                   ; ///< TODO

      ShaderModules                          modules     ; ///< TODO
      Descriptors                            descriptors ; ///< TODO
      SPIRVMap                               spirv_map   ; ///< TODO
      Attributes                             inputs      ; ///< TODO
      Bindings                               bindings    ; ///< TODO
      Infos                                  infos       ; ///< TODO
      nyx::NyxFile                           nyxfile     ; ///< TODO
      nyx::ShaderIterator                    start       ;
      nyx::ShaderIterator                    end         ;
      nyx::vkg::Device                       device      ; ///< TODO
      vk::DescriptorSetLayout                layout      ; ///< TODO
      vk::PipelineVertexInputStateCreateInfo info        ; ///< TODO
      vk::VertexInputRate                    rate        ; ///< TODO
      
      /** Method to parse the loaded KgFile for its shader information.
       */
      void parse() ;
      
      /** Helper method for creating a Vulkan descriptor set layout 
       */
      void makeDescriptorLayout() ;
      
      /** Helper method for creating Vulkan shader modules.
       */
      void makeShaderModules() ;

      /** Helper method to generate the pipeline shader create infos.
       */
      void makePipelineShaderInfos() ;

      /** Default constructor.
       */
      NyxShaderData() ;
    };
    
    unsigned numIterationsFromType( const char* type )
    {
      const std::string t = std::string( type ) ;
      
           if( t == "mat4" ) return 4 ;
      else if( t == "mat3" ) return 3 ;
      else if( t == "mat2" ) return 2 ;
      else                   return 1 ;
    }
          
    unsigned byteSizeFromFormat( nyx::NyxFile& file, unsigned index )
    {
      const std::string s = file.inputType( index ) ;
      
           if( s == "mat4" ) return 4 * sizeof( float ) ;
      else if( s == "mat3" ) return 3 * sizeof( float ) ;
      else if( s == "mat2" ) return 3 * sizeof( float ) ;
      
      else return file.inputByteSize( index ) ;
    }

    ::vk::Format formatFromAttributeType( const char* type )
    {
      const std::string t = std::string( type ) ;
      
           if( t == "mat4"  ) return ::vk::Format::eR32G32B32A32Sfloat ;
      else if( t == "vec4"  ) return ::vk::Format::eR32G32B32A32Sfloat ;
      else if( t == "ivec4" ) return ::vk::Format::eR32G32B32A32Sint   ;
      else if( t == "uvec4" ) return ::vk::Format::eR32G32B32A32Uint   ;
      else if( t == "mat3"  ) return ::vk::Format::eR32G32B32A32Sfloat ;
      else if( t == "vec3"  ) return ::vk::Format::eR32G32B32A32Sfloat ;
      else if( t == "ivec3" ) return ::vk::Format::eR32G32B32A32Sint   ;
      else if( t == "uvec3" ) return ::vk::Format::eR32G32B32A32Uint   ;
      else if( t == "mat2"  ) return ::vk::Format::eR32G32Sfloat       ;
      else if( t == "vec2"  ) return ::vk::Format::eR32G32Sfloat       ;
      else if( t == "ivec2" ) return ::vk::Format::eR32G32Sint         ;
      else if( t == "uvec2" ) return ::vk::Format::eR32G32Uint         ;
      
      return ::vk::Format::eR32Sfloat ;
    }

    vk::Format formatFromShaderFormat( nyx::vkg::NyxShader::Format format )
    {
      switch( format )
      {
        case nyx::vkg::NyxShader::Format::mat4  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case nyx::vkg::NyxShader::Format::vec4  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case nyx::vkg::NyxShader::Format::ivec4 : return ::vk::Format::eR32G32B32A32Sint   ;
        case nyx::vkg::NyxShader::Format::uvec4 : return ::vk::Format::eR32G32B32A32Uint   ;
        case nyx::vkg::NyxShader::Format::mat3  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case nyx::vkg::NyxShader::Format::vec3  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case nyx::vkg::NyxShader::Format::ivec3 : return ::vk::Format::eR32G32B32A32Sint   ;
        case nyx::vkg::NyxShader::Format::uvec3 : return ::vk::Format::eR32G32B32A32Uint   ;
        case nyx::vkg::NyxShader::Format::mat2  : return ::vk::Format::eR32G32Sfloat       ;
        case nyx::vkg::NyxShader::Format::vec2  : return ::vk::Format::eR32G32Sfloat       ;
        case nyx::vkg::NyxShader::Format::ivec2 : return ::vk::Format::eR32G32Sint         ;
        case nyx::vkg::NyxShader::Format::uvec2 : return ::vk::Format::eR32G32Uint         ;
        default : return ::vk::Format::eR32Sfloat ;
      }
    }
                
    ::vk::ShaderStageFlagBits convert( const nyx::ShaderStage& stage )
    {
      switch( stage )
      {
        case nyx::ShaderStage::Fragment      : return ::vk::ShaderStageFlagBits::eFragment               ;
        case nyx::ShaderStage::Geometry      : return ::vk::ShaderStageFlagBits::eGeometry               ;
        case nyx::ShaderStage::Tess_C : return ::vk::ShaderStageFlagBits::eTessellationControl    ;
        case nyx::ShaderStage::Tess_E : return ::vk::ShaderStageFlagBits::eTessellationEvaluation ;
        case nyx::ShaderStage::Compute       : return ::vk::ShaderStageFlagBits::eCompute                ;
        case nyx::ShaderStage::Vertex        : return ::vk::ShaderStageFlagBits::eVertex                 ;
        default                                          : return ::vk::ShaderStageFlagBits::eFragment   ;
      }
    }

    void operator<<( ::vk::DescriptorType& desc_type, const nyx::UniformType& type )
    {
      switch( type )
      {
        case nyx::UniformType::Ubo     : desc_type = ::vk::DescriptorType::eUniformBuffer        ; break ;
        case nyx::UniformType::Sampler : desc_type = ::vk::DescriptorType::eCombinedImageSampler ; break ;
        case nyx::UniformType::Ssbo    : desc_type = ::vk::DescriptorType::eStorageBuffer        ; break ;
        case nyx::UniformType::None    : desc_type = ::vk::DescriptorType::eUniformBuffer        ; break ;
        default : break ;
      }
    }

    nyx::ShaderStage nyxStageFromVulkan( const vk::ShaderStageFlagBits& flag )
    {
      switch( flag )
      {
        case vk::ShaderStageFlagBits::eVertex   : return nyx::ShaderStage::Vertex   ;
        case vk::ShaderStageFlagBits::eFragment : return nyx::ShaderStage::Fragment ;
        case vk::ShaderStageFlagBits::eCompute  : return nyx::ShaderStage::Compute  ;
        case vk::ShaderStageFlagBits::eGeometry : return nyx::ShaderStage::Geometry ;
        default : return nyx::ShaderStage::Vertex ;
      }
    }
    
    vk::VertexInputRate convert( vkg::NyxShader::InputRate rate )
    {
      switch( rate )
      {
        case NyxShader::InputRate::Instanced : return vk::VertexInputRate::eInstance ;
        case NyxShader::InputRate::Vertex    : return vk::VertexInputRate::eVertex   ;
        default : return vk::VertexInputRate::eVertex ;
      }
    }
    
    vk::DescriptorType convert( nyx::ImageUsage flags )
    {
      switch( flags )
      {
        case nyx::ImageUsage::Sampled : return vk::DescriptorType::eCombinedImageSampler ;
        case nyx::ImageUsage::Input   : return vk::DescriptorType::eStorageImage ;
        default : return vk::DescriptorType::eSampler ;
      }
    }
    
    vk::DescriptorType convert( nyx::ArrayFlags flags )
    {
      switch( flags.value() )
      {
        case nyx::ArrayFlags::StorageBuffer : return vk::DescriptorType::eStorageBuffer ;
        case nyx::ArrayFlags::UniformBuffer : return vk::DescriptorType::eUniformBuffer ;
        default : return vk::DescriptorType::eUniformBuffer ;
      }
    }

    NyxShaderData::NyxShaderData()
    {
      this->rate = vk::VertexInputRate::eVertex ;
    }
    
    void NyxShaderData::parse()
    {
      std::map<std::string, ::vk::DescriptorSetLayoutBinding> binding_map ;
      vk::DescriptorSetLayoutBinding                          binding     ;
      vk::ShaderModuleCreateInfo                              module_info ;
      vk::PipelineShaderStageCreateInfo                       stage_info  ;
      ::vk::VertexInputAttributeDescription                   attr        ;
      ::vk::VertexInputBindingDescription                     bind        ;

      unsigned index  ;
      unsigned offset ;
      
      offset = 0 ;
      
              
      for( unsigned index = 0; index < this->nyxfile.numInputs(); index++ )
      {
        /** This is done in the case that a matrix ( or value greater than a vec4 ) is an input.
         */
        for( unsigned num_iter = 0 ; num_iter < numIterationsFromType( this->nyxfile.inputType( index ) ); num_iter++ )
        {
          attr.setLocation( this->nyxfile.inputLocation( index )                        ) ;
          attr.setBinding ( 0                                                           ) ; /// Problem? What is this?
          attr.setFormat  ( formatFromAttributeType( this->nyxfile.inputType( index ) ) ) ;
          attr.setOffset  ( offset                                                      ) ;
          
          this->inputs.push_back      ( attr                 ) ;
          offset += byteSizeFromFormat( this->nyxfile, index ) ;
        }
      }
      for( auto iter = this->nyxfile.begin(); iter != this->nyxfile.end(); ++iter )
      {
        for( index = 0; index < iter.numUniforms(); index++ )
        {
          if( binding_map.find( iter.uniformName( index ) ) != binding_map.end() ) 
          {
            auto flags = binding_map[ iter.uniformName( index ) ].stageFlags ;
            flags |= convert( iter.stage() ) ;
            binding_map[ iter.uniformName( index ) ].setStageFlags( flags ) ;
          }
          else
          {
            binding.setBinding         ( iter.uniformBinding( index )                  ) ;
            binding.setDescriptorCount ( iter.uniformSize   ( index )                  ) ;          
            binding.setStageFlags      ( binding.stageFlags |= convert( iter.stage() ) ) ;
            binding.descriptorType << iter.uniformType( index ) ;

            binding_map[ iter.uniformName( index ) ] = binding ;
          }
        }
        module_info.setCodeSize( iter.spirvSize() * sizeof( unsigned ) ) ;
        module_info.setPCode   ( iter.spirv()                          ) ;
        this->spirv_map[ convert( iter.stage() ) ] = module_info ;
      }
      
      index = 0 ;
      
      bind.setBinding  ( 0          ) ;
      bind.setInputRate( this->rate ) ;
      bind.setStride   ( offset     ) ;

      this->bindings.push_back( bind ) ;
      this->descriptors.resize( binding_map.size() ) ;
      for( auto bind : binding_map )
      {
        this->descriptors[ index++ ] = bind.second ;
      }
      
    }
    
    void NyxShaderData::makeDescriptorLayout()
    {
      vk::DescriptorSetLayoutCreateInfo info ;
      
      info.setBindingCount( this->descriptors.size() ) ;
      info.setPBindings   ( this->descriptors.data() ) ;
      
      auto result = this->device.device().createDescriptorSetLayout( info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      
      this->layout = result.value ;
    }
    
    void NyxShaderData::makeShaderModules()
    {
      vk::ShaderModule mod ;
      
      this->modules.clear() ;
      
      for( auto shader : this->spirv_map )
      {
        auto result = this->device.device().createShaderModule( shader.second, nullptr ) ;
        vkg::Vulkan::add( result.result ) ;
        mod = result.value ;
        
        this->modules[ nyxStageFromVulkan( shader.first ) ] = mod ;
      }
    }
    
    void NyxShaderData::makePipelineShaderInfos()
    {
      vk::PipelineShaderStageCreateInfo info ;
      unsigned iter ;
      
      this->infos.clear() ; 
      this->infos.resize( this->modules.size() ) ;
      
      iter = 0 ;
      for( auto it : this->modules )
      {
        info.setStage ( convert( it.first ) ) ;
        info.setModule( it.second           ) ;
        info.setPName ( "main"              ) ;
        
        this->infos[ iter++ ] = info ;
      }
    }

    NyxShader::NyxShader()
    {
      this->shader_data = new NyxShaderData() ;
    }

    NyxShader::NyxShader( const NyxShader& shader )
    {
      this->shader_data = new NyxShaderData() ;
      
      *this->shader_data = *shader.shader_data ;
    }

    NyxShader::~NyxShader()
    {
      delete this->shader_data ;
    }

    NyxShader& NyxShader::operator=( const NyxShader& shader )
    {
      *this->shader_data = *shader.shader_data ;
      
      return *this ;
    }
    
    const NyxFile& NyxShader::file() const
    {
      return data().nyxfile ;
    }

    void NyxShader::initialize( unsigned device, const char* nyx_path )
    {
      if( !Vulkan::initialized() ) Vulkan::initialize() ;

      data().device = Vulkan::device( device ) ;
      
      data().nyxfile.load( nyx_path ) ;
      
      data().parse() ;
      data().makeDescriptorLayout()    ;
      data().makeShaderModules()       ;
      data().makePipelineShaderInfos() ;
   }

    void NyxShader::initialize( unsigned device, const unsigned char* nyx_bytes, unsigned size )
    {
      if( !Vulkan::initialized() ) Vulkan::initialize() ;

      data().device = Vulkan::device( device ) ;
      
      data().nyxfile.load( nyx_bytes, size ) ;
      
      data().parse() ;
      data().makeDescriptorLayout()    ;
      data().makeShaderModules()       ;
      data().makePipelineShaderInfos() ;
    }

    void NyxShader::initialize( unsigned device )
    {
      data().device = Vulkan::device( device ) ;
      
      data().makeDescriptorLayout()    ;
      data().makeShaderModules()       ;
      data().makePipelineShaderInfos() ;
    }

    unsigned NyxShader::numStages() const
    {
      return data().modules.size() ;
    }

    unsigned NyxShader::numVertexAttributes() const
    {
      return data().inputs.size() ;
    }

    unsigned NyxShader::numVertexBindings() const
    {
      return data().bindings.size() ;
    }

    void NyxShader::addAttribute( unsigned location, unsigned binding, const NyxShader::Format& format, unsigned offset )
    {
      vk::VertexInputAttributeDescription attr ;
      
      attr.setLocation( location                         ) ;
      attr.setBinding ( binding                          ) ;
      attr.setFormat  ( formatFromShaderFormat( format ) ) ;
      attr.setOffset  ( offset                           ) ;
      
      data().inputs.push_back( attr ) ;
    }
    
    void NyxShader::addDescriptor( unsigned binding, const nyx::ArrayFlags& type, unsigned count, nyx::ShaderStage stage )
    {
      vk::DescriptorSetLayoutBinding info ;
      
      info.setBinding        ( binding               ) ;
      info.setDescriptorType ( vkg::convert( type )  ) ;
      info.setDescriptorCount( count                 ) ;
      info.setStageFlags     ( vkg::convert( stage ) ) ;
      
      data().descriptors.push_back( info ) ;
    }
    
    void NyxShader::addDescriptor( unsigned binding, nyx::ImageUsage type, unsigned count, nyx::ShaderStage stage )
    {
      vk::DescriptorSetLayoutBinding info ;
      
      info.setBinding        ( binding               ) ;
      info.setDescriptorType ( vkg::convert( type )  ) ;
      info.setDescriptorCount( count                 ) ;
      info.setStageFlags     ( vkg::convert( stage ) ) ;
      
      data().descriptors.push_back( info ) ;
    }
    
    void NyxShader::addInputBinding( unsigned binding, unsigned stride, InputRate rate )
    {
      vk::VertexInputBindingDescription bind ;
      
      bind.setBinding  ( binding              ) ; 
      bind.setStride   ( stride               ) ;
      bind.setInputRate( vkg::convert( rate ) ) ;

      data().bindings.push_back( bind ) ;
    }
        
    void NyxShader::addShaderModule( nyx::ShaderStage stage, const unsigned* spirv, unsigned size )
    {
      vk::ShaderModuleCreateInfo info ;
      
      info.setCodeSize( size  ) ;
      info.setPCode   ( spirv ) ;
      
      data().spirv_map[ vkg::convert( stage ) ] = info ;
    }
    
    unsigned NyxShader::device() const
    {
      return data().device ;
    }
    
    const vk::DescriptorSetLayout& NyxShader::layout() const
    {
      return data().layout ;
    }

    const vk::VertexInputAttributeDescription* NyxShader::attributes() const
    {
      return data().inputs.data() ;
    }

    const vk::VertexInputBindingDescription* NyxShader::bindings() const
    {
      return data().bindings.data() ;
    }

    const vk::PipelineShaderStageCreateInfo* NyxShader::infos() const
    {
      return data().infos.data() ;
    }

    void NyxShader::reset()
    {
      for( auto module : data().modules )
      {
        data().device.device().destroy( module.second, nullptr ) ;
      }
      if( data().layout.operator VkDescriptorSetLayout() != nullptr ) data().device.device().destroy( data().layout, nullptr ) ;
      
      data().nyxfile    = nyx::NyxFile() ;
      data().modules    .clear() ;
      data().inputs     .clear() ;
      data().descriptors.clear() ;
      data().bindings   .clear() ;
      data().spirv_map  .clear() ;
      data().infos      .clear() ;
    }

    NyxShaderData& NyxShader::data()
    {
      return *this->shader_data ;
    }

    const NyxShaderData& NyxShader::data() const
    {
      return *this->shader_data ;
    }
  }
}

