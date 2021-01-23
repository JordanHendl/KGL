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

#ifndef KGL_VKG_IMAGE_H
#define KGL_VKG_IMAGE_H
typedef unsigned VkFlags ;

namespace vk
{
   template <typename BitType>
   class Flags ;

       class CommandBuffer                 ;
       class Sampler                       ;
       class ImageView                     ;
       class Image                         ;
  enum class ImageLayout                   ;
  enum class ImageType                     ;
  enum class Format                        ;
  enum class ImageUsageFlagBits  : VkFlags ;
  enum class SampleCountFlagBits : VkFlags ;
}

namespace kgl
{
  template<class T>
  class Memory ;
  
  namespace vkg
  {
    class Vulkan ;
    class Device ;
    class Buffer ;

    /** Abstraction of a Vulkan Image.
     */
    class Image 
    {
      public:
        
        /** Alias for Vulkan Usage flags.
         */
        using UsageFlags = ::vk::Flags<::vk::ImageUsageFlagBits> ;
        
        /** Alias for Vulkan Sample flags.
         */
        using SampleFlags = ::vk::Flags<::vk::SampleCountFlagBits> ;

        /** Constructor
         */
        Image() ;
        
        /** Copy constructor. Does not do a deep copy, performs a surface-level copy only. ( Copies pointers to GPU data, does not allocate and copy ).
         * @note For a deep copy, see @copy.
         * @param orig The image to grab the image data from.
         */
        Image( const Image& orig ) ;
        
        /** Default deconstructor.
         */
        ~Image() ;
        
        /** Equals operator. Does not do a deep copy,  performs a surface-level copy only. ( Copies pointers to GPU data, does not allocate and copy ).
         * @note For a deep copy, see @copy.
         * @param src The image to grab image data from.
         * @return A reference to this image after the copy.
         */
        Image& operator=( const Image& src ) ;
        
        /** Method to perform a deep copy on the input image.
         * @param src The image to copy from.
         * @param buffer Reference to a valid vulkan command buffer to record the copy operation to.
         */
        void copy( const Image& src, vk::CommandBuffer& buffer ) ;
        
        /** Method to perform a deep copy on the input image.
         * @param src The image to copy from.
         * @param buffer Reference to a valid vulkan command buffer to record the copy operation to.
         */
        void copy( const kgl::vkg::Buffer& src, vk::CommandBuffer& buffer ) ;

        /** Method to initialize this object with the input parameters.
         * @note Uses any set values from other setters in initialization.
         * @param width The width of the image in pixels.
         * @param height The height of the image in pixels.
         * @param num_layers The number of layers of the image.
         */
        bool initialize( const kgl::vkg::Device& gpu, unsigned width, unsigned height, unsigned num_layers = 1 ) ;
        
        /** Method to initialize this object with the input parameters.
         * @note Uses any set values from other setters in initialization.
         * @param width The width of the image in pixels.
         * @param height The height of the image in pixels.
         * @param num_layers The number of layers of the image.
         */
        bool initialize( const kgl::vkg::Device& gpu, unsigned width, unsigned height, vk::Image prealloc, unsigned num_layers = 1 ) ;

        /** Method to initialize this object with the input parameters.
         * @note Uses any set values from other setters in initialization.
         * @param prealloc The pre-allocated memory to use for this object. This object will not allocate any more data.
         * @param width The width of the image in pixels.
         * @param height The height of the image in pixels.
         * @param num_layers The number of layers of the image.
         */
        bool initialize( kgl::Memory<kgl::vkg::Vulkan>& prealloc, unsigned width, unsigned height, unsigned num_layers = 1 ) ;
        
        /** Method to retrieve the underlying memory of this object.
         * @return The memory representation of this object.
         */
        kgl::Memory<kgl::vkg::Vulkan>& memory() ;
        
        /** Method to retrieve the underlying memory of this object.
         * @return The memory representation of this object.
         */
        const kgl::Memory<kgl::vkg::Vulkan>& memory() const ;
        
        /** Method to set this image's vulkan usage.
         * @param usage The Vulkan usage of the image.
         */
        void setUsage( const ::vk::ImageUsageFlagBits& usage ) ;
        
        /** Method to set this image's vulkan usage.
         * @param usage The Vulkan usage of the image.
         */
        void setUsage( const Image::UsageFlags& usage ) ;
        
        /** Method to set the type of image this is.
         * @param The vulkan type of image.
         */
        void setType( const ::vk::ImageType& type ) ;
        
        /** Method to set the number of samples to use for this image.
         * @param num_samples Numer of mip-levels to use for this image.
         */
        void setNumSamples( const ::vk::SampleCountFlagBits& num_samples ) ;
        
        /** Method to set the number of mip-levels to use for this image.
         * @param mip_levels The number of mip-levels to use for this image.
         */
        void setMipLevels( unsigned mip_levels ) ;
        
        /** Method to set the format of this object. 
         * @param format The Vulkan format to set this image to.
         */
        void setFormat( const vk::Format& format ) ;
        
        /** Method to set the layout of this image.
         * @param layout The Vulkan layour to set this image to.
         */
        void setLayout( const vk::ImageLayout& layout ) ;
        
        /** Method to transition this object's layout to the specified one on the GPU.
         * @param layout The layout to transition this image to.
         * @param cmd_buff The Vulkan command buffer to record the transition operation to.
         */
        void transition( const vk::ImageLayout& layout, vk::CommandBuffer& cmd_buff ) const ;
        
        /** Method to transition the image back to it's last known layout.
         * @param cmd_buff The Vulkan command buffer to record the transition operation to.
         */
        void revertLayout( vk::CommandBuffer& cmd_buff ) const ;
        
        /** Method to retrieve the vulkan image view associated with this image.
         * @return The vulkan image view associated with this image.
         */ 
        const ::vk::ImageView& view() const ;
        
        /** Method to retrieve the vulkan sampler associated with this image.
         * @return The vulkan sampler associated with this image.
         */
        const ::vk::Sampler& sampler() const ;

        /** Method to retrieve this image's width in pixels.
         * @return The image width in pixels.
         */
        unsigned width() const ;
        
        /** Method to retrieve this image's height in pixels.
         * @return The image height in pixels.
         */
        unsigned height() const ;
        
        /** Method to retrieve the number of layers in this image.
         * @return The number of layers in this image.
         */
        unsigned layers() const ;
        
        /** Method to release all allocated data by this image.
         * @note If preallocated, this simply releases the memory control and does not deallocate.
         */
        void reset() ;
        
      private:
        
        /** Forward-declared structure containing this object's internal data.
         */
        struct ImageData *img_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        ImageData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const ImageData& data() const ;
    };
  }
}

#endif

