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
 * File:   RenderPass.h
 * Author: jhendl
 *
 * Created on March 21, 2021, 3:43 PM
 */

#pragma once

#include "Image.h"
#include "Array.h"
#include <vector>

namespace nyx
{
  enum class PipelineStage : unsigned
  {
    Vertex,
    Fragment,
    Compute,
    TessC,
  };
  
  template<typename Framework>
  class RenderPass ;

  template<typename Framework>
  class Memory ;
  
  template<typename Framework, typename Type>
  class Array ;
  
  template<typename Framework>
  class Chain ;

  class Attachment
  {
    public:
      Attachment() ;
      ~Attachment() ;
      void setClearColor( float red, float green, float blue, float alpha ) ;
      void setStencilTest( bool value ) ;
      void setStencilClear( bool value ) ;
      void setStencilStore( bool value ) ;
      void setFormat( nyx::ImageFormat format ) ;
      void setLayout( nyx::ImageLayout layout ) ;
      
      bool clearStencil() const ;
      bool testStencil() const ;
      bool storeStencil() const ;
      nyx::ImageFormat format() const ;
      nyx::ImageLayout layout() const ;
      
      float red() const ;
      float green() const ;
      float blue() const ;
      float alpha() const ;
    private:
      float    r, g, b, a    ;
      bool     test_stencil  ;
      bool     store_stencil ;
      bool     clear_stencil ;
      unsigned img_format    ;
      unsigned img_layout    ;
  };

  class Subpass
  {
    public:
      
      inline void addAttachment( nyx::Attachment attachment )
      {
        this->attachment_deps.push_back( attachment ) ;
      };
      
      inline void setAttachment( unsigned index, nyx::Attachment attachment )
      {
        if( this->attachment_deps.size() < index )
        {
          this->attachment_deps[ index ] = attachment ;
        }
      }

      inline void addSubpassDependancy( unsigned subpass_index )
      {
        this->subpass_deps.push_back( subpass_index ) ;
      };
      
      inline void setSubpassDependancy( unsigned index, unsigned subpass_index )
      {
        if( this->subpass_deps.size() < index )
        {
          this->subpass_deps[ index ] = subpass_index ;
        }
      };
      
      inline void setDepthStencilEnable( bool val )
      {
        this->depth_stencil_enable = val ;
      }
      
      inline void setDepthClearValue( float val )
      {
        this->depth_clear = val ;
      }
      
    private:
      template<typename Framework>
      friend class RenderPass ;

      std::vector<nyx::Attachment> attachment_deps ;
      std::vector<unsigned>        subpass_deps    ;
      
      bool  depth_stencil_enable = false ;
      float depth_clear          = 1.0f  ;
  };

  template<typename Framework>
  class RenderPass
  {
    public:
      /** Default constructor.
       */
      RenderPass() = default ;
      
      /** Default deconstructor.
       */
      ~RenderPass() = default ;
      
      /** Method to convert this object to the underlying implementation-specific version.
       * @return The implementation-specific version of this object.
       */
      operator const typename Framework::RenderPass&() const ;
      
      /** Method to initialize this object.
       * @param device The device to use for all GPU operations.
       * @param nyx_file_path The path to the .nyx file on the filesystem to use.
       * @param context The Framework context for this renderer to draw to.
       */
      void initialize( unsigned device ) ;
      
      /** Method to initialize this object.
       * @param device The device to use for all GPU operations.
       * @param nyx_file_bytes The bytes of the .nyx file to use for this object.
       * @param size The size of the bytes array.
       * @param context The Framework context for this renderer to draw to.
       */
      void initialize( unsigned device, unsigned window_id ) ;
      
      /**
       * @return 
       */
      bool initialized() const ;
 
      /** Method to set the dimensions of this render pass's output attachments.
       * @param width The width in pixels of this object's output images.
       * @param height The height in pixels of this object's output images.
       */
      void setDimensions( unsigned width, unsigned height ) ;
 
      /** Method to retrieve the number of framebuffers generated by this renderer.
       * @return The number of framebuffers generated by this renderer.
       */
      unsigned count() const ;
 
      /** Method to retrieve the device used by this renderer.
       * @return Const reference to the device used for this renderer.
       */
      unsigned device() const ;
      
      /** Method to retrieve a framebuffer from this object.
       * @param index The index of framebuffer to retrieve. See @count for the amount.
       * @return Const reference to this object's internal framebuffer.
       */
      template<unsigned index = 0>
      const auto framebuffer() const ;
      
      void reset() ;
      
      /** Method to present this render pass to the screen, if initialized with a window.
       * @return Whether or not this render pass had to reallocate data based on window events.
       */
      bool present() ;
      
      void addSubpass( const nyx::Subpass& attachment ) ;
      
    private:
      typename Framework::RenderPass impl ;
  };
 
  template<typename Framework>
  RenderPass<Framework>::operator const typename Framework::RenderPass& () const
  {
    return this->impl ;
  }
 
  template<typename Framework>
  bool RenderPass<Framework>::initialized() const
  {
    return this->impl.initialized() ;
  }

  template<typename Framework>
  void RenderPass<Framework>::initialize( unsigned device )
  {
    this->impl.initialize( device ) ;
  }
 
  template<typename Framework>
  void RenderPass<Framework>::initialize( unsigned device, unsigned window_id )
  {
    this->impl.initialize( device, window_id ) ;
  }
 
  template<typename Framework>
  unsigned RenderPass<Framework>::count() const
  {
    return this->impl.count() ;
  }
 
  template<typename Framework>
  void RenderPass<Framework>::addSubpass( const nyx::Subpass& subpass )
  {
    this->impl.addSubpass( subpass.attachment_deps.data(), subpass.attachment_deps.size(),
                           subpass.subpass_deps   .data(), subpass.subpass_deps   .size(), 
                           subpass.depth_stencil_enable  , subpass.depth_clear           ) ;
  }

  template<typename Framework>
  unsigned RenderPass<Framework>::device() const
  {
    return this->impl.device() ;
  }
  
  template<typename Framework>
  bool RenderPass<Framework>::present()
  {
    return this->impl.present() ;
  }

  template<typename Framework>
  template<unsigned index>
  const auto RenderPass<Framework>::framebuffer() const
  {
    return this->impl.framebuffer() ;
  }
 
  template<typename Framework>
  void RenderPass<Framework>::reset()
  {
    this->impl.reset() ;
  }

  template<typename Framework>
  void RenderPass<Framework>::setDimensions( unsigned width, unsigned height )
  {
    this->impl.setDimensions( width, height ) ;
  }
}