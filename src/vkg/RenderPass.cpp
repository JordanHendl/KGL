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

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS

#include "RenderPass.h"
#include "Device.h"
#include "Image.h"
#include "Swapchain.h"
#include "Vulkan.h"
#include "library/RenderPass.h"
#include "library/Image.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <array>

namespace nyx
{
  namespace vkg
  {
    struct RenderPassData
    {
      using Framebuffers = std::vector<vk::Framebuffer>           ;
      using Images       = std::vector<vkg::Image>                ;
      using Attachments  = std::vector<vk::AttachmentDescription> ;
      using References   = std::vector<vk::AttachmentReference>   ;
      using Dependencies = std::vector<vk::SubpassDependency>     ;
      using Subpasses    = std::vector<vk::SubpassDescription>    ;
      using ClearColors  = std::vector<vk::ClearValue>            ;
      
      ClearColors      clear_colors        ;
      Framebuffers     framebuffers        ;
      Dependencies     dependencies        ;
      References       references          ;
      Subpasses        subpasses           ;
      Images           images              ;
      Attachments      attachments         ;
      vkg::Device      device              ;
      vkg::Swapchain   swapchain           ;
      vk::RenderPass   pass                ;
      vk::Rect2D       area                ;
      mutable unsigned current_framebuffer ;

      RenderPassData() ;
      ~RenderPassData() ;
      void makeRenderPass    () ;
      void makeFramebuffers  () ;
    };
    
    RenderPassData::RenderPassData()
    {
      this->current_framebuffer = 0 ;
      this->area.extent.setWidth ( 1280 ) ;
      this->area.extent.setHeight( 1024 ) ;
    }
    
    RenderPassData::~RenderPassData()
    {
      this->images      .clear() ;
      this->framebuffers.clear() ;
      this->attachments .clear() ;
      this->subpasses   .clear() ;
      this->references  .clear() ;
      this->dependencies.clear() ;
    }

    void RenderPassData::makeFramebuffers()
    {
      static constexpr unsigned NUM_BUFFERS = 3 ;

      vk::FramebufferCreateInfo  info   ;
      std::vector<vk::ImageView> views  ;
      vk::Format                 format ;
      
      if( !this->swapchain.initialized() )
      {
        this->images.resize( NUM_BUFFERS * this->attachments.size() ) ;
      }

      this->framebuffers.resize( NUM_BUFFERS ) ;
      
      for( unsigned attach = 1; attach <= NUM_BUFFERS; attach++ )
      {
        for( unsigned index = 0; index < this->attachments.size(); index++ )
        {
          unsigned idx = attach * index ;
          
          format = this->attachments[ index ].format ;
          if( this->swapchain.initialized() )
          {
            // Grab from swapchain here.
            views.push_back( this->swapchain.image( attach - 1 ).view() ) ;
            info.setWidth  ( this->swapchain.width ()                   ) ;
            info.setHeight ( this->swapchain.height()                   ) ;
          }
          else
          {
            info.setWidth          ( this->area.extent.width  ) ;
            info.setHeight         ( this->area.extent.height ) ;
            this->images[ idx ].initialize( this->device, Vulkan::convert( format ), this->area.extent.width, this->area.extent.height, 1 ) ;
            views.push_back( this->images[ idx ].view() ) ;
          }
        }
        
        info.setAttachmentCount( this->attachments.size() ) ;
        info.setPAttachments   ( views.data()             ) ;
        info.setLayers         ( 1                        ) ;
        info.setRenderPass     ( this->pass               ) ;
        
        Vulkan::add( this->device.device().createFramebuffer( &info, nullptr, &this->framebuffers[ attach - 1 ] ) ) ;
      }
    }

    void RenderPassData::makeRenderPass()
    {
      vk::RenderPassCreateInfo info ;
      
      info.setAttachments ( this->attachments  ) ;
      info.setDependencies( this->dependencies ) ;
      info.setSubpasses   ( this->subpasses    ) ;
      
      auto result = this->device.device().createRenderPass( info, nullptr ) ;
      Vulkan::add( result.result ) ;
      this->pass = result.value ;
    }
    
    RenderPass::RenderPass()
    {
      this->render_pass_data = new RenderPassData() ;
    }

    RenderPass::~RenderPass()
    {
      delete this->render_pass_data ;
    }

    void RenderPass::initialize( unsigned device )
    {
      const vkg::Queue queue = Vulkan::graphicsQueue( device ) ;
      data().device = Vulkan::device( device ) ;
      
      data().makeRenderPass  () ;
      data().makeFramebuffers() ;
    }

    void RenderPass::initialize( unsigned device, unsigned window_id )
    {
      const vkg::Queue queue = Vulkan::presentQueue( window_id, device ) ;
      data().device = Vulkan::device( device ) ;
      
      data().swapchain.initialize( queue, Vulkan::context( window_id ) ) ;
      data().swapchain.acquire() ;
      
      data().area.extent.width  = data().swapchain.width () ;
      data().area.extent.height = data().swapchain.height() ;

      data().attachments[ data().attachments.size() - 1 ].format = data().swapchain.format() ;
      data().makeRenderPass  () ;
      data().makeFramebuffers() ;
    }

