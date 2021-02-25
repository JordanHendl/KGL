/*
 * Copyright (C) 2021 jhendl
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
 * File:   Renderer.cpp
 * Author: jhendl
 * 
 * Created on February 7, 2021, 11:28 AM
 */

#include "Renderer.h"
#include "Vulkan.h"
#include "library/Renderer.h"
#include <template/List.h>
#include <library/Image.h>

namespace nyx
{
  namespace vkg
  {
    static nyx::ImageLayout layout( nyx::ImageFormat format ) ;
    
    struct RendererData
    {

      vkg::RenderPass     pass            ;
      vkg::NyxShader      shader          ;
      vkg::Pipeline       pipeline        ;
      vkg::Swapchain      swapchain       ;
      vkg::CommandBuffer  cmd             ;
      vkg::Queue          queue           ;
      vkg::DescriptorPool pool            ;
      vkg::Descriptor     descriptor      ;
      unsigned            device          ;
      unsigned            amt_attachments ;
      unsigned            width           ;
      unsigned            height          ;
      unsigned            sample_count    ;

      RendererData() ;
      
      void remake() ;
    };
    
    nyx::ImageLayout layout( nyx::ImageFormat format )
    {
      switch( format )
      {
        case nyx::ImageFormat::R32F :
          return nyx::ImageLayout::DepthRead ;
        case nyx::ImageFormat::RGBA8   :
        case nyx::ImageFormat::RGB8    :
        case nyx::ImageFormat::R8      :
        case nyx::ImageFormat::RGBA32F :
        case nyx::ImageFormat::RGB32F  :
        default:
          return nyx::ImageLayout::ColorAttachment ;
      };
    }
    
    void RendererData::remake()
    {
      this->pass     .reset() ;
      this->pipeline .reset() ;
      this->cmd      .reset() ;
      
      this->pass.setScissorExtentX(   this->swapchain.width () ) ;
      this->pass.setScissorExtentY(   this->swapchain.height() ) ;
      this->pass.setViewportWidth (0, this->swapchain.width () ) ;
      this->pass.setViewportHeight(0, this->swapchain.height() ) ;

      this->pass    .initialize( this->swapchain          ) ;
      this->pipeline.initialize( this->pass, this->shader ) ;
      this->cmd     .initialize( this->queue, 1           ) ;
    }

    RendererData::RendererData()
    {
      this->amt_attachments = 0 ;
      this->sample_count    = 1 ;
    }

    RendererImpl::RendererImpl()
    {
      this->renderer_data = new RendererData() ;
    }

    RendererImpl::~RendererImpl()
    {
      delete this->renderer_data ;
    }

    void RendererImpl::initialize( unsigned device, const char* nyx_file_path )
    {
      nyx::vkg::NyxShader shader ;
      data().device = device                          ;
      data().queue  = Vulkan::graphicsQueue( device ) ;

      shader         .initialize( device, nyx_file_path ) ;
      data().cmd     .initialize( data().queue, 1       ) ;
      data().pass    .initialize( device                ) ;
      data().pipeline.initialize( device, nyx_file_path ) ;
      data().pool    .initialize( shader, 1             ) ;
      
      data().descriptor = data().pool.make() ;
    }

    void RendererImpl::initialize( unsigned device, const char* nyx_file_path, const vkg::Surface& context )
    {
      data().device = device                                  ;
      data().queue  = Vulkan::presentQueue( context, device ) ;

      data().cmd      .initialize( data().queue, 1       ) ;
      data().swapchain.initialize( data().queue, context ) ;
      data().shader   .initialize( device, nyx_file_path ) ;

      data().pass.setAttachmentFinalLayout( nyx::ImageLayout::PresentSrc                     , data().amt_attachments - 1 ) ;
      data().pass.setAttachmentFormat     ( vkg::Vulkan::convert( data().swapchain.format() ), data().amt_attachments - 1 ) ;
      
      data().pass    .initialize( data().swapchain           ) ;
      data().pipeline.initialize( data().pass, data().shader ) ;
      data().pool    .initialize( data().shader, 1           ) ;
      
      data().descriptor = data().pool.make() ;
      
      if( data().swapchain.acquire() == Vulkan::Error::RecreateSwapchain )
      {
        data().remake() ;
      }
    }
    
    void RendererImpl::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size )
    {
      data().device = device                          ;
      data().queue  = Vulkan::graphicsQueue( device ) ;

      data().shader  .initialize( device, nyx_file_bytes, size      ) ;
      data().cmd     .initialize( data().queue, 1                   ) ;
      data().pass    .initialize( device                            ) ;
      data().pipeline.initialize( data().pass, nyx_file_bytes, size ) ;
      data().pool    .initialize( data().shader, 1                  ) ;
      
      data().descriptor = data().pool.make() ;
    }

