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
#include "Image.h"
#include "Device.h"
#include "Queue.h"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace kgl
{
  namespace vkg
  {
    struct SwapchainData
    {
      using Formats = std::vector<vk::SurfaceFormatKHR> ;
      using Modes   = std::vector<vk::PresentModeKHR>   ;
      using Images  = std::vector<kgl::vkg::Image>      ;
      
      Formats                    formats        ;
      Modes                      modes          ;
      Images                     images         ;
      kgl::vkg::Queue            present_queue  ;
      vk::SwapchainKHR           swapchain      ;
      vk::SurfaceCapabilitiesKHR capabilities   ;
      vk::SurfaceFormatKHR       surface_format ;
      vk::SurfaceKHR             surface        ;
      vk::Extent2D               extent         ;
      
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
        this->images[ index ].initialize( this->present_queue.device(), this->extent.width, this->extent.height, imgs[ index ] ) ;
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

    void Swapchain::initialize(  const kgl::vkg::Queue& present_queue, const vk::SurfaceKHR& surface )
    {
      data().present_queue = present_queue ;
      data().surface       = surface       ;

      data().findProperties() ;
      data().chooseExtent  () ;
      data().makeSwapchain () ;
//      data().generateImages() ;
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

    const vk::SwapchainKHR Swapchain::swapchain() const
    {
      return data().swapchain ;
    }

    void Swapchain::reset()
    {
    
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
