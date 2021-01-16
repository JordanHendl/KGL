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
 * File:   Pipeline.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 1:21 AM
 */

#include "Pipeline.h"
#include "Device.h"
#include "KgShader.h"
#include "RenderPass.h"
#include <vulkan/vulkan.hpp>

namespace kgl
{
  namespace vkg
  {
    struct PipelineConfig
    {
      vk::Viewport                             viewport               ;
      vk::Rect2D                               scissor                ;
      vk::PipelineViewportStateCreateInfo      viewport_info          ;
      vk::PipelineColorBlendStateCreateInfo    color_blend_info       ;
      vk::PipelineRasterizationStateCreateInfo rasterization_info     ;
      vk::PipelineInputAssemblyStateCreateInfo assembly_info          ;
      vk::PipelineMultisampleStateCreateInfo   multisample_info       ;
      vk::PipelineColorBlendAttachmentState    color_blend_attachment ;
      
      /** Default constructor.
       */
      PipelineConfig() ;
    };

    struct PipelineData
    {
      PipelineConfig       config              ;
      kgl::vkg::Device     device              ;
      kgl::vkg::RenderPass render_pass         ;
      kgl::vkg::KgShader   shader              ;
      vk::Pipeline         pipeline            ;
      vk::PipelineLayout   layout              ;
      vk::PipelineCache    cache               ;
      vk::ShaderStageFlags push_constant_flags ;
      unsigned             push_constant_size  ;
      
      /** Default constructor.
       */
      PipelineData() ;
      
      /** Helper method to create a vulkan pipeline layout.
       */
      void createLayout() ;
      
      /** Helper method to create a vulkan pipeline.
       */
      void createPipeline() ;
    };
    
    PipelineConfig::PipelineConfig()
    {
      const ::vk::ColorComponentFlags color_blend_mask = ::vk::ColorComponentFlagBits::eR | ::vk::ColorComponentFlagBits::eG | ::vk::ColorComponentFlagBits::eB | ::vk::ColorComponentFlagBits::eA ;
      
      this->rasterization_info.setDepthClampEnable       ( false                         ) ;
      this->rasterization_info.setRasterizerDiscardEnable( false                         ) ;
      this->rasterization_info.setPolygonMode            ( ::vk::PolygonMode::eFill      ) ;
      this->rasterization_info.setLineWidth              ( 1.0f                          ) ;
      this->rasterization_info.setCullMode               ( ::vk::CullModeFlagBits::eNone ) ;
      this->rasterization_info.setFrontFace              ( ::vk::FrontFace::eClockwise   ) ;
      this->rasterization_info.setDepthBiasEnable        ( false                         ) ;
      this->rasterization_info.setDepthBiasConstantFactor( 0.0f                          ) ;
      this->rasterization_info.setDepthBiasClamp         ( 0.0f                          ) ;
      this->rasterization_info.setDepthBiasSlopeFactor   ( 0.0f                          ) ;
      
      this->multisample_info.setSampleShadingEnable  ( false                         ) ;
      this->multisample_info.setMinSampleShading     ( 1.0f                          ) ;
      this->multisample_info.setAlphaToOneEnable     ( false                         ) ;
      this->multisample_info.setAlphaToCoverageEnable( false                         ) ;
      this->multisample_info.setPSampleMask          ( nullptr                       ) ;
      this->multisample_info.setRasterizationSamples ( ::vk::SampleCountFlagBits::e1 ) ;
      
      this->scissor.offset.setX( 0 ) ;
      this->scissor.offset.setY( 0 ) ;
      
      this->color_blend_attachment.setColorWriteMask     ( color_blend_mask             ) ;
      this->color_blend_attachment.setBlendEnable        ( false                        ) ;
      this->color_blend_attachment.setSrcColorBlendFactor( ::vk::BlendFactor::eSrcAlpha ) ;
      this->color_blend_attachment.setDstColorBlendFactor( ::vk::BlendFactor::eOne      ) ;
      this->color_blend_attachment.setColorBlendOp       ( ::vk::BlendOp::eAdd          ) ;
      this->color_blend_attachment.setSrcAlphaBlendFactor( ::vk::BlendFactor::eOne      ) ;
      this->color_blend_attachment.setDstAlphaBlendFactor( ::vk::BlendFactor::eZero     ) ;
      this->color_blend_attachment.setAlphaBlendOp       ( ::vk::BlendOp::eAdd          ) ;
      
      this->color_blend_info.setLogicOpEnable    ( false                         ) ;
      this->color_blend_info.setLogicOp          ( ::vk::LogicOp::eCopy          ) ;
      this->color_blend_info.setAttachmentCount  ( 1                             ) ;
      this->color_blend_info.setPAttachments     ( &this->color_blend_attachment ) ;
      
      this->viewport_info.setViewportCount( 1 ) ;
      this->viewport_info.setScissorCount ( 1 ) ;

      this->assembly_info.setTopology              ( ::vk::PrimitiveTopology::eTriangleList ) ;
      this->assembly_info.setPrimitiveRestartEnable( false                                  ) ;
    }

    PipelineData::PipelineData()
    {
      this->push_constant_size  = 0                                ;
      this->push_constant_flags = vk::ShaderStageFlagBits::eVertex ;
    }

