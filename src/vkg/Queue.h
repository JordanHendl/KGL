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
 * File:   Queue.h
 * Author: Jordan Hendl
 *
 * Created on December 30, 2020, 6:11 PM
 */

#ifndef KGL_VKG_QUEUE_H
#define KGL_VKG_QUEUE_H

#include "Swapchain.h"
#include "Synchronization.h"


namespace vk
{
  class Queue         ;
  class Device        ;
  class SubmitInfo    ;
  class Queue         ;
  class CommandBuffer ;
}
namespace kgl
{
  namespace vkg
  {
    class Synchronization ;
    class CommandBuffer   ;
    class Device          ;
    
    /** Class for managing a vulkan queue. 
     * This object should only be created via a library device object.
     */
    class Queue
    {
      public:
        
        /** Default constructor.
         */
        Queue() ;
        
        /** Copy constructor. Assigns this object to the input.
         * @param queue The queue to assign this object to.
         */
        Queue( const Queue& queue ) ; 
        
        /** Default deconstructor.
         */
        ~Queue() ;
        
        /** Method to retrieve the vulkan queue family associated with this queue.
         * @return The queue family associated with this queue.
         */
        unsigned family() const ;
        
        /** Conversion operator for boolean to check if this object is valid.
         * @return Whether or not this object is initalized.
         */
        operator bool() const ;

        /** Assignment operator. Assigns this queue to the input.
         * @param queue The queue to assign this object to.
         * @return Reference to this object after assignment.
         */
        Queue& operator=( const Queue& queue ) ;
        
        /** Method to retrieve the underlying vulkan queue of this object.
         * @return Const-reference to the underlying vulkan queue of this object.
         */
        const vk::Queue& queue() const ;
        
        /** Method to retrive the library device used for this object's queue creation.
         * @return The library device used for this object's creation.
         */
        const kgl::vkg::Device& device() const ;
        
        /** Method to submit a command to a queue.
         * @note This object handles concurrent CPU-side access to vulkan queues.
         * @param cmd_buff The command buffer to submit.
         */
        void submit( const kgl::vkg::CommandBuffer& cmd_buff ) ;
        
        /** Method to submit a command to a queue.
         * @note This object handles concurrent CPU-side access to vulkan queues.
         * @param cmd_buff The command buffer to submit.
         * @param sync The library synchronization object to synchronize this submition.
         */
        void submit( const kgl::vkg::CommandBuffer& cmd_buff, const kgl::vkg::Synchronization& sync ) ;

        /** Method to submit a command to a queue.
         * @note This object handles concurrent CPU-side access to vulkan queues.
         * @param cmd_buff The command buffer to submit.
         */
        void submit( const vk::CommandBuffer& cmd_buff ) ;

        /** Method to submit a command to a queue.
         * @note This object handles concurrent CPU-side access to vulkan queues.
         * @param cmd_buff The command buffer to submit.
         * @param sync The library synchronization object to synchronize this submition.
         */
        void submit( const vk::CommandBuffer& cmd_buff, const kgl::vkg::Synchronization& sync ) ;
        
        /** Method to submit a swapchain present operation to this queue.
         * @param swapchain The swapchain to submit the present operation for.
         * @param img_index The image index acquired by the swapchain.
         * @param sync The synchronization object to use for syncing the GPU operation.
         */
        void submit( const kgl::vkg::Swapchain& swapchain, unsigned img_index, const kgl::vkg::Synchronization& sync ) ;

      private:
        
        /** Friend decleration. This object should only be initialized through a device.
         */
        friend class kgl::vkg::Device ;

        /** Method to initialize this object for use by the library device.
         * @param device The device used in the creation of this object.
         * @param queue_family The index of queue family associated with this queue. 
         * @param queue_id The unique queue-id of this object.
         */
        void initialize( const kgl::vkg::Device& device, const vk::Queue& queue, unsigned queue_family, unsigned queue_id ) ;
        

        /** Forward Declared structure to contain this object's internal data.
         */
        struct QueueData *queue_data ;
        
        /** Method to retrieve a reference to this object's internal data structure.
         * @return Reference to this object's internal data structure.
         */
        QueueData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data structure.
         * @return const-reference to this object's internal data structure.
         */
        const QueueData& data() const ;
    };
  }
}

#endif /* QUEUE_H */