    bool RenderPass::initialized() const
    {
      return data().device.initialized() && data().pass ;
    }

    void RenderPass::setDimensions( unsigned width, unsigned height )
    {
      data().area.extent.setWidth ( width  ) ;
      data().area.extent.setHeight( height ) ;
    } 

    unsigned RenderPass::count() const
    {
      return data().attachments.size() ;
    }
    
    vk::Framebuffer RenderPass::current() const
    {
      return data().framebuffers[ data().current_framebuffer++ ] ;
      if( data().current_framebuffer > data().framebuffers.size() - 1 ) data().current_framebuffer = 0 ;
    }
    
    unsigned RenderPass::device() const
    {
      return data().device ;
    }

    void RenderPass::reset()
    {
      for( auto& framebuffer : data().framebuffers ) data().device.device().destroy( framebuffer ) ;
      if( data().pass ) data().device.device().destroy( data().pass ) ;
      data().swapchain.reset() ;
      
      data().images      .clear() ;
      data().framebuffers.clear() ;
      data().attachments .clear() ;
      data().subpasses   .clear() ;
      data().references  .clear() ;
      data().dependencies.clear() ;
    }

    const vkg::Image& RenderPass::framebuffer( unsigned index ) const
    {
      if( index < data().images.size() ) return data().images[ index ] ;
      return data().images[ 0 ] ;
    }
    
    void RenderPass::addSubpass( const nyx::Attachment* attachments, unsigned attachment_count, const unsigned* subpass_deps, unsigned num_subpass_deps )
    {
      const unsigned src_subpass = data().subpasses.empty() ? 0 : data().subpasses.size() ;
      
      vk::AttachmentDescription attach_desc  ;
      vk::AttachmentReference   attach_ref   ;
      vk::SubpassDescription    subpass_desc ;
      vk::SubpassDependency     subpass_dep  ;
      vk::ClearValue            clear        ;
      vk::ClearColorValue       color        ;
      
      subpass_desc.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics ) ;
      
      for( unsigned index = 0; index < attachment_count; index++ )
      {
        attach_desc = Vulkan::convert( attachments[ index ] ) ;
        
        color.float32[ 0 ] = attachments[ index ].red  () ;
        color.float32[ 1 ] = attachments[ index ].green() ;
        color.float32[ 2 ] = attachments[ index ].blue () ;
        color.float32[ 3 ] = attachments[ index ].alpha() ;
        
        clear.setColor( color ) ;
        attach_ref.setAttachment( data().attachments.size() ) ;
        attach_ref.setLayout    ( attach_desc.finalLayout ) ;
        data().clear_colors.push_back( clear       ) ;
        data().attachments.push_back ( attach_desc ) ;
        data().references .push_back ( attach_ref  ) ;
        
        if( subpass_desc.pColorAttachments == nullptr )
        {
          subpass_desc.setPColorAttachments( data().references.data() + data().references.size() - 1 ) ;
        }
        subpass_desc.colorAttachmentCount++ ;
      }
      
      if( num_subpass_deps == 0 )
      {
        subpass_dep.setSrcSubpass   ( VK_SUBPASS_EXTERNAL ) ;
        subpass_dep.setSrcStageMask ( vk::PipelineStageFlagBits::eColorAttachmentOutput ) ;
        subpass_dep.setDstStageMask ( vk::PipelineStageFlagBits::eColorAttachmentOutput ) ;
        subpass_dep.setDstAccessMask( vk::AccessFlagBits::eColorAttachmentWrite         ) ;
      }
      
      // TODO Add attachment references here for referencing other attachments.
      for( unsigned index = 0; index < num_subpass_deps; index++ )
      {
        subpass_dep.setSrcSubpass( src_subpass           ) ;
        if( subpass_deps[ index ] < data().dependencies.size() )
        {
          subpass_dep.setDstSubpass( subpass_deps[ index ] ) ;
        }  
        subpass_dep.setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput ) ;
        subpass_dep.setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput ) ;
      }
      
      data().subpasses.push_back   ( subpass_desc ) ;
      data().dependencies.push_back( subpass_dep  ) ;
    }
    
    const vk::Framebuffer* RenderPass::framebuffers() const
    {
      return data().framebuffers.data() ;
    }
    
    const vk::ClearValue*  RenderPass::clearValues() const
    {
      return data().clear_colors.data() ;
    }
    
    const vk::RenderPass& RenderPass::pass() const
    {
      return data().pass ;
    }
    
    const vk::Rect2D& RenderPass::area() const
    {
      return data().area ;
    }

    RenderPassData& RenderPass::data()
    {
      return *this->render_pass_data ;
    }

    const RenderPassData& RenderPass::data() const
    {
      return *this->render_pass_data ;
    }
  }
}
