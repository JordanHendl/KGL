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
 * File:   RenderPass.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 28, 2020, 10:34 AM
 */

#include "RenderPass.h"
#include "Device.h"
#include "Image.h"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace kgl
{
  namespace vkg
  {
    struct RenderPassData
    {
      typedef std::vector<kgl::vkg::Image                 > FramebufferImages      ;
      typedef std::vector<vk::Framebuffer                 > Framebuffers           ;
      typedef std::vector<vk::AttachmentDescription       > AttachmentDescriptions ;
      typedef std::vector<vk::AttachmentReference         > AttachmentReferences   ;
      typedef std::vector<vk::SubpassDescription          > SubpassDescriptions    ;
      typedef std::vector<vk::SubpassDependency           > SubpassDependencies    ;
      
      unsigned               width               ;
      unsigned               height              ;
      unsigned               layers              ;
      kgl::vkg::Device       device              ;
      ::vk::RenderPass       render_pass         ;
      Framebuffers           framebuffers        ;
      FramebufferImages      images              ;
      AttachmentDescriptions attach_descriptions ;
      AttachmentReferences   attach_references   ;
      SubpassDescriptions    sub_descriptions    ;
      SubpassDependencies    sub_dependencies    ;

      /** Default constructor.
       */
      RenderPassData() ;
              
      /** Method to create & initialize a vulkan render pass.
       * @return A newly created render pass.
       */
      ::vk::RenderPass makeRenderPass() ;
      
      /** Helper Method to make the framebuffers specified by this render pass.
       */
      void makeFramebuffers() ;
    };
    
    RenderPassData::RenderPassData()
    {
      this->width  = 0 ;
      this->height = 0 ;
      this->layers = 0 ;
    }
    
    ::vk::RenderPass RenderPassData::makeRenderPass()
    {
      ::vk::RenderPass           render_pass ;
      ::vk::RenderPassCreateInfo info        ;
      
      info.setAttachmentCount( this->attach_descriptions.size() ) ;
      info.setSubpassCount   ( this->sub_descriptions.size()    ) ;
      info.setDependencyCount( this->sub_dependencies.size()    ) ;
      info.setPAttachments   ( this->attach_descriptions.data() ) ;
      info.setPSubpasses     ( this->sub_descriptions.data()    ) ;
      info.setPDependencies  ( this->sub_dependencies.data()    ) ;

      return render_pass ;
    }

    void RenderPassData::makeFramebuffers()
    {
      vk::FramebufferCreateInfo info ;
      
    }

    RenderPass::RenderPass()
    {
      this->pass_data = new RenderPassData() ;
    }

    RenderPass::RenderPass( const RenderPass& render_pass )
    {
      this->pass_data = new RenderPassData() ;
      
      *this = render_pass ;
    }

    RenderPass::~RenderPass()
    {
      delete this->pass_data ;
    }

    RenderPass& RenderPass::operator=( const RenderPass& render_pass )
    {
      *this->pass_data = *render_pass.pass_data ;
      
      return *this ;
    }

    void RenderPass::initalize( const kgl::vkg::Device& device )
    {
      data().device = device ;
    }

    const ::vk::Framebuffer* RenderPass::framebuffers() const
    {
      return data().framebuffers.data() ;
    }

    const ::vk::RenderPass& RenderPass::pass() const
    {
      return data().render_pass ;
    }

    unsigned RenderPass::numFramebuffers() const
    {
      return data().framebuffers.size() ;
    }

    void RenderPass::setNumFramebuffers( unsigned count )
    {
      data().framebuffers.resize( count ) ;
      data().images      .resize( count ) ;
    }

    void RenderPass::setNumAttachments( unsigned count )
    {
      data().attach_descriptions.resize( count ) ;
      data().attach_references  .resize( count ) ;
    }

    void RenderPass::setNumSubpasses( unsigned count )
    {
      data().sub_descriptions.resize( count ) ;
    }

    void RenderPass::setNumDependancies( unsigned count )
    {
      data().sub_dependencies.resize( count ) ;
    }

    void RenderPass::setFramebufferWidth( unsigned width )
    {
      data().width = width ;
    }

    void RenderPass::setFramebufferHeight( unsigned height )
    {
      data().height = height ;
    }

    void RenderPass::setFramebufferLayers( unsigned layers )
    {
      data().layers = layers ;
    }

    void RenderPass::setAttachmentNumSamples( const ::vk::SampleCountFlagBits& flags, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setSamples( flags ) ;
    }

    void RenderPass::setAttachmentLoadOp( const ::vk::AttachmentLoadOp& op, unsigned idx  )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setLoadOp( op ) ;
    }

    void RenderPass::setAttachmentStoreOp( const ::vk::AttachmentStoreOp& op, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setStoreOp( op ) ;
    }

    void RenderPass::setAttachmentStencilLoadOp( const ::vk::AttachmentLoadOp& op, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setStencilLoadOp( op ) ;
    }

    void RenderPass::setAttachmentStencilStoreOp( const ::vk::AttachmentStoreOp& op, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setStencilStoreOp( op ) ;
    }

    void RenderPass::setAttachmentInitialLayout( const ::vk::ImageLayout& layout, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setInitialLayout( layout ) ;
    }

    void RenderPass::setAttachmentFinalLayout( const ::vk::ImageLayout& layout, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setFinalLayout( layout ) ;
    }

    void RenderPass::setAttachmentFormat( const ::vk::Format& format, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setFormat( format ) ;
    }

    void RenderPass::setReferenceLayout( const ::vk::ImageLayout& layout, unsigned idx )
    {
      if( idx < data().attach_references.size() ) data().attach_references[ idx ].setLayout( layout ) ;
    }

    void RenderPass::setSubpassPipelineBindPoint( const ::vk::PipelineBindPoint& point, unsigned idx )
    {
      if( idx < data().sub_descriptions.size() ) data().sub_descriptions[ idx ].setPipelineBindPoint( point ) ;
    }

    void RenderPass::setDependancySrc( unsigned src, unsigned idx )
    {
      if( idx < data().sub_dependencies.size() ) data().sub_dependencies[ idx ].setSrcSubpass( src ) ;
    }

    void RenderPass::setDependancyDst( unsigned dst, unsigned idx )
    {
      if( idx < data().sub_dependencies.size() ) data().sub_dependencies[ idx ].setDstSubpass( dst ) ;
    }

    void RenderPass::setDependancySrcStage( const ::vk::PipelineStageFlagBits& src, unsigned idx )
    {
      if( idx < data().sub_dependencies.size() ) data().sub_dependencies[ idx ].setSrcStageMask( src ) ;
    }

    void RenderPass::setDependancyDstStage( const ::vk::PipelineStageFlagBits& dst, unsigned idx )
    {
      if( idx < data().sub_dependencies.size() ) data().sub_dependencies[ idx ].setDstStageMask( dst ) ;
    }

    void RenderPass::setDependancySrcAccess( const ::vk::AccessFlagBits& src, unsigned idx )
    {
      if( idx < data().sub_dependencies.size() ) data().sub_dependencies[ idx ].setSrcAccessMask( src ) ;
    }

    void RenderPass::setDependancyDstAccess( const ::vk::AccessFlagBits& dst, unsigned idx )
    {
      if( idx < data().sub_dependencies.size() ) data().sub_dependencies[ idx ].setDstAccessMask( dst ) ;
    }

    RenderPassData& RenderPass::data()
    {
      return *this->pass_data ;
    }

    const RenderPassData& RenderPass::data() const
    {
      return *this->pass_data ;
    }
  }
}