    void PipelineData::createLayout()
    {
      vk::PipelineLayoutCreateInfo info        ;
      vk::PushConstantRange        range       ;
      vk::DescriptorSetLayout      desc_layout ;
      
      desc_layout = this->shader.layout() ;
      
      range.setOffset    ( 0                         ) ;
      range.setSize      ( this->push_constant_size  ) ;
      range.setStageFlags( this->push_constant_flags ) ;
      
      info.setSetLayoutCount        ( 1                                     ) ;
      info.setPSetLayouts           ( &desc_layout                          ) ;
      info.setPushConstantRangeCount( this->push_constant_size != 0 ? 1 : 0 ) ;
      info.setPPushConstantRanges   ( &range                                ) ;
      
      this->layout = this->device.device().createPipelineLayout( info, nullptr ) ;
    }
    
    void PipelineData::createPipeline()
    {
      vk::GraphicsPipelineCreateInfo         graphics_info ;
      vk::ComputePipelineCreateInfo          compute_info  ;
      vk::PipelineVertexInputStateCreateInfo vertex_input  ;
      
      vertex_input.setVertexAttributeDescriptionCount( this->shader.numVertexAttributes() ) ;
      vertex_input.setPVertexAttributeDescriptions   ( this->shader.attributes()          ) ;
      vertex_input.setVertexBindingDescriptionCount  ( this->shader.numVertexBindings()   ) ;
      vertex_input.setPVertexBindingDescriptions     ( this->shader.bindings()            ) ;
      

      if( this->render_pass.initialized() )
      {
        this->config.viewport_info.setPViewports( this->render_pass.viewports() ) ;
        this->config.viewport_info.setPScissors ( this->render_pass.scissors()  ) ;

        graphics_info.setPStages            ( this->shader.infos()             ) ;
        graphics_info.setStageCount         ( this->shader.numStages()         ) ;
        graphics_info.setLayout             ( this->layout                     ) ;
        graphics_info.setPVertexInputState  ( &vertex_input                    ) ;
        graphics_info.setPInputAssemblyState( &this->config.assembly_info      ) ;
        graphics_info.setPViewportState     ( &this->config.viewport_info      ) ;
        graphics_info.setPRasterizationState( &this->config.rasterization_info ) ;
        graphics_info.setPMultisampleState  ( &this->config.multisample_info   ) ;
        graphics_info.setPColorBlendState   ( &this->config.color_blend_info   ) ;
        graphics_info.setRenderPass         ( this->render_pass.pass()         ) ;
        
        this->pipeline = this->device.device().createGraphicsPipeline( this->cache, graphics_info ) ;
      }
      else
      {
        compute_info.setLayout( this->layout              ) ;
        compute_info.setStage ( this->shader.infos()[ 0 ] ) ;
        
        this->pipeline = this->device.device().createComputePipeline( this->cache, compute_info ) ;
      }
    }
    
    Pipeline::Pipeline()
    {
      this->pipeline_data = new PipelineData() ;
    }

    Pipeline::Pipeline( const Pipeline& pipeline )
    {
      this->pipeline_data = new PipelineData() ;
    
      *this = pipeline ;
    }

    Pipeline::~Pipeline()
    {
      delete this->pipeline_data ;
    }

    Pipeline& Pipeline::operator=( const Pipeline& pipeline )
    {
      *this->pipeline_data = *pipeline.pipeline_data ;
      
      return *this ;
    }

    Pipeline::operator ::vk::Pipeline&()
    {
      return data().pipeline ;
    }

    Pipeline::operator const ::vk::Pipeline&()
    {
      return data().pipeline ;
    }

    void Pipeline::initialize( const kgl::vkg::Device& device, const char* kg_path )
    {
      data().device = device ;
      data().shader.initialize( device, kg_path ) ;
      
      data().createLayout() ;
      data().createPipeline() ;
    }

    void Pipeline::initialize( const kgl::vkg::RenderPass& pass, const char* kg_path )
    {
      data().render_pass = pass          ;
      data().device      = pass.device() ;
      data().shader.initialize( pass.device(), kg_path ) ;
      
      data().createLayout() ;
      data().createPipeline() ;
    }

    void Pipeline::initialize( const KgShader& shader )
    {
      data().shader = shader          ;
      data().device = shader.device() ;
      
      data().createLayout() ;
      data().createPipeline() ;
    }

    void Pipeline::initialize( const kgl::vkg::RenderPass& pass, const KgShader& shader )
    {
      data().render_pass = pass          ;
      data().device      = pass.device() ;
      data().shader      = shader        ;
      
      data().config.viewport = pass.viewport() ;

      data().createLayout() ;
      data().createPipeline() ;
    }

    void Pipeline::setPushConstanceByteSize( unsigned size )
    {
      data().push_constant_size = size ;
    }
    
    void Pipeline::setPushConstantStageFlag( unsigned stage )
    {
      data().push_constant_flags = static_cast<vk::ShaderStageFlags>( stage ) ;
    }
    
    void Pipeline::reset()
    {
//      data().device.device().destroy( data().pipeline, nullptr ) ;
      data().device.device().destroy( data().layout  , nullptr ) ;
    }

    const vk::Pipeline& Pipeline::pipeline() const
    {
      return data().pipeline ;
    }

    const vk::PipelineLayout& Pipeline::layout() const
    {
      return data().layout ;
    }

    PipelineData& Pipeline::data()
    {
      return *this->pipeline_data ;
    }

    const PipelineData& Pipeline::data() const
    {
      return *this->pipeline_data ;
    }
  }
}