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
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Swapchain.h"
#include "Descriptor.h"
#include "Pipeline.h"
#include "NyxShader.h"
#include "Vulkan.h"
#include <library/Image.h>

namespace nyx
{
  namespace vkg
  {
    static nyx::ImageLayout layout( nyx::ImageFormat format ) ;
    
    struct RendererData
    {
      vkg::RenderPass pass            ;
      vkg::Pipeline   pipeline        ;
      vkg::Swapchain  swapchain       ;
      unsigned        device          ;
      unsigned        amt_attachments ;
      unsigned        width           ;
      unsigned        height          ;
      unsigned        sample_count    ;

      RendererData() ;
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
      data().pass    .initialize( device                ) ;
      data().pipeline.initialize( device, nyx_file_path ) ;
    }
//
//    void RendererImpl::initialize( unsigned device, const char* nyx_file_path, const vkg::Surface& context )
//    {
//    
//    }
//
    void RendererImpl::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size )
    {
      data().pass    .initialize( device                            ) ;
      data().pipeline.initialize( data().pass, nyx_file_bytes, size ) ;
    }

    void RendererImpl::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const vkg::Surface& context )
    {
      data().swapchain.initialize( Vulkan::presentQueue( context, device ), context ) ;
      data().pass.setAttachmentFinalLayout( nyx::ImageLayout::PresentSrc                     , data().amt_attachments - 1 ) ;
      data().pass.setAttachmentFormat     ( vkg::Vulkan::convert( data().swapchain.format() ), data().amt_attachments - 1 ) ;
      data().pass.initialize( data().swapchain ) ;
      data().pipeline.initialize( data().pass, nyx_file_bytes, size ) ;
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
//    void RendererImpl::addViewport( const nyx::Viewport& viewport )
//    {
//    
//    }
//
//    void RendererImpl::bind( const char* name, const nyx::vkg::Buffer& buffer, unsigned element_size, unsigned count )
//    {
//    
//    }
//
    unsigned RendererImpl::count() const
    {
      return data().pass.numFramebuffers() ;
    }
//
    unsigned RendererImpl::current() const
    {
      return 0 ;
    }

    unsigned RendererImpl::device() const
    {
      return data().device ;
    }

//    void RendererImpl::drawBase( const nyx::vkg::Buffer& buffer, unsigned count, unsigned offset )
//    {
//    
//    }


    const vkg::Image& RendererImpl::image( unsigned index ) const
    {
      return data().pass.image( index ) ;
    }

//    void RendererImpl::drawIndexedBase( const nyx::vkg::Buffer& index, const nyx::vkg::Buffer& vert, unsigned index_count, unsigned vert_count, unsigned offset )
//    {
//    
//    }
//
//    void RendererImpl::pushConstantBase( const void* value, unsigned byte_size, nyx::PipelineStage stage_flags )
//    {
//    
//    }

    void RendererImpl::setDimensions( unsigned width, unsigned height )
    {
      data().pass.setFramebufferWidth ( width  ) ;
      data().pass.setFramebufferHeight( height ) ;
    }

    void RendererImpl::reset()
    {
    
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