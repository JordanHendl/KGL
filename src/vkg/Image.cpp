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
 * File:   Image.cpp
 * Author: Jordan Hendl
 *
 * Created on December 27, 2020, 2:46 PM
 */

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT

#include "Image.h"
#include "Device.h"
#include "Vulkan.h"
#include "Buffer.h"
#include <library/Memory.h>
#include <library/Image.h>
#include <vulkan/vulkan.hpp>
#include <algorithm>

namespace nyx
{
  namespace vkg
  {
    using Impl = nyx::vkg::Vulkan ;
    struct ImageData
    {
      vkg::Device                device       ; ///< The library device to use for all vulkan calls.
      nyx::Memory<Impl>          memory       ; ///< The underlying memory container.
      vk::MemoryRequirements     requirements ; ///< The memory requirements for this image.
      vk::ImageSubresourceLayers subresource  ; ///< The subresource layout describing this image.
      bool                       preallocated ; ///< Whether or not this image was preallocated or not.
      unsigned                   width        ; ///< The width of this image in pixels.
      unsigned                   height       ; ///< The height of this image in pixels.
      unsigned                   layers       ; ///< The number of layers of this image.
      unsigned                   num_mip      ; ///< The mip level of this image.
      vk::Image                  image        ; ///< The raw vulkan image handle.
      vk::ImageView              view         ; ///< The raw vulkan image view handle.
      vk::Sampler                sampler      ; ///< The raw vulkan image sampler.
      mutable ::vk::ImageLayout  layout       ; ///< The current vulkan layout of this image.
      mutable ::vk::ImageLayout  old_layout   ; ///< The old layout of this image.
      vk::Format                 format       ; ///< The current vulkan format of this image.
      vk::ImageType              type         ; ///< The vulkan image type for this image
      vk::SampleCountFlagBits    num_samples  ; ///< The number of samples to use for sampling this image.
      vk::ImageUsageFlags        usage_flags  ; ///< The usage flags for this image.

      /** Default constructor.
       */
      ImageData() ;      
      
      /** Helper method to create a vulkan image view.
       * @return A Valid vulkan image view.
       */
      vk::ImageView createView() ;
      
      /** Method to create a vulkan sampler.
       * @return A Valid vulkan sampler.
       */
      vk::Sampler createSampler() ;

      /** Method to create a vulkan image.
       * @return A Made vulkan image.
       */
      vk::Image createImage() ;
    };
    
    ImageData::ImageData()
    {
      const auto default_usage = ::vk::ImageUsageFlagBits::eSampled         |
                                 ::vk::ImageUsageFlagBits::eTransferSrc     |
                                 ::vk::ImageUsageFlagBits::eColorAttachment |
                                 ::vk::ImageUsageFlagBits::eTransferDst ;
      


      this->width        = 0                             ;
      this->height       = 0                             ;
      this->layers       = 1                             ;
      this->num_mip      = 1                             ;
      this->format       = ::vk::Format::eR8G8B8A8Srgb   ;
      this->layout       = ::vk::ImageLayout::eUndefined ;
      this->old_layout   = ::vk::ImageLayout::eUndefined ;
      this->type         = ::vk::ImageType::e2D          ;
      this->usage_flags  = default_usage                 ;
      this->num_samples  = ::vk::SampleCountFlagBits::e1 ;
      this->preallocated = false                         ;

      this->subresource.setAspectMask    ( vk::ImageAspectFlagBits::eColor ) ;
      this->subresource.setBaseArrayLayer( 0                               ) ;
      this->subresource.setLayerCount    ( this->layers                    ) ;
      this->subresource.setMipLevel      ( 0                               ) ;
    }
    
    Image::Image()
    {
      this->img_data = new ImageData() ;
    }
    
