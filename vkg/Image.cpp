#include "Image.h"
#include "Device.h"
#include "../Memory.h"
#include "Vulkan.h"
#include <vulkan/vulkan.hpp>

namespace kgl
{
  namespace vkg
  {
    struct ImageData
    {
      vkg::Device               device       ;
      kgl::VkMemory             memory       ;
      vk::MemoryRequirements    requirements ;
      bool                      preallocated ;
      unsigned                  width        ;
      unsigned                  height       ;
      unsigned                  layers       ;
      unsigned                  num_mip      ;
      ::vk::Image               image        ;
      ::vk::ImageView           view         ;
      ::vk::Sampler             sampler      ;
      ::vk::ImageLayout         layout       ;
      ::vk::ImageLayout         old_layout   ;
      ::vk::Format              format       ;
      ::vk::ImageType           type         ;
      ::vk::SampleCountFlagBits num_samples  ;
      ::vk::ImageUsageFlags     usage_flags  ;

      /** Default constructor.
       */
      ImageData() ;      
      
      /** Method to create a vulkan image.
       * @return A Made vulkan image.
       */
      ::vk::Image createImage() ;
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
    
    
    ::vk::Image ImageData::createImage()
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

    void Image::copy( const Image& src, vk::CommandBuffer& buffer )
    {
    
    }

    void Image::copy( const unsigned char* src, unsigned element_size, vk::CommandBuffer& cmd_buff )
    {
      
    }

    bool Image::initialize( const vkg::Device& device, unsigned width, unsigned height, unsigned num_layers )
    {
      unsigned needed_size ;
      
      data().device = device     ;
      data().width  = width      ;
      data().height = height     ;
      data().layers = num_layers ;
      
      data().image = data().createImage() ;
      
      data().requirements = device.device().getImageMemoryRequirements( data().image ) ;
      
      if( !data().preallocated )
      {
        data().memory.initialize( data().requirements.size, device, false ) ;
      }
      
      if( data().requirements.size <= data().memory.size() - data().memory.offset() )
      {
        device.device().bindImageMemory( data().image, data().memory.memory(), data().memory.offset() ) ;
        
        return true ;
      }
      
      return false ;
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
