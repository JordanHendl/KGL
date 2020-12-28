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
#include <vulkan/vulkan.hpp>
#include <vector>

namespace kgl
{
  namespace vkg
  {
    struct RenderPassData
    {
      typedef std::vector<vk::Framebuffer> Framebuffers ;
      
      ::vk::RenderPass render_pass  ;
      Framebuffers     framebuffers ;
    };

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
    
    }

    void RenderPass::setNumAttachments( unsigned count )
    {
    
    }

    void RenderPass::setNumSubpasses( unsigned count )
    {
    
    }

    void RenderPass::setNumDependancies( unsigned count )
    {
    
    }

    void RenderPass::setFramebufferWidth( unsigned width )
    {
    
    }

    void RenderPass::setFramebufferHeight( unsigned height )
    {
    
    }

    void RenderPass::setFramebufferLayers( unsigned layers )
    {
    
    }

    void RenderPass::setAttachmentNumSamples( const ::vk::SampleCountFlagBits& flags, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentNumSamples( const RenderPass::SampleFlags& flags, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentLoadOp( const ::vk::AttachmentLoadOp& op, unsigned idx  )
    {
    
    }

    void RenderPass::setAttachmentStoreOp( const ::vk::AttachmentStoreOp& op, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentStencilLoadOp( const ::vk::AttachmentLoadOp& op, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentStencilStoreOp( const ::vk::AttachmentLoadOp& op, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentInitialLayout( const ::vk::ImageLayout& layout, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentFinalLayout( const ::vk::ImageLayout& layout, unsigned idx )
    {
    
    }

    void RenderPass::setAttachmentFormat( const ::vk::Format& format, unsigned idx )
    {
    
    }

    void RenderPass::setReferenceLayout( const ::vk::ImageLayout& layout, unsigned idx )
    {
    
    }

    void RenderPass::setSubpassPipelineBindPoint( const ::vk::PipelineBindPoint& point, unsigned idx )
    {
    
    }

    void RenderPass::setDependancySrc( unsigned src, unsigned idx )
    {
    
    }

    void RenderPass::setDependancyDst( unsigned dst, unsigned idx )
    {
    
    }

    void RenderPass::setDependancySrcStage( const ::vk::PipelineStageFlagBits& src, unsigned idx )
    {
    
    }

    void RenderPass::setDependancyDstStage( const ::vk::PipelineStageFlagBits& dst, unsigned idx )
    {
    
    }

    void RenderPass::setDependancySrcAccess( const ::vk::AccessFlagBits& src, unsigned idx )
    {
    
    }

    void RenderPass::setDependancyDstAccess( const ::vk::AccessFlagBits& dst, unsigned idx )
    {
    
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
