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

#include "RenderPass.h"
#include "Device.h"
#include "Image.h"
#include "Swapchain.h"
#include "Vulkan.h"
#include "library/Image.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <array>

namespace nyx
{
  namespace vkg
  {
    /** Structure encompassing a viewport ration.
     */
    struct ViewportRatio
    {
      float width_ratio   = 1.0f ; ///< The viewport width ratio.
      float height_ratio  = 1.0f ; ///< The viewport height ratio.
    };

    /** Structure to encompass a render pass's internal data.
     */
    struct RenderPassData
    {
      typedef std::vector<nyx::vkg::Image          > FramebufferImages      ; ///< TODO
      typedef std::vector<vk::Framebuffer          > Framebuffers           ; ///< TODO
      typedef std::vector<vk::AttachmentDescription> AttachmentDescriptions ; ///< TODO
      typedef std::vector<vk::AttachmentReference  > AttachmentReferences   ; ///< TODO
      typedef std::vector<vk::SubpassDescription   > SubpassDescriptions    ; ///< TODO
      typedef std::vector<vk::SubpassDependency    > SubpassDependencies    ; ///< TODO
      typedef std::vector<vk::Viewport             > Viewports              ; ///< TODO
      typedef std::vector<ViewportRatio            > ViewportRatios         ; ///< TODO
      typedef std::vector<vk::ImageView            > ImageViews             ; ///< TODO
      
      unsigned               width               ; ///< TODO
      unsigned               height              ; ///< TODO
      unsigned               layers              ; ///< TODO
      mutable unsigned       current             ; ///< The current framebuffer to use for this pass.
      nyx::vkg::Device       device              ; ///< TODO
      vk::SwapchainKHR       swapchain           ; ///< TODO
      vk::ClearValue         clear_color         ; ///< TODO
      vk::RenderPass         render_pass         ; ///< TODO
      vk::Rect2D             area                ; ///< TODO
      vk::Rect2D             scissor             ; ///< TODO
      Framebuffers           framebuffers        ; ///< TODO
      FramebufferImages      images              ; ///< TODO
      AttachmentDescriptions attach_descriptions ; ///< TODO
      AttachmentReferences   attach_references   ; ///< TODO
      SubpassDescriptions    sub_descriptions    ; ///< TODO
      SubpassDependencies    sub_dependencies    ; ///< TODO
      Viewports              viewports           ; ///< TODO
      ViewportRatios         ratios              ; ///< TODO

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
      
      /** Helper Method to make the framebuffers specified by this render pass.
       */
      void makeFramebuffers( const nyx::vkg::Swapchain& chain ) ;
    };
    
    RenderPassData::RenderPassData()
    {
      ::vk::AttachmentDescription attach_desc ;
      ::vk::AttachmentReference   attach_ref  ;
      ::vk::SubpassDescription    sub_desc    ;
      ::vk::Viewport              viewport    ;
      ::vk::SubpassDependency     dependency  ;
      
      viewport.setX       ( 0    ) ;
      viewport.setY       ( 0    ) ;
      viewport.setMinDepth( 0.f  ) ;
      viewport.setMaxDepth( 1.f  ) ;
      viewport.setWidth   ( 1240 ) ;
      viewport.setHeight  ( 1024 ) ;
      
      attach_desc.setFormat        ( vk::Format::eR8G8B8A8Srgb                ) ;
      attach_desc.setSamples       ( vk::SampleCountFlagBits::e1              ) ;
      attach_desc.setLoadOp        ( vk::AttachmentLoadOp::eClear             ) ;
      attach_desc.setStencilLoadOp ( vk::AttachmentLoadOp::eDontCare          ) ;
      attach_desc.setStoreOp       ( vk::AttachmentStoreOp::eDontCare         ) ;
      attach_desc.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare         ) ;
      attach_desc.setInitialLayout ( vk::ImageLayout::eUndefined              ) ;
      attach_desc.setFinalLayout   ( vk::ImageLayout::eColorAttachmentOptimal ) ;
      
      attach_ref.setLayout    ( ::vk::ImageLayout::eGeneral ) ;
      attach_ref.setAttachment( 0                           ) ;
      
      // Default to one set of descriptions for each. By default, this render pass describes a simple RGBA8 Render Pass with one color attachment.
      this->attach_descriptions.push_back( attach_desc ) ;
      this->attach_references  .push_back( attach_ref  ) ;
      
      sub_desc.setPipelineBindPoint   ( ::vk::PipelineBindPoint::eGraphics ) ;
      sub_desc.setColorAttachmentCount( 1                                  ) ;
      sub_desc.setPColorAttachments   ( &this->attach_references[ 0 ]      ) ;

