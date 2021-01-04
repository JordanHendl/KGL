/*
 * The MIT License
 *
 * Copyright 2020 Jordan Hendl.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   Device.h
 * Author: Jordan Hendl
 *
 * Created on December 23, 2020, 8:03 PM
 */

#ifndef KGL_VKG_DEVICE_H
#define KGL_VKG_DEVICE_H

namespace vk
{
  class Device         ;
  class PhysicalDevice ;
  class SurfaceKHR     ;
}

namespace kgl
{
  namespace vkg
  {
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
        
        /** Method to initialize this object with the input physical device.
         * @param physical_device The physical device to use for this device's definitions.
         */
        void initialize( const ::vk::PhysicalDevice& physical_device ) ;
        
        /** Method to initialize this object with the input physical device.
         * @param physical_device The physical device to use for this device's definitions.
         * @param surface The surface to use to generate a present queue.
         */
        void initialize( const ::vk::PhysicalDevice& physical_device, const ::vk::SurfaceKHR& surface ) ;
        
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

#endif /* DEVICE_H */

