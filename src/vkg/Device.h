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
 * File:   Device.h
 * Author: Jordan Hendl
 *
 * Created on December 23, 2020, 8:03 PM
 */
#pragma once

namespace vk
{
  class Device         ;
  class PhysicalDevice ;
  class SurfaceKHR     ;
}

namespace nyx
{
  namespace vkg
  {
    class Queue   ;
    class Surface ;
    class Device 
    {
      public:
        
        /** Default constructor.
         */
        Device() ;
        
        /** Copy Constructor. Copies the input's data.
         * @param src The Vulkan Device to copy from.
         */
        Device( const Device& src ) ;
        
        /** Default deconstructor.
         */
        ~Device() ;
        
        /** Equals operator. Copies the input's device data.
         * @param src The source to copy from.
         * @return Reference to this object after the copy is performed.
         */
        Device& operator=( const Device& src ) ;
        
        /** Conversion operator for a vulkan device, so this object can be used in the place of that as well.
         */
        operator const ::vk::Device&() const ;
        
        /** Conversion operator for a vulkan device, so this object can be used in the place of that as well.
         */
        operator ::vk::Device() const ;
        
        /** Conversion operator for a vulkan device, so this object can be used in the place of that as well.
         */
        operator const ::vk::Device&() ;
        
        /** Method to reset and deallocate all data from this device.
         */
        void reset() ;
        
        /** Method to query this device to see if an extension is loaded.
         * @param ext_name The name of the extension to be checked for.
         * @return Whether or not this device has the extension loaded.
         */
        bool hasExtension( const char* ext_name ) const ;

        /** Method to set the priority of the input queue id.
         * @param queue_index the index of queue in this family to set the priority of.
         * @param priority The 0.0 - 1.0 priority for this queue.
         */
        void setGraphicsPriority( unsigned queue_index, float priority ) ;

        /** Method to set the priority of the input queue id.
         * @param queue_index the index of queue in this family to set the priority of.
         * @param priority The 0.0 - 1.0 priority for this queue.
         */
        void setComputePriority( unsigned queue_index, float priority ) ;
        
        /** Method to set the priority of the input queue id.
         * @param queue_index the index of queue in this family to set the priority of.
         * @param priority The 0.0 - 1.0 priority for this queue.
         */
        void setTransferPriority( unsigned queue_index, float priority ) ;

        /** Method to set the priority of the input queue id.
         * @param queue_index the index of queue in this family to set the priority of.
         * @param priority The 0.0 - 1.0 priority for this queue.
         */
        void setPresentPriority( unsigned queue_index, float priority ) ;
        
        /** Method to set the number of graphics queues to generate ( if possible ) for this device.
         * @param amt The amount of graphics queues to create.
         */
        void setGraphicsQueueCount( unsigned amt ) ;
        
        /** Method to set the number of graphics queues to generate ( if possible ) for this device.
         * @param amt The amount of graphics queues to create.
         */
        void setComputeQueueCount( unsigned amt ) ;
        
        /** Method to set the number of graphics queues to generate ( if possible ) for this device.
         * @param amt The amount of graphics queues to create.
         */
        void setTransferQueueCount( unsigned amt ) ;
        
        /** Conversion operator for a vulkan device, so this object can be used in the place of that as well.
         */
        operator ::vk::Device() ;
        
        /** Convertion operator for an unsigned integer. Retrieves the ID of this device.
         */
        operator unsigned() const ;

        /** Method to retrieve whether or not this object is initialized.
         * @return Whether or not this object is initialized.
         */
        bool initialized() const ;

        /** Method to initialize this object with the input physical device.
         * @param physical_device The physical device to use for this device's definitions.
         */
        void initialize( const ::vk::PhysicalDevice& physical_device, unsigned id ) ;
        
        /** Method to initialize this object with the input physical device.
         * @param physical_device The physical device to use for this device's definitions.
         * @param surface The surface to use to generate a present queue.
         */
        void initialize( const ::vk::PhysicalDevice& physical_device, unsigned id, const ::vk::SurfaceKHR& surface ) ;
        
        /** Method to add a validation layer to this device upon it's creation.
         * @param layer_name The name of the validation layer to use. If it is invalid, it is skipped.
         */
        void addValidationLayer( const char* layer_name ) ;

        /** Method to add an extension to be loaded when the device is loaded.
         * @param extension_name The name of the extension to be loaded, if available.
         */
        void addExtension( const char* extension_name ) ;
        
        /** Method to retrieve a const-reference to this object's internal Vulkan device handle.
         * @return Const-reference to the Vulkan device handle of this object.
         */
        const ::vk::Device& device() const ;
        
        /** Method to retrieve a const-reference to this object's internal Vulkan physical device handle.
         * @return Const-reference to the Vulkan physical device handle of this object.
         */
        const ::vk::PhysicalDevice& physicalDevice() const ;
        
        /** Method to retrieve a graphics queue from the device, if available.
         * @return A const reference to a created graphics queue.
         */
        const nyx::vkg::Queue& graphicsQueue() const ;
        
        /** Method to retrieve a present queue from the device, if available.
         * @return A const reference to the created present queue.
         */
        const nyx::vkg::Queue& presentQueue( const vkg::Surface& surface ) const ;
        
        /** Method to retrieve a compute queue from the device, if available.
         * @return A const reference to the created present queue.
         */
        const nyx::vkg::Queue& computeQueue() const ;
        
        /** Method to retrieve a transfer queue from the device, if available.
         * @return A const reference to the created transfer queue.
         */
        const nyx::vkg::Queue& transferQueue() ;
        
        /** Method to enable the Sampler Anisotropy device feature.
         * @param value Whether or not to enable the feature.
         */
        void setSamplerAnisotropy( bool value ) ;
        
        /** Method to enable the Dual Source Blend device feature.
         * @param value Whether or not to enable the feature.
         */
        void setDualSrcBlend( bool value ) ;
        
        /** Method to enable the Robust Buffer Access device feature.
         * @param value Whether or not to enable the feature.
         */
        void setRobustBufferAccess( bool value ) ;
        
        /** Method to wait for all operations on this device to finish.
         */
        void wait() const ;

      private:
        
        /** Forward declared structure containing this object's data.
         */
        struct DeviceData *device_data ;
        
        /** Method to retrieve a reference to this object's internal data structure.
         * @return Reference to this object's internal data structure.
         */
        DeviceData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data structure.
         * @return Const-reference to this object's internal data structure.
         */
        const DeviceData& data() const ;
    };
  }
}