    void RendererImpl::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const vkg::Surface& context )
    {
      nyx::vkg::NyxShader shader ;
      data().device = device                                  ;
      data().queue  = Vulkan::presentQueue( context, device ) ;

      data().shader   .initialize( device, nyx_file_bytes, size ) ;
      data().cmd      .initialize( data().queue, 1              ) ;
      data().swapchain.initialize( data().queue, context        ) ;
      
      data().pass.setAttachmentFinalLayout( nyx::ImageLayout::PresentSrc                     , data().amt_attachments - 1 ) ;
      data().pass.setAttachmentFormat     ( vkg::Vulkan::convert( data().swapchain.format() ), data().amt_attachments - 1 ) ;

      data().pass     .initialize( data().swapchain           ) ;
      data().pipeline .initialize( data().pass, data().shader ) ;
      data().pool     .initialize( data().shader, 1           ) ;
      
      data().descriptor = data().pool.make() ;
      
      if( data().swapchain.acquire() == Vulkan::Error::RecreateSwapchain )
      {
        data().remake() ;
      }
    }

    void RendererImpl::addAttachment( nyx::ImageFormat format )
    {
      data().pass.setAttachmentFormat       ( format                     , data().amt_attachments ) ;
      data().pass.setAttachmentFinalLayout  ( vkg::layout( format )      , data().amt_attachments ) ;
      data().pass.setAttachmentInitialLayout( nyx::ImageLayout::Undefined, data().amt_attachments ) ;
      data().pass.setAttachmentNumSamples   ( data().sample_count        , data().amt_attachments ) ;

      if( format == nyx::ImageFormat::RGBA8 || format == nyx::ImageFormat::RGBA32F || format == nyx::ImageFormat::BGRA8 )
      {
        data().pass.setAttachmentLoad         ( false, data().amt_attachments ) ;
        data().pass.setAttachmentStore        ( true , data().amt_attachments ) ;
        data().pass.setAttachmentStencilLoad  ( false, data().amt_attachments ) ;
        data().pass.setAttachmentStencilStore ( false, data().amt_attachments ) ;
      }
      else
      {
        data().pass.setAttachmentLoad         ( false, data().amt_attachments ) ;
        data().pass.setAttachmentStore        ( false, data().amt_attachments ) ;
        data().pass.setAttachmentStencilLoad  ( false, data().amt_attachments ) ;
        data().pass.setAttachmentStencilStore ( true , data().amt_attachments ) ;
      }
      
      data().amt_attachments++ ;
    }
    
    void RendererImpl::bind( const char* name, const nyx::vkg::Buffer& buffer )
    {
      Vulkan::deviceSynchronize( data().device ) ;
      
      data().descriptor.set( name, buffer ) ;
    }
    
    void RendererImpl::bind( const char* name, const nyx::vkg::Image& image )
    {
      Vulkan::deviceSynchronize( data().device ) ;
      
      data().descriptor.set( name, image ) ;
    }
    
    void RendererImpl::addViewport( const nyx::Viewport& viewport )
    {
      data().pass.setViewportWidth ( 0, viewport.width()  ) ;
      data().pass.setViewportHeight( 0, viewport.height() ) ;
      data().pass.setViewportX     ( 0, viewport.xpos()   ) ;
      data().pass.setViewportY     ( 0, viewport.ypos()   ) ;
      
      if( data().pass.initialized() )
      {
        data().remake() ;
      }
    }
    
    unsigned RendererImpl::count() const
    {
      return data().pass.numFramebuffers() ;
    }

    unsigned RendererImpl::current() const
    {
      return data().swapchain.current() ;
    }

    unsigned RendererImpl::device() const
    {
      return data().device ;
    }

    void RendererImpl::drawBase( const nyx::vkg::Buffer& buffer, unsigned count, unsigned offset )
    {
      if( !data().cmd.recording() )
      {
        data().cmd.record( data().pass       ) ;
        data().cmd.bind  ( data().pipeline   ) ;
        data().cmd.bind  ( data().descriptor ) ;
      }

      data().cmd.drawBase( buffer, count, offset ) ;
    }

    void RendererImpl::drawIndexedBase( const nyx::vkg::Buffer& index, const nyx::vkg::Buffer& vert, unsigned index_count, unsigned vert_count, unsigned offset )
    {
      if( !data().cmd.recording() )
      {
        data().cmd.record( data().pass       ) ;
        data().cmd.bind  ( data().pipeline   ) ;
        data().cmd.bind  ( data().descriptor ) ;
      }

      data().cmd.drawIndexedBase( index, vert, index_count, vert_count, offset ) ;
    }

    const vkg::Image& RendererImpl::image( unsigned index ) const
    {
      return data().pass.image( index ) ;
    }

    void RendererImpl::pushConstantBase( const void* value, unsigned byte_size, nyx::PipelineStage stage_flags )
    {
      data().cmd.pushConstantBase( value, byte_size, stage_flags ) ;
    }

    void RendererImpl::setDimensions( unsigned width, unsigned height )
    {
      data().pass.setFramebufferWidth ( width  ) ;
      data().pass.setFramebufferHeight( height ) ;
    }
    
    void RendererImpl::finalize()
    {
      if( data().cmd.recording() )
      {
        data().cmd.stop() ;
        data().queue.submit( data().cmd ) ;
      }
      
      if( data().swapchain.initialized() )
      {
        if( data().swapchain.submit()  == Vulkan::Error::RecreateSwapchain ) data().remake() ;
        if( data().swapchain.acquire() == Vulkan::Error::RecreateSwapchain ) data().remake() ;
      }
    }

    void RendererImpl::reset()
    {
      Vulkan::device( data().device ).wait() ;

      data().pipeline .reset() ;
      data().pass     .reset() ;
      data().swapchain.reset() ;
      data().cmd      .reset() ;
      data().descriptor.reset() ;
//      data().pool     .reset() ;
    }

    RendererData& RendererImpl::data()
    {
      return *this->renderer_data ;
    }

    const RendererData& RendererImpl::data() const
    {
      return *this->renderer_data ;
    }
  }
}