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
 * File:   Image.h
 * Author: Jordan Hendl
 *
 * Created on December 27, 2020, 2:46 PM
 */

#pragma once

#include "Nyx.h"

namespace nyx
{
  template<typename Impl>
  class Memory ;

  enum class ImageFormat : unsigned 
  {
    R8,     ///< Single channel Char.
    R32I,   ///< Single channel Integer.
    R32F,   ///< Single channel Float.
    RGB8,   ///< Three channel Char.
    BGR8,   ///< Three channel Char.
    RGB32I, ///< Three channel Integer.
    RGB32F, ///< Three channel Float.
    RGBA8,  ///< Four channel Char.
    BGRA8,  ///< Three channel Char.
    RGBA32I,///< Four channel Integer.
    RGBA32F,///< Four channel Float.
    D32F,   ///< Since channel depth float .
  };
  
  enum class ImageLayout : unsigned
  {
    Undefined,       ///< Undefined.
    General,         ///< General purpose.
    ColorAttachment, ///< Color Attachment.
    ShaderRead,      ///< Only valid for shader read.
    TransferSrc,     ///< Only valid for copies from.
    TransferDst,     ///< Only valid for copies to.
    PresentSrc,      ///< Only valid for presenting.
    DepthRead,       ///< Only valid for being read from as depth.
    DepthStencil,    ///< Only valid for using as render pass attachmnent.
  };
    

  enum class ImageUsage : unsigned
  {
    TransferSrc,          ///<  Transfer Source.
    TransferDst,          ///<  Transfer Destination.
    Sampled,              ///<  Image to be sampled.
    Storage,              ///<  Storage.
    ColorAttachment,      ///<  Color Attachment.
    DepthStencil,         ///<  Depth Stencil attachment.
    Input,                ///<  InputAttachment.
    ShadingRate,          ///<  Shading rate bit.
    VKEXTFragmentDensity, ///<  Vulkan Image Fragment Density Map ( EXT only ).
  };
  
  enum class ImageType : unsigned
  {
    n1D = 1u,
    n2D = 2u,
    n3D = 3u,  
  };

  /** Generic Image object.
   */
  template<typename Impl>
  class Image
  {
    public:
      
      /** Default constructor.
       */
      Image() = default ;
      
      /** Copy constructor. Copies the input image.
       */
      inline Image( const Image<Impl>& image ) ;
      
      /** Copy constructor with an framework's texture object as input. Copies texture object into this one.
       * @warning This CAN take in a texture of the incorrect 
       * @param texture
       */
      inline Image( const typename Impl::Texture& texture ) ;

      /** Default deconstructor.
       */
      ~Image() = default ;
      
      /** Assignment operator. Assigns this image to the input.
       * @param image The image to assign this object to.
       * @return Reference to this image after the assignment.
       */
      inline Image& operator=( const Image<Impl>& image ) ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      inline operator const typename Impl::Texture&() const ;

      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      inline operator typename Impl::Texture&() ;
      
      /** Method to perform a deep copy on the input image.
       * @param src The image to copy from.
       * @param cmd Reference to a valid command record to record the copy operation to.
       */
      inline void copy( const Image& src, typename Impl::Queue& cmd ) ;

      /** Method to perform a deep copy on the input image.
       * @param src The image to copy from.
       * @param cmd Reference to a valid command record to record the copy operation to.
       */
      inline void copy( const typename Impl::Buffer& src, typename Impl::Queue& cmd ) ;

      /** Method to retrieve the device associated with this image.
       * @return the GPU Device associated with this image.
       */
      inline unsigned device() const ;
      
      /** Method to check whether this object is initialized or not.
       * @return Whether this object is initialized or not.
       */
      inline bool initialized() const ;

      /** Method to initialize this image with the input parameters.
       * @param gpu The Implementation-specific GPU to use for this image.
       * @param width The width of the image in pixels.
       * @param height The height of the image in pixels.
       * @param layers The number of layers in the image.
       * @return Whether or not this image was able to successfully initialize.
       */
      inline bool initialize( nyx::ImageFormat format, unsigned gpu, unsigned width, unsigned height, unsigned layers = 1 ) ;
      
      /** Method to initialize this image with preallocated memory.
       * @param prealloc The preallocated memory to use for this image's memory.
       * @param width The width of this image in pixels.
       * @param height The height of this image in pixels.
       * @param layers The number of layers in this image.
       * @return Whether or not this image was able to successfully initialize.
       */
      inline bool initialize( nyx::ImageFormat format, const Memory<Impl>& prealloc, unsigned width, unsigned height, unsigned layers = 1 ) ;
      
      /** Method to retrieve the format of this image.
       * @return The format of this image.
       */
      inline nyx::ImageFormat format() ;

      /** Method to retrieve the width of this image.
       * @return The width of this image in pixels.
       */
      inline unsigned width() const ;
      
      /** Method to retrieve the height of this image.
       * @return The height of this image in pixels.
       */
      inline unsigned height() const ;
      
