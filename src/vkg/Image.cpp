#include "Image.h"
#include "Device.h"
#include "Vulkan.h"
#include <library/Memory.h>
#include <vulkan/vulkan.hpp>

namespace kgl
{
  namespace vkg
  {
    struct ImageData
    {
      vkg::Device               device       ; ///< TODO
      kgl::vkg::VkMemory        memory       ; ///< TODO
      vk::MemoryRequirements    requirements ; ///< TODO
      bool                      preallocated ; ///< TODO
      unsigned                  width        ; ///< TODO
      unsigned                  height       ; ///< TODO
      unsigned                  layers       ; ///< TODO
      unsigned                  num_mip      ; ///< TODO
      ::vk::Image               image        ; ///< TODO
      ::vk::ImageView           view         ; ///< TODO
      ::vk::Sampler             sampler      ; ///< TODO
      ::vk::ImageLayout         layout       ; ///< TODO
      ::vk::ImageLayout         old_layout   ; ///< TODO
      ::vk::Format              format       ; ///< TODO
      ::vk::ImageType           type         ; ///< TODO
      ::vk::SampleCountFlagBits num_samples  ; ///< TODO
      ::vk::ImageUsageFlags     usage_flags  ; ///< TODO

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
                                 ::vk::ImageUsageFlagBits::eTransferDst ;
      
      this->width        = 0                             ;
      this->height       = 0                             ;
      this->layers       = 0                             ;
      this->num_mip      = 1                             ;
      this->format       = ::vk::Format::eR8G8B8A8Srgb   ;
      this->layout       = ::vk::ImageLayout::eUndefined ;
      this->old_layout   = ::vk::ImageLayout::eUndefined ;
      this->type         = ::vk::ImageType::e2D          ;
      this->usage_flags  = default_usage                 ;
      this->num_samples  = ::vk::SampleCountFlagBits::e1 ;
      this->preallocated = false                         ;
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
      range.setLayerCount    ( this->layers                    ) ;
      range.setLevelCount    ( this->num_mip                   ) ;
      
      info.setImage           ( this->image            ) ;
      info.setViewType        ( vk::ImageViewType::e2D ) ; // @TODO Make configurable.
      info.setFormat          ( this->format           ) ;
      info.setSubresourceRange( range                  ) ;
      
      return this->device.device().createImageView( info, nullptr ) ;
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
      
      return this->device.device().createSampler( info, nullptr ) ;
    }

    vk::Image ImageData::createImage()
    {
      ::vk::ImageCreateInfo info   ;
      ::vk::Image           image  ;
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
             
      image = this->device.device().createImage( info, nullptr ) ;

      return image ;
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

//    void Image::copy( const Image& src, vk::CommandBuffer& buffer )
//    {
//    
//    }
//
//    void Image::copy( const unsigned char* src, unsigned element_size, vk::CommandBuffer& cmd_buff )
//    {
//      
//    }

    bool Image::initialize( const vkg::Device& device, unsigned width, unsigned height, unsigned num_layers )
    {
      data().device = device     ;
      data().width  = width      ;
      data().height = height     ;
      data().layers = num_layers ;
      
      data().image   = data().createImage()   ;

      data().requirements = device.device().getImageMemoryRequirements( data().image ) ;
      
      if( !data().preallocated )
      {
        data().memory.initialize( device, data().requirements.size, false ) ;
      }
      
      if( data().requirements.size <= data().memory.size() - data().memory.offset() )
      {
        device.device().bindImageMemory( data().image, data().memory.memory(), data().memory.offset() ) ;
        
        data().view    = data().createView ()   ;
        data().sampler = data().createSampler() ;
        return true ;
      }
      
      return false ;
    }

    bool Image::initialize( const kgl::vkg::Device& gpu, unsigned width, unsigned height, vk::Image prealloc, unsigned num_layers )
    {
      data().device = gpu        ;
      data().width  = width      ;
      data().height = height     ;
      data().layers = num_layers ;

      data().image   = prealloc               ;
      data().view    = data().createView ()   ;
      data().sampler = data().createSampler() ;

      return true ;
    }
    
    bool Image::initialize( kgl::Memory<kgl::vkg::Vulkan>& prealloc, unsigned width, unsigned height, unsigned num_layers )
    {
      data().memory       = prealloc ;
      data().preallocated = true     ;
      
      return this->initialize( prealloc.device(), width, height, num_layers ) ;
    }

    void Image::setUsage( const ::vk::ImageUsageFlagBits& usage )
    {
      data().usage_flags = usage ;
    }
    
    void Image::setUsage( const Image::UsageFlags& usage )
    {
      data().usage_flags = usage ;
    }

    void Image::setType( const ::vk::ImageType& type )
    {
      data().type = type ;
    }

    void Image::setNumSamples( const ::vk::SampleCountFlagBits& num_samples )
    {
      data().num_samples = num_samples ;
    }
    
    void Image::setMipLevels( unsigned mip_levels )
    {
      data().num_mip = mip_levels ;
    }

    void Image::setFormat( const vk::Format& format )
    {
      data().format = format ;
    }

    void Image::setLayout( const vk::ImageLayout& layout )
    {
      data().layout = layout ;
    }

    void Image::transition( const vk::ImageLayout& layout, vk::CommandBuffer& cmd_buff )
    {
      ::vk::ImageMemoryBarrier barrier   ;
      ::vk::ImageSubresource   range     ;
      ::vk::PipelineStageFlags src       ;
      ::vk::PipelineStageFlags dst       ;
      ::vk::DependencyFlags    dep_flags ;
      
      cmd_buff.pipelineBarrier( src, dst, dep_flags, 0, nullptr, 0, nullptr, 1, &barrier ) ;
      
      data().old_layout = data().layout ;
      data().layout     = layout        ;
    }

    void Image::revertLayout( vk::CommandBuffer& cmd_buff )
    {
      this->transition( data().old_layout, cmd_buff ) ;
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
      data().device.device().destroy( data().image ) ;
      
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