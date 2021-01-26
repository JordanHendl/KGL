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
 * File:   Swapchain.cpp
 * Author: Jordan Hendl
 * 
 * Created on January 14, 2021, 2:21 AM
 */

#include "Swapchain.h"
#include "Synchronization.h"
#include "Image.h"
#include "Device.h"
#include "Queue.h"
#include "Vulkan.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <queue>
#include <limits.h>

namespace nyx
{
  namespace vkg
  {
    struct SwapchainData
    {
      using Formats = std::vector<vk::SurfaceFormatKHR>      ; ///< TODO
      using Modes   = std::vector<vk::PresentModeKHR>        ; ///< TODO
      using Images  = std::vector<nyx::vkg::Image>           ; ///< TODO
      using Fences  = std::vector<vk::Fence>                 ; ///< TODO
      using Syncs   = std::vector<nyx::vkg::Synchronization> ; ///< TODO
      
      Syncs                      syncs          ; ///< The synchronizations of this object.
      Fences                     fences         ; ///< The fences used for managing frames.
      Formats                    formats        ; ///< TODO
      Modes                      modes          ; ///< TODO
      Images                     images         ; ///< TODO
      nyx::vkg::Queue            present_queue  ; ///< TODO
      vk::SwapchainKHR           swapchain      ; ///< TODO
      vk::SurfaceCapabilitiesKHR capabilities   ; ///< TODO
      vk::SurfaceFormatKHR       surface_format ; ///< TODO
      vk::SurfaceKHR             surface        ; ///< TODO
      vk::Extent2D               extent         ; ///< TODO
      std::queue<unsigned>       acquired       ; ///< The images acquired from this swapchain.
      unsigned                   current_frame  ; ///< The frame counter used to monitor swapchain presenting.
      
      /** Default constructor.
       */
      SwapchainData() ;

      /** Method to return the mode for this swapchain, if it's available.
       * @param value The requested mode.
       * @return The mode selected by what was available.
       */
      vk::PresentModeKHR mode( vk::PresentModeKHR value ) ;

      /** Helper method for finding a vulkan format from all formats supported.
       */
      void format( vk::Format value, vk::ColorSpaceKHR color ) ;

      /** Helper method for generating the swapchain.
       */
      void makeSwapchain() ;

      /** Helper method to find the images of this swapchain.
       */
      void generateImages() ;

      /** Helper method to find the swapchain properties from the device.
       */
      void findProperties() ;
      
      /** Method to choose an extent from the surface capabilities.
       */
      void chooseExtent() ;
    };
    
    SwapchainData::SwapchainData()
    {
      this->current_frame = 0 ;
    }

    void SwapchainData::makeSwapchain()
    {
      vk::SwapchainCreateInfoKHR info ;
      
      this->format( vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear ) ; // TODO make config

      info.setSurface         ( this->surface                                ) ;
      info.setMinImageCount   ( this->capabilities.minImageCount + 1         ) ;
      info.setImageFormat     ( this->surface_format.format                  ) ;
      info.setImageColorSpace ( this->surface_format.colorSpace              ) ;
      info.setImageExtent     ( this->extent                                 ) ;
      info.setImageArrayLayers( 1                                            ) ; // TODO make config.
      info.setImageUsage      ( vk::ImageUsageFlagBits::eColorAttachment     ) ; // TODO make config.
      info.setPreTransform    ( this->capabilities.currentTransform          ) ;
      info.setCompositeAlpha  ( vk::CompositeAlphaFlagBitsKHR::eOpaque       ) ; // TODO make config.
      info.setPresentMode     ( this->mode( vk::PresentModeKHR::eImmediate ) ) ; // TODO make config.
      
      info.setImageSharingMode     ( vk::SharingMode::eExclusive ) ;
      info.setQueueFamilyIndexCount( 0                           ) ;
      info.setQueueFamilyIndices   ( nullptr                     ) ;
      
      this->present_queue.device().device().createSwapchainKHR( &info, nullptr, &this->swapchain ) ;
    }

    void SwapchainData::generateImages()
    {
      std::vector<vk::Image> imgs = this->present_queue.device().device().getSwapchainImagesKHR( this->swapchain ) ;
      
      this->images.resize( imgs.size() ) ;
      for( unsigned index = 0; index < this->images.size(); index++ )
      {
        this->images[ index ].setFormat( this->surface_format.format ) ;
        this->images[ index ].initialize( this->present_queue.device(), nyx::vkg::Vulkan::convert( this->surface_format.format ), this->extent.width, this->extent.height, imgs[ index ] ) ;
      }
    }
    
    void SwapchainData::format( vk::Format value, vk::ColorSpaceKHR color )
    {
      for( const auto& format : this->formats )
      {
        if( format.format == value && format.colorSpace == color )
        {
          this->surface_format = format ;
        }
      }
    }
    
    void SwapchainData::findProperties()
    {
      const vk::PhysicalDevice device = this->present_queue.device().physicalDevice() ;

      this->formats      = device.getSurfaceFormatsKHR     ( this->surface ) ;
      this->capabilities = device.getSurfaceCapabilitiesKHR( this->surface ) ;
      this->modes        = device.getSurfacePresentModesKHR( this->surface ) ;
    }
    