      /** Method to retrieve the number of layers there are in this image.
       * @return The number of layers there are of this image.
       */
      inline unsigned layers() const ;
      
      /** Method to resize this object to the desired width and height.
       * @return Whether or not this object performed a resize.
       * @param width The width in pixels to resize this object to.
       * @param height The Height in pixels to resize this object to.
       */
      inline bool resize( unsigned width, unsigned height ) ;
      
      /** Method to set the mip-levels of this image.
       * @param num_levels The number of mip levels to use for this image.
       */
      inline void setMipLevels( unsigned num_levels ) ;
      
      /** Method to retrieve the size of this image.
       * @return The size of this image in pixels.
       */
      inline unsigned size() const ;
      
      /** Method to retrieve the byte size of this image.
       * @return The size of this byte image in pixels.
       */
      inline unsigned byteSize() const ;

      /** Method to reset this image and deallocate any allocated data.
       */
      inline void reset() ;
      
      /** Method to retrieve the layout of this image.
       * @return The layout of the image.
       */
      inline nyx::ImageLayout layout() const ;
      
      /** Method to return the a const-reference to the implementation-specific image handle this image contains.
       * @return Const-Reference to the implementation-specific image handle this image contains.
       */
      inline const typename Impl::Texture& image() const ;
      
      /** Method to return the a reference to the implementation-specific image handle this image contains.
       * @return Reference to the implementation-specific image handle this image contains.
       */
      inline typename Impl::Texture& image() ;
      
    private:
      /** The implementation specific texture handle.
       */
      typename Impl::Texture impl_image ;
  };

  template<typename Impl>
  Image<Impl>::Image( const Image<Impl>& image )
  {
    *this = image ;
  }
  
  template<typename Impl>
  Image<Impl>::Image( const typename Impl::Texture& image )
  {
    this->impl_image = image ;
  }
  
  template<typename Impl>
  Image<Impl>& Image<Impl>::operator=( const Image<Impl>& image )
  {
    this->impl_image = image.impl_image ;
    
    return *this ;
  }

  template<typename Impl>
  Image<Impl>::operator const typename Impl::Texture&() const
  {
    return this->impl_image ;
  }
  
  template<typename Impl>
  Image<Impl>::operator typename Impl::Texture&()
  {
    return this->impl_image ;
  }
  
  template<typename Impl>
  void Image<Impl>::copy( const Image& src, typename Impl::Queue& cmd ) 
  {
    this->impl_image.copy( src, cmd ) ;
  }
  
  template<typename Impl>
  void Image<Impl>::copy( const typename Impl::Buffer& src, typename Impl::Queue& cmd )
  {
    this->impl_image.copy( src, cmd ) ;
  }
  
  template<typename Impl>
  unsigned Image<Impl>::device() const
  {
    return this->impl_image.device() ;
  }
  
  template<typename Impl>
  nyx::ImageLayout Image<Impl>::layout() const
  {
    return this->impl_image.layout() ;
  }

  template<typename Impl>
  bool Image<Impl>::initialized() const
  {
    return this->impl_image.initialized() ;
  }
  
  template<typename Impl>
  bool Image<Impl>::initialize( nyx::ImageFormat format, unsigned gpu, unsigned width, unsigned height, unsigned layers )
  {
    return this->impl_image.initialize( gpu, format, width, height, layers ) ;
  }

  template<typename Impl>
  bool Image<Impl>::initialize( nyx::ImageFormat format, const Memory<Impl>& prealloc, unsigned width, unsigned height, unsigned layers )
  {
    return this->impl_image.initialize( prealloc, format, width, height, layers ) ;
  }

  template<typename Impl>
  unsigned Image<Impl>::width() const
  {
    return this->impl_image.width() ;
  }

  template<typename Impl>
  unsigned Image<Impl>::height() const
  {
    return this->impl_image.height() ;
  }

  template<typename Impl>
  unsigned Image<Impl>::layers() const
  {
    return this->impl_image.layers() ;
  }

  template<typename Impl>
  void Image<Impl>::setMipLevels( unsigned num_levels )
  {
    this->impl_image.setMipLevels( num_levels ) ;
  }

  template<typename Impl>
  unsigned Image<Impl>::size() const
  {
    return this->impl_image.size() ;
  }
  
  template<typename Impl>
  bool Image<Impl>::resize( unsigned width, unsigned height )
  {
    return this->impl_image.resize( width, height ) ;
  }
  
  template<typename Impl>
  unsigned Image<Impl>::byteSize() const
  {
    return this->impl_image.byteSize() ;
  }
  
  template<typename Impl>
  void Image<Impl>::reset()
  {
    this->impl_image.reset() ;
  }

  template<typename Impl>
  const typename Impl::Texture& Image<Impl>::image() const
  {
    return this->impl_image ;
  }

  template<typename Impl>
  typename Impl::Texture& Image<Impl>::image()
  {
    return this->impl_image ;
  }
}
