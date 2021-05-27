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
#include <library/Image.h>

namespace nyx
{
  namespace vkg
  {
    struct RendererData
    {

      const vkg::RenderPass* pass            ;
      vkg::NyxShader         shader          ;
      vkg::Pipeline          pipeline        ;
      vkg::DescriptorPool    pool            ;
      vkg::Descriptor        descriptor      ;
      unsigned               device          ;
      unsigned               width           ;
      unsigned               height          ;
      unsigned               sample_count    ;
      unsigned long long     window_id       ;
      
      RendererData() ;
      
      void remake() ;
    };
    
    void RendererData::remake()
    {
      this->pipeline.reset() ;
      
      this->pipeline.initialize( *this->pass, this->shader ) ;
    }

    RendererData::RendererData()
    {
      this->sample_count    = 1   ;
      this->window_id         = 0x0 ;
    }

    RendererImpl::RendererImpl()
    {
      this->renderer_data = new RendererData() ;
    }

    RendererImpl::~RendererImpl()
    {
      delete this->renderer_data ;
    }

    void RendererImpl::initialize( unsigned device, const vkg::RenderPass& pass, const char* nyx_file_path )
    {
      
      data().device = device ;
      data().pass   = &pass  ;

      data().shader  .initialize( device, nyx_file_path ) ;
      data().pipeline.initialize( pass  , data().shader ) ;
      data().pool    .initialize( data().shader, 1      ) ;
      
      data().descriptor = data().pool.make() ;
    }

    void RendererImpl::initialize( unsigned device, const vkg::RenderPass& pass, const unsigned char* nyx_file_bytes, unsigned size )
    {
      data().device = device ;
      data().pass   = &pass  ;

      data().shader  .initialize( device, nyx_file_bytes, size ) ;
      data().pipeline.initialize( pass  , data().shader        ) ;
      data().pool    .initialize( data().pipeline.shader(), 1  ) ;
      
      data().descriptor = data().pool.make() ;
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
      data().pipeline.addViewport( viewport ) ;
      
      if( data().pipeline.initialized() )
      {
        data().pipeline.reset() ;
        data().pipeline.initialize( *data().pass, data().shader ) ;
      }
    }
    
    unsigned RendererImpl::device() const
    {
      return data().device ;
    }

//    void RendererImpl::pushConstantBase( const void* value, unsigned byte_size, nyx::PipelineStage stage_flags )
//    {
//      data().cmd.pushConstantBase( value, byte_size, stage_flags ) ;
//    }

//    void RendererImpl::finalize()
//    {
//      if( data().cmd.recording() )
//      {
//        data().cmd.stop() ;
//      }
//      
//      if( data().swapchain.initialized() )
//      {
//        if( data().swapchain.submit()  == Vulkan::Error::RecreateSwapchain ) data().remake() ;
//        if( data().swapchain.acquire() == Vulkan::Error::RecreateSwapchain ) data().remake() ;
//      }
//    }

    void RendererImpl::reset()
    {
      Vulkan::device( data().device ).wait() ;

      data().pipeline .reset() ;
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
    
    void Renderer::initialize( unsigned device, const vkg::RenderPass& pass, const char* nyx_file_path )
    {
      this->impl.initialize( device, pass, nyx_file_path ) ;
    }

    void Renderer::initialize( unsigned device, const vkg::RenderPass& pass, const unsigned char* nyx_file_bytes, unsigned size )
    {
      this->impl.initialize( device, pass, nyx_file_bytes, size ) ;
    }

    bool Renderer::initialized() const
    {
      return impl.data().pipeline.initialized() ;
    }
    
    const vkg::Descriptor& Renderer::descriptor() const
    {
      return impl.data().descriptor ;
    }

    const vkg::Pipeline& Renderer::pipeline() const
    {
      return impl.data().pipeline ;
    }

    void Renderer::addViewport( const nyx::Viewport& viewport )
    {
      this->impl.addViewport( viewport ) ;
    }

    void Renderer::bind( const char* name, const vkg::Image& image )
    {
      this->impl.bind( name, image ) ;
    }
    
    void Renderer::bind( const char* name, const vkg::Image* const* images, unsigned count )
    {
      Vulkan::deviceSynchronize( impl.data().device ) ;
      
      impl.data().descriptor.set( name, images, count ) ;
    }
    
    void Renderer::setTestDepth( bool val )
    {
      this->impl.data().pipeline.setTestDepth( val ) ;
    }
    
    void Renderer::reset()
    {
      this->impl.reset() ;
    }

    unsigned Renderer::device() const
    {
      return this->impl.device() ;
    }
  }
}