    vk::ImageView ImageData::createView()
    {
      vk::ImageViewCreateInfo   info  ;
      vk::ImageSubresourceRange range ;

      range.setAspectMask    ( vk::ImageAspectFlagBits::eColor ) ; // @TODO: Make configurable.
      range.setBaseArrayLayer( 0                               ) ;
      range.setBaseMipLevel  ( 0                               ) ;
      range.setLayerCount    ( 1                               ) ;
      range.setLevelCount    ( 1                               ) ;

      info.setImage           ( this->image            ) ;
      info.setViewType        ( vk::ImageViewType::e2D ) ; // @TODO Make configurable.
      info.setFormat          ( this->format           ) ;
      info.setSubresourceRange( range                  ) ;
      
      auto result = device.device().createImageView( info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      return result.value ;
    }
    
    vk::Sampler ImageData::createSampler()
    {
      const auto max_anisotropy = 16.0f ;
      
      vk::SamplerCreateInfo info ;
      
      // @TODO make this configurable.
      info.setMagFilter              ( ::vk::Filter::eNearest                   ) ;
      info.setMinFilter              ( ::vk::Filter::eNearest                   ) ;
      info.setAddressModeU           ( ::vk::SamplerAddressMode::eClampToBorder ) ;
      info.setAddressModeV           ( ::vk::SamplerAddressMode::eClampToBorder ) ;
      info.setAddressModeW           ( ::vk::SamplerAddressMode::eClampToBorder ) ;
      info.setBorderColor            ( ::vk::BorderColor::eIntTransparentBlack  ) ;
      info.setCompareOp              ( ::vk::CompareOp::eNever                  ) ;
      info.setMipmapMode             ( ::vk::SamplerMipmapMode::eNearest        ) ;
      info.setAnisotropyEnable       ( ::vk::Bool32( false )                    ) ;
      info.setUnnormalizedCoordinates( ::vk::Bool32( false )                    ) ;
      info.setCompareEnable          ( ::vk::Bool32( false )                    ) ;
      info.setMaxAnisotropy          ( max_anisotropy                           ) ;
      info.setMipLodBias             ( 0.0f                                     ) ;
      info.setMinLod                 ( 0.0f                                     ) ;
      info.setMaxLod                 ( 0.0f                                     ) ;
      
      auto result = device.device().createSampler( info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      return result.value ;
    }

    vk::Image ImageData::createImage()
    {
      ::vk::ImageCreateInfo info   ;
      ::vk::Extent3D        extent ;
      
      extent.setWidth ( this->width  ) ;
      extent.setHeight( this->height ) ;
      extent.setDepth ( this->layers ) ;
      
      info.setExtent       ( extent                        ) ;
      info.setUsage        ( this->usage_flags             ) ;
      info.setFormat       ( this->format                  ) ;
      info.setImageType    ( this->type                    ) ;
      info.setSamples      ( this->num_samples             ) ;
      info.setMipLevels    ( this->num_mip                 ) ;
      info.setArrayLayers  ( this->layers                  ) ;
      info.setInitialLayout( this->layout                  ) ;
      info.setSharingMode  ( ::vk::SharingMode::eExclusive ) ;
      info.setTiling       ( ::vk::ImageTiling::eOptimal   ) ;

      auto result = this->device.device().createImage( info, nullptr ) ;
      vkg::Vulkan::add( result.result ) ;
      return result.value ;
    }

    Image::Image( const Image& orig )
    {
      this->img_data = new ImageData() ;
    
      *this = orig ;
    }

    Image::~Image()
    {
      delete this->img_data ;
    }

    Image& Image::operator=( const Image& src )
    {
      *this->img_data = *src.img_data ;

      return *this ;
    }

    void Image::copy( const Image& src, const nyx::vkg::CommandBuffer& buffer )
    {
      vk::ImageCopy info    ;
      vk::Extent3D  extent  ;

      extent.setHeight( std::min( this->height(), src.height() ) ) ;
      extent.setWidth ( std::min( this->width (), src.width () ) ) ;
      extent.setDepth ( std::min( this->layers(), src.layers() ) ) ;
      
      info.setExtent        ( extent                 ) ;
      info.setSrcOffset     ( 0                      ) ;
      info.setDstOffset     ( 0                      ) ;
      info.setSrcSubresource( src.data().subresource ) ;
      info.setDstSubresource( data().subresource     ) ;
      
      
      this->transition( nyx::vkg::Vulkan::convert( vk::ImageLayout::eTransferDstOptimal ), buffer ) ; 
      src .transition ( nyx::vkg::Vulkan::convert( vk::ImageLayout::eTransferSrcOptimal ), buffer ) ; 
      for( unsigned index = 0; index < buffer.size(); index++ )
      {
        buffer.buffer( index ).copyImage( src.data().image, src.data().layout, data().image, data().layout, 1, &info ) ;
      }
      src .revertLayout ( buffer ) ;
      this->revertLayout( buffer ) ;
      
    }
    
    void Image::copy( const nyx::vkg::Buffer& src, const nyx::vkg::CommandBuffer& buffer )
    {
      vk::BufferImageCopy info   ;
      vk::Extent3D        extent ;
      vk::Offset3D        offset ;
      
      offset.setX( 0 ) ;
      offset.setY( 0 ) ;
      offset.setZ( 0 ) ;
      
      extent.setHeight( this->height() ) ;
      extent.setWidth ( this->width () ) ;
      extent.setDepth ( this->layers() ) ;
      
      info.setBufferImageHeight( 0                  ) ;
      info.setBufferOffset     ( 0                  ) ;
      info.setBufferRowLength  ( 0                  ) ;
      info.setImageOffset      ( offset             ) ;
      info.setImageSubresource ( data().subresource ) ;
      info.setImageExtent      ( extent             ) ;
      
      
      this->transition( nyx::ImageLayout::TransferDst, buffer ) ; 
      for( unsigned index = 0; index < buffer.size(); index++ )
      {
        buffer.buffer( index ).copyBufferToImage( src.buffer(), data().image, vk::ImageLayout::eTransferDstOptimal, 1, &info ) ;
      }
      this->revertLayout( buffer ) ;
    }
    
    bool Image::initialized() const
    {
      if( !data().device.initialized() ) return false ;
      if( !data().image                ) return false ;
      if( !data().sampler              ) return false ;
      if( !data().view                 ) return false ;

      return true ;
    }

    bool Image::initialize( const vkg::Device& device, nyx::ImageFormat format, unsigned width, unsigned height, unsigned num_layers )
    {
      data().device = device     ;
      data().width  = width      ;
      data().height = height     ;
      data().layers = num_layers ;
      
      data().format = nyx::vkg::Vulkan::convert( format ) ;
      data().subresource.setLayerCount( num_layers ) ;
      
      data().image = data().createImage() ;

      data().requirements = device.device().getImageMemoryRequirements( data().image ) ;
      
      if( !data().preallocated )
      {
        data().memory.initialize( device, data().requirements.size, data().requirements.memoryTypeBits, false ) ;
      }
      
      if( data().requirements.size <= data().memory.size() - data().memory.offset() )
      {
        vkg::Vulkan::add( device.device().bindImageMemory( data().image, data().memory.memory(), data().memory.offset() ) ) ;
        
        data().view    = data().createView ()   ;
        data().sampler = data().createSampler() ;
        
        return true ;
      }
      
      return false ;
    }

    bool Image::initialize( const nyx::vkg::Device& gpu, nyx::ImageFormat format, unsigned width, unsigned height, vk::Image prealloc, unsigned num_layers )
    {
      data().device = gpu        ;
      data().width  = width      ;
      data().height = height     ;
      data().layers = num_layers ;
      
      data().image   = prealloc                            ;
      data().format  = nyx::vkg::Vulkan::convert( format ) ;
      data().view    = data().createView ()                ;
      data().sampler = data().createSampler()              ;

      return true ;
    }
    
    bool Image::initialize( nyx::Memory<nyx::vkg::Vulkan>& prealloc, nyx::ImageFormat format, unsigned width, unsigned height, unsigned num_layers )
    {
      data().memory       = prealloc ;
      data().preallocated = true     ;
      
      return this->initialize( prealloc.device(), format, width, height, num_layers ) ;
    }

    void Image::resize( unsigned width, unsigned height )
    {
      if( data().width != width && data().height != height )
      {
        this->reset() ;
        this->initialize( data().device, nyx::vkg::Vulkan::convert( data().format ), width, height ) ;
      }
    }
    
    void Image::setUsage( const nyx::ImageUsage& usage )
    {
      data().usage_flags = nyx::vkg::Vulkan::convert( usage ) ;
    }
    
    void Image::setType( const nyx::ImageType& type )
    {
      data().type = nyx::vkg::Vulkan::convert( type ) ;
    }

    void Image::setNumSamples( unsigned samples )
    {
      samples = samples ;
      data().num_samples = vk::SampleCountFlagBits::e1 ; //TODO make this work.
    }
    
    void Image::setMipLevels( unsigned mip_levels )
    {
      data().num_mip = mip_levels ;
      data().subresource.setMipLevel( mip_levels ) ;
    }

    void Image::setFormat( const nyx::ImageFormat& format )
    {
      data().format = nyx::vkg::Vulkan::convert( format ) ;
    }

    void Image::setLayout( const nyx::ImageLayout& layout )
    {
      data().layout = nyx::vkg::Vulkan::convert( layout ) ;
    }

    void Image::transition( const nyx::ImageLayout& layout, const nyx::vkg::CommandBuffer& cmd_buff ) const
    {
      vk::ImageMemoryBarrier    barrier    ;
      vk::ImageSubresourceRange range      ;
      vk::PipelineStageFlags    src        ;
      vk::PipelineStageFlags    dst        ;
      vk::DependencyFlags       dep_flags  ;
      vk::ImageLayout           new_layout ;
      
      new_layout = nyx::vkg::Vulkan::convert( layout ) ;
      
      range.setBaseArrayLayer( 0                               ) ;
      range.setBaseMipLevel  ( 0                               ) ;
      range.setLevelCount    ( 1                               ) ;
      range.setLayerCount    ( this->layers()                  ) ;
      range.setAspectMask    ( vk::ImageAspectFlagBits::eColor ) ;

      barrier.setOldLayout       ( data().layout                    ) ;
      barrier.setNewLayout       ( new_layout                       ) ;
      barrier.setImage           ( data().image                     ) ;
      barrier.setSubresourceRange( range                            ) ;
      barrier.setSrcAccessMask   ( vk::AccessFlagBits::eMemoryWrite ) ;
      barrier.setDstAccessMask   ( vk::AccessFlagBits::eMemoryRead  ) ;
      
      dep_flags = vk::DependencyFlagBits::eDeviceGroupKHR ;
      src       = vk::PipelineStageFlagBits::eAllCommands ;
      dst       = vk::PipelineStageFlagBits::eAllCommands ;
      
      for( unsigned index = 0; index < cmd_buff.size(); index++ )
      {
        cmd_buff.buffer( index ).pipelineBarrier( src, dst, dep_flags, 0, nullptr, 0, nullptr, 1, &barrier ) ;
      }
      
      data().old_layout = data().layout ;
      data().layout     = new_layout    ;
    }

    void Image::revertLayout( const nyx::vkg::CommandBuffer& cmd_buff ) const 
    {
      if( data().old_layout != vk::ImageLayout::eUndefined )
      {
        this->transition( nyx::vkg::Vulkan::convert( data().old_layout ), cmd_buff ) ;
      }
    }
    
    unsigned Image::size() const
    {
      return data().width * data().height * data().layers ;
    }
    
    unsigned Image::byteSize() const
    {
      return data().memory.size() ;
    }

    const ::vk::Sampler& Image::sampler() const
    {
      return data().sampler ;
    }
    
    const ::vk::ImageView& Image::view() const
    {
      return data().view ;
    }
    
    unsigned Image::width() const
    {
      return data().width ;
    }

    unsigned Image::height() const
    {
      return data().height ;
    }

    unsigned Image::layers() const
    {
      return data().layers ;
    }

    void Image::reset()
    {
      if( data().image )
      {
        data().device.device().destroy( data().sampler ) ;
        data().device.device().destroy( data().view    ) ;
        data().device.device().destroy( data().image   ) ;
      }
      
      if( !data().preallocated )
      {
        data().memory.deallocate() ;
      }
    }

    ImageData& Image::data()
    {
      return *this->img_data ;
    }

    const ImageData& Image::data() const
    {
      return *this->img_data ;
    }
  }
}
