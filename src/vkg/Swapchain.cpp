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

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VULKAN_HPP_NOEXCEPT
#define VULKAN_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS

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
      vkg::Queue                 queue  ; ///< TODO
      vkg::Device                device         ;
      vk::SwapchainKHR           swapchain      ; ///< TODO
      vk::SurfaceCapabilitiesKHR capabilities   ; ///< TODO
      vk::SurfaceFormatKHR       surface_format ; ///< TODO
      vk::SurfaceKHR             surface        ; ///< TODO
      vk::Extent2D               extent         ; ///< TODO
      std::queue<unsigned>       acquired       ; ///< The images acquired from this swapchain.
      unsigned                   current_frame  ; ///< The frame counter used to monitor swapchain presenting.
      bool                       skip_frame     ;

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
      this->current_frame = 0     ;
      this->skip_frame    = false ;
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
      
      vkg::Vulkan::add( this->device.device().createSwapchainKHR( &info, nullptr, &this->swapchain ) ) ;
    }

    void SwapchainData::generateImages()
    {
      auto result = this->device.device().getSwapchainImagesKHR( this->swapchain ) ;
      
      vkg::Vulkan::add( result.result ) ;
      
      std::vector<vk::Image> imgs = result.value ;
      
      this->images.resize( imgs.size() ) ;
      for( unsigned index = 0; index < this->images.size(); index++ )
      {
        this->images[ index ].setFormat( nyx::vkg::Vulkan::convert( this->surface_format.format ) ) ;
        this->images[ index ].initialize( this->queue.device(), nyx::vkg::Vulkan::convert( this->surface_format.format ), this->extent.width, this->extent.height, imgs[ index ] ) ;
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
      const vk::PhysicalDevice device = this->device.physicalDevice() ;

      auto result1 = device.getSurfaceFormatsKHR     ( this->surface ) ;
      auto result2 = device.getSurfaceCapabilitiesKHR( this->surface ) ;
      auto result3 = device.getSurfacePresentModesKHR( this->surface ) ;
      
      vkg::Vulkan::add( result1.result ) ;
      vkg::Vulkan::add( result2.result ) ;
      vkg::Vulkan::add( result3.result ) ;
      
      this->formats      = result1.value ;
      this->capabilities = result2.value ;
      this->modes        = result3.value ;
    }
    
    void SwapchainData::chooseExtent()
    {
      if( this->capabilities.currentExtent.width != UINT32_MAX )
      {
        this->extent = this->capabilities.currentExtent ;
      }
      else
      {
        this->extent.width  = std::max( this->capabilities.minImageExtent.width , std::min( this->capabilities.maxImageExtent.width , this->extent.width  ) ) ;
        this->extent.height = std::max( this->capabilities.minImageExtent.height, std::min( this->capabilities.maxImageExtent.height, this->extent.height ) ) ;
      }
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

    void Swapchain::initialize( const nyx::vkg::Queue& present_queue, const vk::SurfaceKHR& surface )
    {
      Vulkan::initialize() ;

      data().queue   = present_queue                           ;
      data().surface = surface                                 ;
      data().device  = Vulkan::device( data().queue.device() ) ;
      data().fences.clear() ;
      data().findProperties() ;
      data().chooseExtent  () ;
      data().makeSwapchain () ;
      data().generateImages() ;
      
      data().syncs .resize( this->count() ) ;
      data().fences.resize( this->count() ) ;
      for( auto& sync : data().syncs ) sync.initialize( data().device, 1 ) ;
    }

    unsigned Swapchain::acquire()
    {
      const auto device    = data().device.device() ;
      const unsigned index = data().current_frame   ;
      if( data().fences[ index ] ) 
      {
        vkg::Vulkan::add( device.waitForFences( 1, &data().fences[ index ], VK_TRUE, UINT64_MAX ) ) ;
        vkg::Vulkan::add( device.resetFences( 1, &data().fences[ index ]                        ) ) ;
      }
      
      data().syncs[ index ].resetFence() ;
      auto result = device.acquireNextImageKHR( data().swapchain, UINT64_MAX, data().syncs[ index ].signal(), data().syncs[ index ].signalFence() ) ;
      
      if( result.result == vk::Result::eErrorOutOfDateKHR || result.result == vk::Result::eSuboptimalKHR )
      {
        vkg::Vulkan::add( device.waitIdle() ) ;
        this->initialize( data().queue, data().surface ) ;

        data().skip_frame = true ;
        return Vulkan::Error::RecreateSwapchain ;
      }
      else
      {
        data().fences[ index ] = data().syncs[ index ].signalFence() ;
        data().acquired.push( static_cast<unsigned>( result.value ) ) ;
        
        data().current_frame = ( index + 1 ) % data().syncs.size() ;
      }
      
      data().syncs[ index ].waitOn( data().syncs[ index ] ) ;
      return Vulkan::Error::Success ;
    }

    unsigned Swapchain::submit()
    {
      const unsigned index = data().acquired.front() ;
      
      if( !data().skip_frame )
      {
        if( data().queue.submit( *this, index, data().syncs[ index ] ) == nyx::vkg::Vulkan::Error::RecreateSwapchain )
        {
          Vulkan::deviceSynchronize( data().queue.device() ) ;
          this->initialize( data().queue, data().surface ) ;
          
          data().acquired.pop() ;
          data().syncs[ index ].clear() ;
          
          return Vulkan::Error::RecreateSwapchain ;
        }
      }
      else
      {
        data().skip_frame = true ;
      }

      data().acquired.pop() ;

      data().syncs[ index ].clear() ;
      
      return Vulkan::Error::Success ;
    }

    const nyx::vkg::Image* Swapchain::images() const
    {
      return data().images.data() ;
    }
    
    unsigned Swapchain::device() const
    {
      return data().queue.device() ;
    }
    
    const vk::Format& Swapchain::format() const
    {
      return data().surface_format.format ;
    }

    bool Swapchain::initialized() const
    {
      return data().swapchain && !data().acquired.empty() ;
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
      return data().acquired.front() ;
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
        data().device.device().destroy( data().swapchain ) ;
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