    void SwapchainData::chooseExtent()
    {
      if( this->capabilities.currentExtent.width != UINT32_MAX )
      {
        this->extent = this->capabilities.currentExtent ;
      }
      
      this->extent.width  = std::max( this->capabilities.minImageExtent.width , std::min( this->capabilities.maxImageExtent.width , this->extent.width  ) ) ;
      this->extent.height = std::max( this->capabilities.minImageExtent.height, std::min( this->capabilities.maxImageExtent.height, this->extent.height ) ) ;
    }
    
    vk::PresentModeKHR SwapchainData::mode( vk::PresentModeKHR value ) 
    {
      for( auto& mode : this->modes )
      {
        if( mode == value ) return mode ;
      }
      
      return vk::PresentModeKHR::eFifo ;
    }

    Swapchain::Swapchain()
    {
      this->swap_data = new SwapchainData() ;
    }

    Swapchain::Swapchain( const Swapchain& swapchain )
    {
      this->swap_data = new SwapchainData() ;
      
      *this = swapchain ;
    }

    Swapchain::~Swapchain()
    {
      delete this->swap_data ;
    }

    Swapchain& Swapchain::operator=( const Swapchain& swapchain )
    {
      *this->swap_data = *swapchain.swap_data ;
      
      return *this ;
    }

    Swapchain::operator const vk::SwapchainKHR&()
    {
      return data().swapchain ;
    }

    Swapchain::operator vk::SwapchainKHR&()
    {
      return data().swapchain ;
    }

    void Swapchain::initialize(  const nyx::vkg::Queue& present_queue, const vk::SurfaceKHR& surface )
    {
      data().present_queue = present_queue ;
      data().surface       = surface       ;
      
      
      data().fences.clear() ;
      data().findProperties() ;
      data().chooseExtent  () ;
      data().makeSwapchain () ;
      data().generateImages() ;
      
      data().syncs .resize( this->count() ) ;
      data().fences.resize( this->count() ) ;
      for( auto& sync : data().syncs ) sync.initialize( present_queue.device(), 1 ) ;
    }

    const nyx::vkg::Synchronization& Swapchain::acquire()
    {
      const auto device    = data().present_queue.device().device() ;
      const unsigned index = data().current_frame                   ;
      if( data().fences[ index ] ) 
      {
        device.waitForFences( 1, &data().fences[ index ], VK_TRUE, UINT64_MAX ) ;
        device.resetFences( 1, &data().fences[ index ] ) ;
      }
      
      auto result = device.acquireNextImageKHR( data().swapchain, UINT64_MAX, data().syncs[ index ].signal(), data().syncs[ index ].signalFence() ) ;
      
      if( result.result == vk::Result::eErrorOutOfDateKHR || result.result == vk::Result::eSuboptimalKHR )
      {
        device.waitIdle() ;
        this->reset() ;
        this->initialize( data().present_queue, data().surface ) ;
      }
      else
      {
        data().fences[ index ] = data().syncs[ index ].signalFence() ;
        data().acquired.push( static_cast<unsigned>( result.value ) ) ;
        
        data().current_frame = ( index + 1 ) % data().syncs.size() ;
      }
      
      return data().syncs[ index ] ;
    }

    void Swapchain::submit( const nyx::vkg::Synchronization& sync )
    {
      const unsigned index = data().acquired.front() ;
      
      data().syncs[ index ].waitOn( sync ) ;
      data().present_queue.submit( *this, index, data().syncs[ index ] ) ;
      data().acquired.pop() ;

      data().syncs[ index ].clear() ;
    }
    
    const nyx::vkg::Image* Swapchain::images() const
    {
      return data().images.data() ;
    }
    
    const nyx::vkg::Device& Swapchain::device() const
    {
      return data().present_queue.device() ;
    }
    
    const vk::Format& Swapchain::format() const
    {
      return data().surface_format.format ;
    }

    bool Swapchain::initialized() const
    {
      return data().swapchain ;
    }

    unsigned Swapchain::width() const
    {
      return data().extent.width ;
    }

    unsigned Swapchain::height() const
    {
      return data().extent.height ;
    }

    unsigned Swapchain::count() const
    {
      return data().images.size() ;
    }

    unsigned Swapchain::current() const
    {
      return data().acquired.back() ;
    }
    
    const nyx::vkg::Image& Swapchain::image( unsigned idx ) const
    {
      static const nyx::vkg::Image dummy ;
      
      if( idx < data().images.size() ) return data().images[ idx ] ;
      
      return dummy ;
    }
    
    const vk::SwapchainKHR& Swapchain::swapchain() const
    {
      return data().swapchain ;
    }

    void Swapchain::reset()
    {
      if( data().swapchain )
      {
        
        for( auto& image : data().images )
        {
          image.reset() ;
        }
        
        data().images.clear() ;
        data().present_queue.device().device().destroy( data().swapchain ) ;
      }
    }

    SwapchainData& Swapchain::data()
    {
      return *this->swap_data ;
    }

    const SwapchainData& Swapchain::data() const
    {
      return *this->swap_data ;
    }
  }
}
