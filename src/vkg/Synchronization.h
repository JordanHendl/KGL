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
 * File:   Synchronization.h
 * Author: Jordan Hendl
 *
 * Created on January 8, 2021, 7:17 PM
 */

#ifndef KGL_VKG_SYNCHRONIZATION_H
#define KGL_VKG_SYNCHRONIZATION_H

namespace vk
{
  class Fence          ;
  class Semaphore      ;
  class Device         ;
  class PhysicalDevice ;
}

namespace kgl
{
  namespace vkg
  {
    class Device ;

    /** Object to manage Vulkan synchronization primitives.
     */
    class Synchronization
    {
      public:
        
        /** Default constructor.
         */
        Synchronization() ;
        
        /** Copy constructor. Copies the input object into this one.
         * @param sync The object to copy.
         */
        Synchronization( const Synchronization& sync ) ;
        
        /** Default deconstructor.
         */
        ~Synchronization() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param sync The object to assign this object to.
         * @return A reference to this object after assignment.
         */
        Synchronization& operator=( const Synchronization& sync ) ;
        
        /** Method to retrieve the physical device used for this object's creation.
         * @return The device used for this object's creation.
         */
        const vk::Device& device() const ;

        /** Method to retrieve the physical device used for this object's creation.
         * @return The device used for this object's creation.
         */
        const vk::PhysicalDevice& physicalDevice() const ;
        
        /** Method to initialize this object. 
         * @param device The device to use for object creation.
         * @param num_sems The amount of signal semaphores to create.
         * @param num_fences The amount of fences to create.
         */
        void initialize( const kgl::vkg::Device& device, unsigned num_sems = 1 ) ;
        
        /** Method to add the input synchronization object to be waited on by this one.
         * @param sync The synchronization object to wait on for this current object's operation.
         */
        void waitOn( const kgl::vkg::Synchronization& sync ) ;
        
        /** Method to retrieve the number of fences created by this object.
         * @return The number of fences created by this object.
         */
        unsigned numFences() const ;
        
        /** Method to retrieve the number of signal semaphores created by this object.
         * @return The number of signal semaphores created by this object.
         */
        unsigned numSignals() const ;
        
        /** Method to retrieve the number of semaphores this object is to wait on.
         * @return The number of semaphores this object is set to wait on.
         */
        unsigned numWaitSems() const ;
        
        /** Method to retrieve the specified semaphore to be signaled.
         * @param idx The index of semaphore to grab.
         * @return Const reference to the signal semaphore.
         */
        const vk::Semaphore& signal( unsigned idx = 0 ) const ;
        
        /** Method to retrieve the specified semaphore to be waited upon.
         * @param idx The index of semaphore to grab.
         * @return Const reference to the wait semaphore.
         */
        const vk::Semaphore& wait( unsigned idx = 0 ) const ;
        
        /** Method to retrieve the specified fence to be signaled.
         * @param idx The index of fence to grab.
         * @return Const reference to the signal fence.
         */
        const vk::Fence& signalFence() const ;
        
        /** Method to retrieve the specified fence to be waited on.
         * @param idx The index of fence to grab.
         * @return Const reference to the wait fence.
         */
        const vk::Fence& waitFence( unsigned idx = 0 ) const ;
        
        /** Method to retrieve const pointer to the start of this object's signal semaphores.
         * @return Const-pointer to the start of this object's signal semaphores.
         */
        const vk::Semaphore* signals() const ;
        
        /** Method to retrieve const pointer to the start of this object's wait semaphores.
         * @return Const-pointer to the start of this object's wait semaphores.
         */
        const vk::Semaphore* waits() const ;
        
        /** Method to retrieve const pointer to the start of this object's wait fences.
         * @return Const-pointer to the start of this object's wait fences.
         */
        const vk::Fence* waitFences() const ;
        
        /** Method to retrieve const pointer to the start of this object's signal fences.
         * @return Const-pointer to the start of this object's signal fences.
         */
        const vk::Fence* signalFences() const ;
        
        /** Method to wait on the fences specfied wait fences of this object.
         */
        void waitOnFences() ;

        /** Method to clear all waiting objects of this object.
         */
        void clear() ;
      private:
        
        /** Forward declared structure to contain this object's internal data.
         */
        struct SynchronizationData *sync_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        SynchronizationData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const SynchronizationData& data() const ;
    };
  }
}

#endif 

