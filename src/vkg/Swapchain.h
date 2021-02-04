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

#ifndef NYX_VKG_SWAPCHAIN_H
#define NYX_VKG_SWAPCHAIN_H

#include "Image.h"

namespace vk
{
  class SurfaceKHR   ;
  class SwapchainKHR ;
  enum class Format  ;
}

namespace nyx
{
  namespace vkg
  {
    class Device          ;
    class Queue           ;
    class Synchronization ;
    class Image           ;
    class RenderPass      ;

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
         * @param present_queue The present queue to use for this object's initialization.
         * @param surface The Vulkan Surface for the window to create a swapchain of.
         */
        void initialize( const nyx::vkg::Queue& present_queue, const vk::SurfaceKHR& surface ) ;
        
        /** Method to check if this object has been initialized or not.
         * @return Whether or not this object is initialized.
         */
        bool initialized() const ;
        
        /** Method to retrieve the library device used for this object.
         * @return The device used to initialize this object.
         */
        unsigned device() const ;

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
        const vk::SwapchainKHR& swapchain() const ;
        
        /** Method to retrieve the image associated with this swapchain.
         * @param idx The index of image to grab. See @count for the number of images.
         * @return const reference to the image at the specified index.
         */
        const nyx::vkg::Image& image( unsigned idx = 0 ) const ;

       /** Method to retrieve the current index of image in use by the swapchain.
         * @return The current index of image in use by the swapchain.
         */
        unsigned current() const ;

        /** Method to tell this swapchain to acquire the next image in it's frambuffers.
         * @return The vkg library error to associate with this operation.
         */
        unsigned acquire() ;
        
        /** Method to submit this swapchain's presentation to the queue used for it's initialization.
         * @param sync The synchronization object used to sync this operation so that it occurs after other GPU events.
         */
        unsigned submit() ;
        
        /** Method to reset this object and deallocate all data.
         */
        void reset() ;

      private:
        
        friend class RenderPassData ;

        /** Method to retrieve a const pointer to the start of this object's image list.
         * @see Swapchain::count() for the amount of images in the list.
         * @return Const pointer to the start of the image list.
         */
        const nyx::vkg::Image* images() const ;

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

