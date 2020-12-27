#include "Image.h"

namespace kgl
{
  namespace vkg
  {
    struct ImageData
    {
      
    };

    Image::Image()
    {
    
    }

    Image::Image( const Image& orig )
    {
    
    }

    Image::~Image()
    {
    
    }

    Image& Image::operator=( const Image& src )
    {
    
    }

    void Image::copy( const Image& src, vk::CommandBuffer& buffer )
    {
    
    }

    void Image::copy( const unsigned char* src, unsigned element_size, vk::CommandBuffer& cmd_buff )
    {
    
    }

    void Image::initialize( const vk::Device& gpu, unsigned width, unsigned height, unsigned num_layers = 1 )
    {
    
    }

    void Image::initialize( kgl::Memory<kgl::vkg::Vulkan>& prealloc, const vk::Device& gpu, unsigned width, unsigned height, unsigned num_layers = 1 )
    {
    
    }

    void Image::setUsage( unsigned usage )
    {
    
    }

    void Image::setType( unsigned type )
    {
    
    }

    void Image::setNumSamples( unsigned num_samples )
    {
    
    }

    void Image::setMipLevels( unsigned mip_levels )
    {
    
    }

    void Image::setFormat( const vk::Format& format )
    {
    
    }

    void Image::setLayout( const vk::ImageLayout& layout )
    {
    
    }

    void Image::transitionLayout( const vk::ImageLayout& layout, vk::CommandBuffer& cmd_buff )
    {
    
    }

    void Image::revertLayout( vk::CommandBuffer& cmd_buff )
    {
    
    }

    unsigned Image::width() const
    {
    
    }

    unsigned Image::height() const
    {
    
    }

    unsigned Image::numLayers() const
    {
    
    }

    void Image::reset()
    {
    
    }

    ImageData& Image::data()
    {
    
    }

    const ImageData& Image::data() const
    {
    
    }
  }
}