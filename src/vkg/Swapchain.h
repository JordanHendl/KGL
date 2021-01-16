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
 * File:   Swapchain.h
 * Author: Jordan Hendl
 *
 * Created on January 14, 2021, 2:21 AM
 */

#ifndef KGL_VKG_SWAPCHAIN_H
#define KGL_VKG_SWAPCHAIN_H

#include "Vulkan.h"

namespace vk
{
  class SurfaceKHR   ;
  class SwapchainKHR ;
  enum class Format  ;

}

namespace kgl
{
  namespace vkg
  {
    class Queue ;
    
    /** Class for managing a Vulkan Swapchain.
     */
    class Swapchain
    {
      public:
        
        /** Default constructor.
         */
        Swapchain() ;
        
        /** Copy constructor.
         * @param swapchain The swapchain object to copy.
         */
        Swapchain( const Swapchain& swapchain ) ;
        
        /** Default deconstructor.
         */
        ~Swapchain() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param swapchain The swapchain object to assign this object to.
         * @return Reference to this object after assignment.
         */
        Swapchain& operator=( const Swapchain& swapchain ) ;
        
        /** Conversion operator to convert this object to a Vulkan Swapchain.
         * @return The Vullkan Swapchain of this object.
         */
        operator const vk::SwapchainKHR&() ;
        
        /** Conversion operator to convert this object to a Vulkan Swapchain.
         * @return The Vullkan Swapchain of this object.
         */
        operator vk::SwapchainKHR&() ;
        
        /** Method to initialize this object and create the Vullkan Swapchain.
         * @param device The device to use for Vulkan calls.
         * @param surface The Vulkan Surface for the window to create a swapchain of.
         */
        void initialize( const kgl::vkg::Queue& present_queue, const vk::SurfaceKHR& surface ) ;
        
        /** Method to check if this object has been initialized or not.
         * @return Whether or not this object is initialized.
         */
        bool initialized() const ;

        /** Method to set the vulkan format to use for the swapchain creation.
         * @param format The format of the swapchain's images.
         */
        void setFormat( const vk::Format& format ) ;
        
        /** Method to retrieve the width of the swapchain's framebuffers.
         * @return The width in pixels of this swapchain.
         */
        unsigned width() const ;
        
        /** Method to retrieve the height of the swapchain's framebuffers.
         * @return The height in pixels of this swapchain.
         */
        unsigned height() const ;
        
        /** Method to retrieve the number of framebuffers in this swapchain.
         * @return The number of framebuffers in this swapchain.
         */
        unsigned count() const ;
        
        /** Method to retrieve the format of this swapchain's framebuffers.
         * @return The Vulkan format of this swapchain's framebuffers.
         */
        const vk::Format& format() const ;
        
        /** Method to retrieve the internal Vulkan Swapchain of this object.
         * @return The internal Vulkan Swapchain of this object.
         */
        const vk::SwapchainKHR swapchain() const ;
        
        /** Method to reset this object and deallocate all data.
         */
        void reset() ;

      private:
        
        /** Forward declared structure to contain this object's internal data.
         */
        struct SwapchainData *swap_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        SwapchainData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const SwapchainData& data() const ;
    };
  }
}

#endif /* SWAPCHAIN_H */