      dependency.srcSubpass = VK_SUBPASS_EXTERNAL ;
      dependency.dstSubpass = 0 ;
      dependency.setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput ) ;
      dependency.setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput ) ;
      dependency.setDstAccessMask( vk::AccessFlagBits::eColorAttachmentWrite ) ;

      this->viewports.push_back( viewport ) ;
      
      this->sub_dependencies.push_back( dependency ) ;
      this->sub_descriptions.push_back( sub_desc   ) ;
      this->width   = 0 ;
      this->height  = 0 ;
      this->layers  = 0 ;
      this->current = 0 ;
    }
    
    ::vk::RenderPass RenderPassData::makeRenderPass()
    {
      ::vk::RenderPassCreateInfo info ;
      
      info.setAttachmentCount( this->attach_descriptions.size() ) ;
      info.setSubpassCount   ( this->sub_descriptions.size()    ) ;
      info.setDependencyCount( this->sub_dependencies.size()    ) ;
      info.setPAttachments   ( this->attach_descriptions.data() ) ;
      info.setPSubpasses     ( this->sub_descriptions.data()    ) ;
      info.setPDependencies  ( this->sub_dependencies.data()    ) ;
      
      auto result = this->device.device().createRenderPass( info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      return result.value ;
    }

    void RenderPassData::makeFramebuffers()
    {
      vk::FramebufferCreateInfo info      ;
      unsigned                  index     ;
      vk::ImageView             view[ 1 ] ;

      index = 0 ;
      info.setRenderPass( this->render_pass ) ;
      
      for( auto& image : this->images )
      {
        auto format = nyx::vkg::Vulkan::convert( this->attach_descriptions[ 0 ].format ) ;
        
        image.initialize( this->device, format, this->width, this->height, this->layers ) ;

        view[ 0 ] = image.view() ;
        
        /** TODO: Make this configurable. There are valid use cases for :
         * Different image formats. Maybe RGBA32F framebuffer attachments?
         * More than one attachment per framebuffer. Maybe depth and color attachments?
         */
        info.setAttachmentCount( 1                 ) ; 
        info.setPAttachments   ( view              ) ;
        info.setWidth          ( this->width       ) ;
        info.setHeight         ( this->height      ) ;
        info.setLayers         ( this->layers      ) ;
        info.setRenderPass     ( this->render_pass ) ;
        
        vkg::Vulkan::add( this->device.device().createFramebuffer( &info, nullptr, &this->framebuffers[ index ] ) ) ;
        
        index++ ;
      }
    }
    
    void RenderPassData::makeFramebuffers( const nyx::vkg::Swapchain& chain )
    {
      vk::FramebufferCreateInfo info      ;
      vk::ImageView             view[ 1 ] ;

      info.setRenderPass( this->render_pass ) ;

      this->framebuffers.resize( chain.count() ) ;
      this->images.resize      ( chain.count() ) ;
      for( unsigned index = 0; index < chain.count(); index++ )
      {
        this->images[ index ] = chain.images()[ index ] ;
        view[ 0 ] = chain.images()[ index ].view() ;
        
        /** TODO: Make this configurable. There are valid use cases for :
         * Different image formats. Maybe RGBA32F framebuffer attachments?
         * More than one attachment per framebuffer. Maybe depth and color attachments?
         */
        info.setAttachmentCount( 1                 ) ; 
        info.setPAttachments   ( view              ) ;
        info.setWidth          ( chain.width()     ) ;
        info.setHeight         ( chain.height()    ) ;
        info.setLayers         ( 1                 ) ;

        vkg::Vulkan::add( this->device.device().createFramebuffer( &info, nullptr, &this->framebuffers[ index ] ) ) ;
      }
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

    void RenderPass::initialize( unsigned device )
    {
      if( !Vulkan::initialized() ) Vulkan::initialize() ;

      data().device = Vulkan::device( device ) ;
      
      data().area.extent.width  = data().width  ;
      data().area.extent.height = data().height ;
      data().render_pass = data().makeRenderPass() ;
      data().makeFramebuffers() ;
    }
    
    void RenderPass::initialize( const nyx::vkg::Swapchain& swapchain )
    {
      if( !Vulkan::initialized() ) Vulkan::initialize() ;

      data().device = Vulkan::device( swapchain.device() ) ;
      data().width  = swapchain.width()                    ;
      data().height = swapchain.height()                   ; 
      
      data().area.extent.width  = data().width  ;
      data().area.extent.height = data().height ;

      data().viewports[ 0 ].width  = data().width ;
      data().viewports[ 0 ].height = data().height ;

      data().attach_descriptions[ 0 ].setFormat( swapchain.format() ) ;

      data().render_pass = data().makeRenderPass() ;
      data().makeFramebuffers( swapchain ) ;
    }
    
    bool RenderPass::initialized() const
    {
      return data().render_pass ;
    }

    const vk::Rect2D& RenderPass::area() const
    {
      return data().area ;
    }
    
    const vk::Rect2D* RenderPass::scissors() const
    {
      return &data().scissor ;
    }

    const vk::Rect2D& RenderPass::scissor() const
    {
      return data().scissor ;
    }
    
    const vk::Viewport* RenderPass::viewports() const
    {
      return data().viewports.data() ;
    }

    const vk::Viewport& RenderPass::viewport( unsigned idx ) const
    {
      static const vk::Viewport dummy ;
      if( idx < data().viewports.size() ) return data().viewports[ idx ] ;
      
      return dummy ;
    }
    
    const vk::ClearValue& RenderPass::clearColors() const
    {
      return data().clear_color ;
    }

    unsigned RenderPass::device() const
    {
      return data().device ;
    }
    
    const ::vk::Framebuffer* RenderPass::framebuffers() const
    {
      return data().framebuffers.data() ;
    }

    const ::vk::RenderPass& RenderPass::pass() const
    {
      return data().render_pass ;
    }
    
    const nyx::vkg::Image& RenderPass::image( unsigned idx ) const
    {
      static const nyx::vkg::Image dummy ;
      
      if( idx < data().images.size() ) return data().images[ idx ] ;
      
      return dummy ;
    }

    unsigned RenderPass::numFramebuffers() const
    {
      return data().framebuffers.size() ;
    }

    void RenderPass::setScissorExtentX( unsigned param )
    {
      data().scissor.extent.width = param ;
    }

    void RenderPass::setScissorExtentY( unsigned param )
    {
      data().scissor.extent.height = param ;
    }

    void RenderPass::setScissorOffsetX( float param )
    {
      data().scissor.offset.x = param ;
    }

    void RenderPass::setScissorOffsetY( float param )
    {
      data().scissor.offset.y = param ;
    }

    void RenderPass::setViewportX( unsigned id, float param )
    {
      const unsigned sz = id + 1 ;
      if( sz > data().viewports.size() ) { data().viewports.resize( sz ) ; data().ratios.resize( sz ) ; }
      
      data().viewports[ id ].x = param ;
    }
    
    void RenderPass::setViewportY( unsigned id, float param )
    {
      const unsigned sz = id + 1 ;
      if( sz > data().viewports.size() ) { data().viewports.resize( sz ) ; data().ratios.resize( sz ) ; }
      
      data().viewports[ id ].y = param ;
    }

    void RenderPass::setViewportWidth( unsigned id, float param )
    {
      const unsigned sz = id + 1 ;
      if( sz > data().ratios.size() ) { data().viewports.resize( sz ) ; data().ratios.resize( sz ) ; }
      
      data().ratios[ id ].width_ratio = param ;
    }

    void RenderPass::setViewportHeight( unsigned id, float param )
    {
      const unsigned sz = id + 1 ;
      if( sz > data().ratios.size() ) { data().viewports.resize( sz ) ; data().ratios.resize( sz ) ; }
      
      data().ratios[ id ].height_ratio = param ;
    }

    void RenderPass::setViewportMinDepth( unsigned id, float param )
    {
      const unsigned sz = id + 1 ;
      if( sz > data().viewports.size() ) { data().viewports.resize( sz ) ; data().ratios.resize( sz ) ; }
      
      data().viewports[ id ].minDepth = param ;
    }

    void RenderPass::setViewportMaxDepth( unsigned id, float param )
    {
      const unsigned sz = id + 1 ;
      if( sz > data().viewports.size() ) { data().viewports.resize( sz ) ; data().ratios.resize( sz ) ; }
      
      data().viewports[ id ].maxDepth = param ;
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
      data().area.extent.width = width ;
      data().width = width ;
    }

    void RenderPass::setFramebufferHeight( unsigned height )
    {
      data().area.extent.height = height ;
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
    
    void RenderPass::setFinalLayout( nyx::ImageLayout layout, unsigned idx )
    {
      if( idx < data().attach_descriptions.size() ) data().attach_descriptions[ idx ].setFinalLayout( vkg::Vulkan::convert( layout ) ) ;
    }
    
    void RenderPass::setClearColor( float red, float green, float blue, float alpha )
    {
      std::array<float, 4> clear_colors = { red, green, blue, alpha } ;
      
      data().clear_color.setColor( static_cast<vk::ClearColorValue>( clear_colors ) ) ;
    }
    
    const vk::Framebuffer& RenderPass::next() const
    {
      unsigned id = data().current ;
      
      data().current++ ;
      if( data().current >= data().framebuffers.size() ) data().current = 0 ;
      
      return data().framebuffers[ id ] ;
    }
    
    void RenderPass::reset()
    {
      data().device.device().destroy( data().render_pass, nullptr ) ;
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
