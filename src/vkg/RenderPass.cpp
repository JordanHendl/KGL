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
#include <iostream>
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
      
      ClearColors      clear_colors         ;
      Framebuffers     framebuffers         ;
      Dependencies     dependencies         ;
      References       references           ;
      References       color_references     ;
      References       depth_references     ;
      Subpasses        subpasses            ;
      Images           images               ;
      Attachments      attachments          ;
      vkg::Device      device               ;
      vkg::Swapchain   swapchain            ;
      vk::RenderPass   pass                 ;
      vk::Rect2D       area                 ;
      mutable unsigned current_framebuffer  ;
      mutable unsigned num_binded_subpasses ;
      unsigned         window_id            ;
      
      RenderPassData() ;
      ~RenderPassData() ;
      void makeRenderPass    () ;
      void makeFramebuffers  () ;
    };
    
    RenderPassData::RenderPassData()
    {
      this->current_framebuffer  = 0 ;
      this->num_binded_subpasses = 0 ;
     
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
      
      this->images      .resize( NUM_BUFFERS * this->attachments.size() ) ;
      this->framebuffers.resize( NUM_BUFFERS                            ) ;
      
      for( unsigned attach = 1; attach <= NUM_BUFFERS; attach++ )
      {
        for( unsigned index = 0; index < this->attachments.size(); index++ )
        {
          unsigned idx = attach * index ;
          
          format = this->attachments[ index ].format ;
          if( this->swapchain.initialized() && format != vk::Format::eD24UnormS8Uint )
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

            if( attachments[ index ].format == vk::Format::eD24UnormS8Uint )
            {
              this->images[ idx ].setUsage( nyx::ImageUsage::DepthStencil ) ;
            }

            this->images[ idx ].initialize( this->device, Vulkan::convert( format ), this->area.extent.width, this->area.extent.height, 1 ) ;
            views.push_back( this->images[ idx ].view() ) ;
          }
        }
        
        info.setAttachmentCount( this->attachments.size() ) ;
        info.setPAttachments   ( views.data()             ) ;
        info.setLayers         ( 1                        ) ;
        info.setRenderPass     ( this->pass               ) ;
        
        Vulkan::add( this->device.device().createFramebuffer( &info, nullptr, &this->framebuffers[ attach - 1 ] ) ) ;
        views.clear() ;
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
      
      data().swapchain.initialize( queue, window_id ) ;
      
      data().window_id          = window_id                 ;
      data().area.extent.width  = data().swapchain.width () ;
      data().area.extent.height = data().swapchain.height() ;

      data().attachments[ data().attachments.size() - 1 ].setFormat     ( data().swapchain.format()       ) ;
      data().attachments[ data().attachments.size() - 1 ].setFinalLayout( vk::ImageLayout::ePresentSrcKHR ) ;
//      data().references [ data().references.size () - 1 ].setLayout     ( vk::ImageLayout::ePresentSrcKHR ) ;
      data().makeRenderPass  () ;
      data().makeFramebuffers() ;
      data().swapchain.acquire() ;
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
    
    void RenderPass::advance() const 
    {
      data().current_framebuffer++ ;
      if( data().current_framebuffer >= data().framebuffers.size() ) data().current_framebuffer = 0 ;
    }
    
    vk::Framebuffer RenderPass::current() const
    { 
      auto ret = data().framebuffers[ data().current_framebuffer ] ;

      return ret ;
    }
    
    bool RenderPass::present()
    {
      bool recreate = false ;
      
      if( data().swapchain.initialized() )
      {
        if( data().swapchain.submit () == Vulkan::Error::RecreateSwapchain )
        {
          unsigned device = data().device    ;
          unsigned id     = data().window_id ;
          for( auto& framebuffer : data().framebuffers ) data().device.device().destroy( framebuffer ) ;
          if( data().pass ) data().device.device().destroy( data().pass ) ;
          data().images      .clear() ;
          data().framebuffers.clear() ;
          data().current_framebuffer = 0 ;
          this->initialize( device, id ) ;
          recreate = true ;
        }
        else if( data().swapchain.acquire() == Vulkan::Error::RecreateSwapchain )
        {
          unsigned device = data().device    ;
          unsigned id     = data().window_id ;
          for( auto& framebuffer : data().framebuffers ) data().device.device().destroy( framebuffer ) ;
          if( data().pass ) data().device.device().destroy( data().pass ) ;
          data().images      .clear() ;
          data().framebuffers.clear() ;
          data().current_framebuffer = 0 ;
          this->initialize( device, id ) ;
          recreate = true ;
        }
      }
      
      return recreate ;
    }

    unsigned RenderPass::device() const
    {
      return data().device ;
    }
    
    unsigned RenderPass::currentIndex() const
    {
      return data().current_framebuffer ;
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
    
    unsigned RenderPass::numBindedSubpasses() const
    {
      return data().num_binded_subpasses++ ;
    }
    
    unsigned RenderPass::subpassCount() const
    {
      return data().subpasses.size() ;
    }
    
    const vkg::Image& RenderPass::framebuffer( unsigned index ) const
    {
      if( index < data().images.size() ) return data().images[ index ] ;
      return data().images[ 0 ] ;
    }
    
    void RenderPass::addSubpass( const nyx::Attachment* attachments, unsigned attachment_count,
                                 const unsigned* subpass_deps, unsigned num_subpass_deps, 
                                 bool depth_enable, float depth_clear )
    {
      const unsigned src_subpass = data().subpasses.empty() ? 0 : data().subpasses.size() ;
      
      vk::AttachmentDescription attach_desc  ;
      vk::AttachmentReference   attach_ref   ;
      vk::SubpassDescription    subpass_desc ;
      vk::SubpassDependency     subpass_dep  ;
      vk::ClearValue            clear        ;
      vk::ClearColorValue       color        ;
      
      subpass_desc.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics ) ;
      
      // There can be only one depth per subpass, so append one if its enabled.
      if( depth_enable )
      {
        clear.depthStencil.depth   = depth_clear ;
        clear.depthStencil.stencil = 0           ;
        data().clear_colors.push_back( clear ) ;
        attach_desc.setStoreOp       ( vk::AttachmentStoreOp::eStore                   ) ;
        attach_desc.setLoadOp        ( vk::AttachmentLoadOp::eClear                    ) ;
        attach_desc.setSamples       ( vk::SampleCountFlagBits::e1                     ) ;
        attach_desc.setFinalLayout   ( vk::ImageLayout::eDepthStencilAttachmentOptimal ) ;
        attach_desc.setFormat        ( vk::Format::eD24UnormS8Uint                     ) ;
        attach_desc.setInitialLayout ( vk::ImageLayout::eUndefined                     ) ;
        attach_desc.setStencilLoadOp ( vk::AttachmentLoadOp::eClear                    ) ;
        attach_desc.setStencilStoreOp( vk::AttachmentStoreOp::eStore                   ) ;
       
        attach_ref.setAttachment    ( data().attachments.size() ) ;
        attach_ref.setLayout        ( attach_desc.finalLayout   ) ;
        
        data().attachments     .push_back( attach_desc ) ;
        data().references      .push_back( attach_ref  ) ;
        data().depth_references.push_back( attach_ref  ) ;
      }
      
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
        
        data().clear_colors    .push_back( clear       ) ;
        data().attachments     .push_back( attach_desc ) ;
        data().references      .push_back( attach_ref  ) ;
        data().color_references.push_back( attach_ref  ) ;
        subpass_desc.colorAttachmentCount++ ;
      }
      
      if( subpass_desc.pColorAttachments == nullptr )
      {
        subpass_desc.setPColorAttachments( data().color_references.data() + data().color_references.size() - subpass_desc.colorAttachmentCount ) ;
      }
      
      if( subpass_desc.pDepthStencilAttachment == nullptr )
      {
        subpass_desc.setPDepthStencilAttachment( data().depth_references.data() ) ;
      }
      
      // TODO Add attachment references here for referencing other attachments.
      for( unsigned index = 0; index < num_subpass_deps; index++ )
      {
        subpass_dep.setSrcSubpass( src_subpass           ) ;
        if( subpass_deps[ index ] < data().dependencies.size() )
        {
          subpass_dep.setDstSubpass( subpass_deps[ index ] ) ;
        }  
        
        subpass_dep.setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests ) ;
        subpass_dep.setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests ) ;
        subpass_dep.setDstAccessMask( vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite      ) ;
      }

      if( attachment_count != 0 ) data().subpasses   .push_back( subpass_desc ) ;
      if( num_subpass_deps != 0 ) data().dependencies.push_back( subpass_dep  ) ;
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
