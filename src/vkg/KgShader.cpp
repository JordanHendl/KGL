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

#include "KgShader.h"
#include <kgfile/KgFile.h>
#include "Device.h"
#include <vulkan/vulkan.hpp>
#include <map>
#include <vector>
#include <istream>
#include <fstream>

namespace kgl
{
  namespace vkg
  {
    /**
     * @param desc_type
     * @param type
     */
    static inline void operator<<( ::vk::DescriptorType& desc_type, const kgl::UniformType& type ) ;
    
    /**
     * @param stage
     */
    static inline ::vk::ShaderStageFlagBits vulkanBitFromStage( const kgl::ShaderStage& stage ) ;
    
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
    static inline vk::Format formatFromShaderFormat( kgl::vkg::KgShader::Format format ) ;

    /**
     * @param flag
     * @return 
     */
    static inline kgl::ShaderStage kglStageFromVulkan( const vk::ShaderStageFlagBits& flag ) ;

    struct KgShaderData
    {
      using SPIRVMap      = std::map<vk::ShaderStageFlagBits, vk::ShaderModuleCreateInfo> ; ///< TODO
      using ShaderModules = std::map<kgl::ShaderStage, vk::ShaderModule>                  ; ///< TODO
      using Attributes    = std::vector<vk::VertexInputAttributeDescription>              ; ///< TODO
      using Bindings      = std::vector<vk::VertexInputBindingDescription>                ; ///< TODO
      using Infos         = std::vector<vk::PipelineShaderStageCreateInfo>                ; ///< TODO
      using Descriptors   = std::vector<vk::DescriptorSetLayoutBinding>                   ; ///< TODO

      ShaderModules                          modules     ; ///< TODO
      Descriptors                            descriptors ; ///< TODO
      SPIRVMap                               spirv_map   ; ///< TODO
      Attributes                             attributes  ; ///< TODO
      Bindings                               bindings    ; ///< TODO
      Infos                                  infos       ; ///< TODO
      kgl::KgFile                            kgfile      ; ///< TODO
      kgl::vkg::Device                       device      ; ///< TODO
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
      KgShaderData() ;
    };
    
    unsigned numIterationsFromType( const char* type )
    {
      const std::string t = std::string( type ) ;
      
           if( t == "mat4" ) return 4 ;
      else if( t == "mat3" ) return 3 ;
      else if( t == "mat2" ) return 2 ;
      else                   return 1 ;
    }
          
