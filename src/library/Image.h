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

#ifndef KGL_IMAGE_H
#define KGL_IMAGE_H

namespace kgl
{
  template<typename IMPL>
  class Memory ;

  /** Generic memory object.
   */
  template<typename IMPL, typename TYPE>
  class Image
  {
    public:
      
      /** Default constructor.
       */
      Image() = default ;
      
      /** Copy constructor. Copies the input image.
       */
      Image( const Image<IMPL, TYPE>& image ) ;
      
      /** Default deconstructor.
       */
      ~Image() = default ;
      
      /** Assignment operator. Assigns this image to the input.
       * @param image The image to assign this object to.
       * @return Reference to this image after the assignment.
       */
      Image& operator=( const Image<IMPL, TYPE>& image ) ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename IMPL::Image&() const ;
      

      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename IMPL::Image() const ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename IMPL::Image&() ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename IMPL::Image() ;

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
      void transition( const typename IMPL::ImageLayout& layout, typename IMPL::CommandRecord& record ) ;
      
      /** Method to set the mip-levels of this image.
       * @param num_levels The number of mip levels to use for this image.
       */
      void setMipLevels( unsigned num_levels ) ;
      
      /** Method to set the format of this image.
       * @param format The implementation-specific image format to use for this image.
       */
      void setFormat( const typename IMPL::ImageFormat& format ) ;
      
      /** Method to set the layout of this image.
       * @param layout The implementation-specific layout to use for this iamge.
       */
      void setLayout( const typename IMPL::ImageLayout& layout ) ;
      
      /** Method to reset this image and deallocate any allocated data.
       */
      void reset() ;
      
      /** Method to return the a const-reference to the implementation-specific image handle this image contains.
       * @return Const-Reference to the implementation-specific image handle this image contains.
       */
      const typename IMPL::Image& image() const ;
      
      /** Method to return the a reference to the implementation-specific image handle this image contains.
       * @return Reference to the implementation-specific image handle this image contains.
       */
      typename IMPL::Image& image() ;
      
    private:
      /** The implementation specific image handle.
       */
      typename IMPL::Image impl_image ;
  };

  template<typename IMPL, typename TYPE>
  Image<IMPL, TYPE>::Image( const Image<IMPL, TYPE>& image )
  {
    *this = image ;
  }

  template<typename IMPL, typename TYPE>
  Image<IMPL, TYPE>& Image<IMPL, TYPE>::operator=( const Image<IMPL, TYPE>& image )
  {
    this->impl_image = image.impl_image ;
  }

  template<typename IMPL, typename TYPE>
  Image<IMPL, TYPE>::operator const typename IMPL::Image&() const
  {
    return this->impl_image ;
  }
  
  template<typename IMPL, typename TYPE>
  Image<IMPL, TYPE>::operator typename IMPL::Image() const
  {
    return this->impl_image ;
  }
  
  template<typename IMPL, typename TYPE>
  Image<IMPL, TYPE>::operator const typename IMPL::Image&()
  {
    return this->impl_image ;
  }
  
  template<typename IMPL, typename TYPE>
  Image<IMPL, TYPE>::operator typename IMPL::Image()
  {
    return this->impl_image ;
  }

  template<typename IMPL, typename TYPE>
  const typename IMPL::Device& Image<IMPL, TYPE>::device() const
  {
    return this->impl_image.device() ;
  }

  template<typename IMPL, typename TYPE>
  bool Image<IMPL, TYPE>::initialize( const typename IMPL::Device& gpu, unsigned width, unsigned height, unsigned layers )
  {
    return this->impl_image.initialize( gpu, width, height, layers ) ;
  }

  template<typename IMPL, typename TYPE>
  bool Image<IMPL, TYPE>::initialize( const Memory<IMPL>& prealloc, unsigned width, unsigned height, unsigned layers )
  {
    return this->impl_image.initialize( prealloc, width, height, layers ) ;
  }

  template<typename IMPL, typename TYPE>
  unsigned Image<IMPL, TYPE>::width() const
  {
    return this->impl_image.width() ;
  }

  template<typename IMPL, typename TYPE>
  unsigned Image<IMPL, TYPE>::height() const
  {
    return this->impl_image.height() ;
  }

  template<typename IMPL, typename TYPE>
  unsigned Image<IMPL, TYPE>::layers() const
  {
    return this->impl_image.layers() ;
  }

  template<typename IMPL, typename TYPE>
  void Image<IMPL, TYPE>::transition( const typename IMPL::ImageLayout& layout, typename IMPL::CommandRecord& record )
  {
    this->impl_image.transition( layout, record ) ;
  }

  template<typename IMPL, typename TYPE>
  void Image<IMPL, TYPE>::setMipLevels( unsigned num_levels )
  {
    this->impl_image.setMipLevels( num_levels ) ;
  }

  template<typename IMPL, typename TYPE>
  void Image<IMPL, TYPE>::setFormat( const typename IMPL::ImageFormat& format )
  {
    this->impl_image.setFormat( format ) ;
  }

  template<typename IMPL, typename TYPE>
  void Image<IMPL, TYPE>::setLayout( const typename IMPL::ImageLayout& layout )
  {
    this->impl_image.setLayout( layout ) ;
  }

  template<typename IMPL, typename TYPE>
  void Image<IMPL, TYPE>::reset()
  {
    this->impl_image.reset() ;
  }

  template<typename IMPL, typename TYPE>
  const typename IMPL::Image& Image<IMPL, TYPE>::image() const
  {
    return this->impl_image ;
  }

  template<typename IMPL, typename TYPE>
  typename IMPL::Image& Image<IMPL, TYPE>::image()
  {
    return this->impl_image ;
  }
 
  template<typename IMPL>
  using FloatImage = Image<IMPL, float> ;
  
  template<typename IMPL>
  using CharImage = Image<IMPL, unsigned char> ;
}

#endif /* IMAGE_H */

