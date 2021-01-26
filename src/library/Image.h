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

#ifndef NYX_IMAGE_H
#define NYX_IMAGE_H

namespace nyx
{
  template<typename IMPL>
  class Memory ;

  enum class ImageFormat
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
  };
  
  enum class ImageLayout
  {
    Undefined,       ///< Undefined.
    General,         ///< General purpose.
    ColorAttachment, ///< Color Attachment.
    ShaderRead,      ///< Only valid for shader read.
    TransferSrc,     ///< Only valid for copies from.
    TransferDst,     ///< Only valid for copies to.
    PresentSrc,      ///< Only valid for presenting.
    DepthRead,       ///< Only valid for being read from as depth.
  };
      
  /** Generic memory object.
   */
  template<typename IMPL, ImageFormat FORMAT = ImageFormat::RGBA8>
  class Image
  {
    public:
      
      /** Default constructor.
       */
      Image() = default ;
      
      /** Copy constructor. Copies the input image.
       */
      Image( const Image<IMPL, FORMAT>& image ) ;
      
      /** Default deconstructor.
       */
      ~Image() = default ;
      
      /** Assignment operator. Assigns this image to the input.
       * @param image The image to assign this object to.
       * @return Reference to this image after the assignment.
       */
      Image& operator=( const Image<IMPL, FORMAT>& image ) ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename IMPL::Texture&() const ;
      

      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename IMPL::Texture() const ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename IMPL::Texture&() ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename IMPL::Texture() ;

      /** Method to perform a deep copy on the input image.
       * @param src The image to copy from.
       * @param cmd Reference to a valid command record to record the copy operation to.
       */
      void copy( const Image& src, const typename IMPL::CommandRecord& cmd ) ;

      /** Method to perform a deep copy on the input image.
       * @param src The image to copy from.
       * @param cmd Reference to a valid command record to record the copy operation to.
       */
      void copy( const typename IMPL::Buffer& src, const typename IMPL::CommandRecord& cmd ) ;

      /** Method to retrieve the device associated with this image.
       * @return the GPU Device associated with this image.
       */
      const typename IMPL::Device& device() const ;
      
      /** Method to initialize this image with the input parameters.
       * @param gpu The Implementation-specific GPU to use for this image.
       * @param width The width of the image in pixels.
       * @param height The height of the image in pixels.
       * @param layers The number of layers in the image.
       * @return Whether or not this image was able to successfully initialize.
       */
      bool initialize( const typename IMPL::Device& gpu, unsigned width, unsigned height, unsigned layers = 1 ) ;
      
      /** Method to initialize this image with preallocated memory.
       * @param prealloc The preallocated memory to use for this image's memory.
       * @param width The width of this image in pixels.
       * @param height The height of this image in pixels.
       * @param layers The number of layers in this image.
       * @return Whether or not this image was able to successfully initialize.
       */
      bool initialize( const Memory<IMPL>& prealloc, unsigned width, unsigned height, unsigned layers = 1 ) ;
      
      /** Method to retrieve the width of this image.
       * @return The width of this image in pixels.
       */
      unsigned width() const ;
      
      /** Method to retrieve the height of this image.
       * @return The height of this image in pixels.
       */
      unsigned height() const ;
      
      /** Method to retrieve the number of layers there are in this image.
       * @return The number of layers there are of this image.
       */
      unsigned layers() const ;
      
      /** Method to transition this image to a different layout.
       * @param layout The implementaion-specific layout to transition this image to.
       * @param record The implementation-specific Command Record to use for this operation.
       */
      void transition( const typename nyx::ImageLayout& layout, typename IMPL::CommandRecord& record ) ;
      
      /** Method to set the mip-levels of this image.
       * @param num_levels The number of mip levels to use for this image.
       */
      void setMipLevels( unsigned num_levels ) ;
      
      /** Method to retrieve the size of this image.
       * @return The size of this image in pixels.
       */
      unsigned size() const ;
      
      /** Method to retrieve the byte size of this image.
       * @return The size of this byte image in pixels.
       */
      unsigned byteSize() const ;

      /** Method to reset this image and deallocate any allocated data.
       */
      void reset() ;
      
      /** Method to return the a const-reference to the implementation-specific image handle this image contains.
       * @return Const-Reference to the implementation-specific image handle this image contains.
       */
      const typename IMPL::Texture& image() const ;
      
      /** Method to return the a reference to the implementation-specific image handle this image contains.
       * @return Reference to the implementation-specific image handle this image contains.
       */
      typename IMPL::Texture& image() ;
      
    private:
      /** The implementation specific texture handle.
       */
      typename IMPL::Texture impl_image ;
  };

  template<typename IMPL, ImageFormat FORMAT>
  Image<IMPL, FORMAT>::Image( const Image<IMPL, FORMAT>& image )
  {
    *this = image ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  Image<IMPL, FORMAT>& Image<IMPL, FORMAT>::operator=( const Image<IMPL, FORMAT>& image )
  {
    this->impl_image = image.impl_image ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  Image<IMPL, FORMAT>::operator const typename IMPL::Texture&() const
  {
    return this->impl_image ;
  }
  
  template<typename IMPL, ImageFormat FORMAT>
  Image<IMPL, FORMAT>::operator typename IMPL::Texture() const
  {
    return this->impl_image ;
  }
  
  template<typename IMPL, ImageFormat FORMAT>
  Image<IMPL, FORMAT>::operator const typename IMPL::Texture&()
  {
    return this->impl_image ;
  }
  
  template<typename IMPL, ImageFormat FORMAT>
  Image<IMPL, FORMAT>::operator typename IMPL::Texture()
  {
    return this->impl_image ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  void Image<IMPL, FORMAT>::copy( const Image& src, const typename IMPL::CommandRecord& cmd ) 
  {
    this->impl_image.copy( src, cmd ) ;
  }
  
  template<typename IMPL, ImageFormat FORMAT>
  void Image<IMPL, FORMAT>::copy( const typename IMPL::Buffer& src, const typename IMPL::CommandRecord& cmd )
  {
    this->impl_image.copy( src, cmd ) ;
  }
  
  template<typename IMPL, ImageFormat FORMAT>
  const typename IMPL::Device& Image<IMPL, FORMAT>::device() const
  {
    return this->impl_image.device() ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  bool Image<IMPL, FORMAT>::initialize( const typename IMPL::Device& gpu, unsigned width, unsigned height, unsigned layers )
  {
    return this->impl_image.initialize( gpu, FORMAT, width, height, layers ) ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  bool Image<IMPL, FORMAT>::initialize( const Memory<IMPL>& prealloc, unsigned width, unsigned height, unsigned layers )
  {
    return this->impl_image.initialize( prealloc, FORMAT, width, height, layers ) ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  unsigned Image<IMPL, FORMAT>::width() const
  {
    return this->impl_image.width() ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  unsigned Image<IMPL, FORMAT>::height() const
  {
    return this->impl_image.height() ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  unsigned Image<IMPL, FORMAT>::layers() const
  {
    return this->impl_image.layers() ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  void Image<IMPL, FORMAT>::transition( const typename nyx::ImageLayout& layout, typename IMPL::CommandRecord& record )
  {
    this->impl_image.transition( layout, record ) ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  void Image<IMPL, FORMAT>::setMipLevels( unsigned num_levels )
  {
    this->impl_image.setMipLevels( num_levels ) ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  unsigned Image<IMPL, FORMAT>::size() const
  {
    return this->impl_image.size() ;
  }
  
  template<typename IMPL, ImageFormat FORMAT>
  unsigned Image<IMPL, FORMAT>::byteSize() const
  {
    return this->impl_image.byteSize() ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  void Image<IMPL, FORMAT>::reset()
  {
    this->impl_image.reset() ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  const typename IMPL::Texture& Image<IMPL, FORMAT>::image() const
  {
    return this->impl_image ;
  }

  template<typename IMPL, ImageFormat FORMAT>
  typename IMPL::Texture& Image<IMPL, FORMAT>::image()
  {
    return this->impl_image ;
  }
  
  template<typename IMPL>
  using RGBAImage = nyx::Image<IMPL, ImageFormat::RGBA8> ;
  
  template<typename IMPL>
  using RGBImage = nyx::Image<IMPL, ImageFormat::RGB8> ;
  
  template<typename IMPL>
  using RGBA32FImage = nyx::Image<IMPL, ImageFormat::RGBA32F> ;
}

#endif /* IMAGE_H */