    unsigned byteSizeFromFormat( kgl::ShaderIterator& it, unsigned index )
    {
      const std::string s = std::string( it.attributeType( index ) ) ;
      
           if( s == "mat4" ) return 4 * sizeof( float ) ;
      else if( s == "mat3" ) return 3 * sizeof( float ) ;
      else if( s == "mat2" ) return 3 * sizeof( float ) ;
      
      else return it.attributeByteSize( index ) ;
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

    vk::Format formatFromShaderFormat( kgl::vkg::KgShader::Format format )
    {
      switch( format )
      {
        case kgl::vkg::KgShader::Format::mat4  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case kgl::vkg::KgShader::Format::vec4  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case kgl::vkg::KgShader::Format::ivec4 : return ::vk::Format::eR32G32B32A32Sint   ;
        case kgl::vkg::KgShader::Format::uvec4 : return ::vk::Format::eR32G32B32A32Uint   ;
        case kgl::vkg::KgShader::Format::mat3  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case kgl::vkg::KgShader::Format::vec3  : return ::vk::Format::eR32G32B32A32Sfloat ;
        case kgl::vkg::KgShader::Format::ivec3 : return ::vk::Format::eR32G32B32A32Sint   ;
        case kgl::vkg::KgShader::Format::uvec3 : return ::vk::Format::eR32G32B32A32Uint   ;
        case kgl::vkg::KgShader::Format::mat2  : return ::vk::Format::eR32G32Sfloat       ;
        case kgl::vkg::KgShader::Format::vec2  : return ::vk::Format::eR32G32Sfloat       ;
        case kgl::vkg::KgShader::Format::ivec2 : return ::vk::Format::eR32G32Sint         ;
        case kgl::vkg::KgShader::Format::uvec2 : return ::vk::Format::eR32G32Uint         ;
        default : return ::vk::Format::eR32Sfloat ;
      }
    }
                
    ::vk::ShaderStageFlagBits vulkanBitFromStage( const kgl::ShaderStage& stage )
    {
      switch( stage )
      {
        case kgl::ShaderStage::FRAGMENT      : return ::vk::ShaderStageFlagBits::eFragment               ;
        case kgl::ShaderStage::GEOMETRY      : return ::vk::ShaderStageFlagBits::eGeometry               ;
        case kgl::ShaderStage::TESSALATION_C : return ::vk::ShaderStageFlagBits::eTessellationControl    ;
        case kgl::ShaderStage::TESSELATION_E : return ::vk::ShaderStageFlagBits::eTessellationEvaluation ;
        case kgl::ShaderStage::COMPUTE       : return ::vk::ShaderStageFlagBits::eCompute                ;
        case kgl::ShaderStage::VERTEX        : return ::vk::ShaderStageFlagBits::eVertex                 ;
        default                                          : return ::vk::ShaderStageFlagBits::eFragment   ;
      }
    }

    void operator<<( ::vk::DescriptorType& desc_type, const kgl::UniformType& type )
    {
      switch( type )
      {
        case kgl::UniformType::UBO     : desc_type = ::vk::DescriptorType::eUniformBuffer        ; break ;
        case kgl::UniformType::SAMPLER : desc_type = ::vk::DescriptorType::eCombinedImageSampler ; break ;
        case kgl::UniformType::SSBO    : desc_type = ::vk::DescriptorType::eStorageBuffer        ; break ;
        case kgl::UniformType::None    : desc_type = ::vk::DescriptorType::eUniformBuffer        ; break ;
        default : break ;
      }
    }

    kgl::ShaderStage kglStageFromVulkan( const vk::ShaderStageFlagBits& flag )
    {
      switch( flag )
      {
        case vk::ShaderStageFlagBits::eVertex   : return kgl::ShaderStage::VERTEX   ;
        case vk::ShaderStageFlagBits::eFragment : return kgl::ShaderStage::FRAGMENT ;
        case vk::ShaderStageFlagBits::eCompute  : return kgl::ShaderStage::COMPUTE  ;
        case vk::ShaderStageFlagBits::eGeometry : return kgl::ShaderStage::GEOMETRY ;
        default : return kgl::ShaderStage::VERTEX ;
      }
    }
    
    KgShaderData::KgShaderData()
    {
      this->rate = vk::VertexInputRate::eVertex ;
    }
    
    void KgShaderData::parse()
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
      for( auto iter = this->kgfile.begin(); iter != this->kgfile.end(); ++iter )
      {
        for( index = 0; index < iter.numUniforms(); index++ )
        {
          if( binding_map.find( iter.uniformName( index ) ) != binding_map.end() ) 
          {
            auto flags = binding_map[ iter.uniformName( index ) ].stageFlags ;
            flags |= vulkanBitFromStage( iter.stage() ) ;
            binding_map[ iter.uniformName( index ) ].setStageFlags( flags ) ;
          }
          else
          {
            binding.setBinding         ( iter.uniformBinding( index )                             ) ;
            binding.setDescriptorCount ( iter.uniformSize   ( index )                             ) ;          
            binding.setStageFlags      ( binding.stageFlags |= vulkanBitFromStage( iter.stage() ) ) ;
            binding.descriptorType << iter.uniformType( index ) ;

            binding_map[ iter.uniformName( index ) ] = binding ;
          }
        }
        
        
        if( iter.stage() == kgl::ShaderStage::VERTEX )
        {
          for( unsigned index = 0; index < iter.numAttributes(); index++ )
          {
            if( iter.attributeIsInput( index ) )
            {
              /** This is done in the case that a matrix ( or value greater than a vec4 ) is an input.
               */
              for( unsigned num_iter = 0 ; num_iter < numIterationsFromType( iter.attributeType( index ) ); num_iter++ )
              {
                attr.setLocation( iter.attributeLocation( index ) + num_iter              ) ;
                attr.setBinding ( 0                                                       ) ; /// Problem? What is this?
                attr.setFormat  ( formatFromAttributeType( iter.attributeType( index )  ) ) ;
                attr.setOffset  ( offset                                                  ) ;
                
                this->attributes.push_back  ( attr        ) ;
                offset += byteSizeFromFormat( iter, index ) ;
              }
            }
          }
        }
        module_info.setCodeSize( iter.spirvSize() * sizeof( unsigned ) ) ;
        module_info.setPCode   ( iter.spirv()                          ) ;
        this->spirv_map[ vulkanBitFromStage( iter.stage() ) ] = module_info ;
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
    
    void KgShaderData::makeDescriptorLayout()
    {
      vk::DescriptorSetLayoutCreateInfo info ;
      
      info.setBindingCount( this->descriptors.size() ) ;
      info.setPBindings   ( this->descriptors.data() ) ;
      
      this->layout = this->device.device().createDescriptorSetLayout( info, nullptr ) ;
    }
    
    void KgShaderData::makeShaderModules()
    {
      vk::ShaderModule mod ;
      
      this->modules.clear() ;
      
      for( auto shader : this->spirv_map )
      {
        mod = this->device.device().createShaderModule( shader.second, nullptr ) ;
        
        this->modules[ kglStageFromVulkan( shader.first ) ] = mod ;
      }
    }
    
    void KgShaderData::makePipelineShaderInfos()
    {
      vk::PipelineShaderStageCreateInfo info ;
      unsigned iter ;
      
      this->infos.clear() ; 
      this->infos.resize( this->modules.size() ) ;
      
      iter = 0 ;
      for( auto it : this->modules )
      {
        info.setStage ( vulkanBitFromStage( it.first ) ) ;
        info.setModule( it.second                      ) ;
        info.setPName ( "main"                         ) ;
        
        this->infos[ iter++ ] = info ;
      }
    }

    KgShader::KgShader()
    {
      this->shader_data = new KgShaderData() ;
    }

    KgShader::KgShader( const KgShader& shader )
    {
      this->shader_data = new KgShaderData() ;
      
      *this->shader_data = *shader.shader_data ;
    }

    KgShader::~KgShader()
    {
      delete this->shader_data ;
    }

    KgShader& KgShader::operator=( const KgShader& shader )
    {
      *this->shader_data = *shader.shader_data ;
      
      return *this ;
    }

    void KgShader::initialize( const kgl::vkg::Device& device, const char* kg_path )
    {
      data().device = device ;
      data().kgfile.load( kg_path ) ;
      
      data().parse() ;
      data().makeDescriptorLayout()    ;
      data().makeShaderModules()       ;
      data().makePipelineShaderInfos() ;
    }

    void KgShader::initialize( const kgl::vkg::Device& device )
    {
      data().device = device ;
      
      data().makeDescriptorLayout()    ;
      data().makeShaderModules()       ;
      data().makePipelineShaderInfos() ;
    }

    unsigned KgShader::numStages() const
    {
      return data().modules.size() ;
    }

    unsigned KgShader::numVertexAttributes() const
    {
      return data().attributes.size() ;
    }

    unsigned KgShader::numVertexBindings() const
    {
      return data().bindings.size() ;
    }

    void KgShader::addAttribute( unsigned location, unsigned binding, const KgShader::Format& format, unsigned offset )
    {
      vk::VertexInputAttributeDescription attr ;
      
      attr.setLocation( location                         ) ;
      attr.setBinding ( binding                          ) ;
      attr.setFormat  ( formatFromShaderFormat( format ) ) ;
      attr.setOffset  ( offset                           ) ;
      
      data().attributes.push_back( attr ) ;
    }
    void KgShader::addDescriptor( unsigned binding, const vk::DescriptorType& type, unsigned count, const vk::ShaderStageFlags flags )
    {
      vk::DescriptorSetLayoutBinding info ;
      
      info.setBinding        ( binding ) ;
      info.setDescriptorType ( type    ) ;
      info.setDescriptorCount( count ) ;
      info.setStageFlags     ( flags ) ;
      
      data().descriptors.push_back( info ) ;
    }
    
    void KgShader::addInputBinding( unsigned binding, unsigned stride, const vk::VertexInputRate& rate )
    {
      vk::VertexInputBindingDescription bind ;
      
      bind.setBinding  ( binding ) ; 
      bind.setStride   ( stride  ) ;
      bind.setInputRate( rate    ) ;

      data().bindings.push_back( bind ) ;
    }
        
    void KgShader::addShaderModule( const vk::ShaderStageFlagBits& flags, const unsigned* spirv, unsigned size )
    {
      vk::ShaderModuleCreateInfo info ;
      
      info.setCodeSize( size  ) ;
      info.setPCode   ( spirv ) ;
      
      data().spirv_map[ flags ] = info ;
    }
    
    const kgl::vkg::Device& KgShader::device() const
    {
      return data().device ;
    }
    
    const vk::DescriptorSetLayout& KgShader::layout() const
    {
      return data().layout ;
    }

    const vk::VertexInputAttributeDescription* KgShader::attributes() const
    {
      return data().attributes.data() ;
    }

    const vk::VertexInputBindingDescription* KgShader::bindings() const
    {
      return data().bindings.data() ;
    }

    const vk::PipelineShaderStageCreateInfo* KgShader::infos() const
    {
      return data().infos.data() ;
    }

    void KgShader::reset()
    {
      for( auto module : data().modules )
      {
        data().device.device().destroy( module.second, nullptr ) ;
      }
      
      data().device.device().destroy( data().layout, nullptr ) ;

      data().modules    .clear() ;
      data().attributes .clear() ;
      data().descriptors.clear() ;
      data().spirv_map  .clear() ;
      data().infos      .clear() ;
    }

    KgShaderData& KgShader::data()
    {
      return *this->shader_data ;
    }

    const KgShaderData& KgShader::data() const
    {
      return *this->shader_data ;
    }
  }
}

