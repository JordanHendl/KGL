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

#pragma once

typedef unsigned VkFlags ;

namespace vk
{
  class Buffer ;
}

namespace nyx
{
  template<class T>
  class Memory ;
  
  template<typename Impl, typename Type>
  class Array ;

  class ArrayFlags ;
  
  using DeviceAddress = unsigned long ;
  
  namespace vkg
  {
    class Vulkan        ;
    class Device        ;
    class CommandBuffer ;
    class Image         ;
    class Descriptor    ;

    /** Class for managing a Vulkan buffer.
     */
    class Buffer
    {
      public:
        friend class nyx::vkg::Vulkan     ;
        friend class nyx::vkg::Descriptor ;
        /** Default Constructor.
         */
        Buffer() ;
        
        /** Copy constructor. Does a surface copy of the input source.
         */
        Buffer( const Buffer& src ) ;
        
        /** Default Constructor
         */
        ~Buffer() ;

        private:
          
        /** Friend decleration so the template interface can access this object's functionality.
         */
        template<typename Impl, typename Type>
        friend class nyx::Array ;
        
        /** Friend decleration so the vkg::Image object can access this object's data for copy.
         */
        friend class vkg::Image ;
        
        /** Friend decleration so the command buffer can draw this object.
         */
        friend class vkg::CommandBuffer ;

        /** Equals operator. Performs a surface copy of the input source
         * @return A reference to this object after the surface copy.
         */
        Buffer& operator=( const Buffer& src ) ;
        
        /** Method to copy an input buffer into this object's data.
         * @param buffer The buffer to copy from.
         * @param byte_size The size in bytes to copy.
         * @param cmd_buff The command buffer to record the copy operation to.
         * @param srcoffset The offset of the input buffer to start at.
         * @param dstoffset The offset of this buffer to start at.
         */
        void copy( const Buffer& buffer, unsigned byte_size, const nyx::vkg::CommandBuffer& cmd_buff, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
        
        /** Method to copy an input buffer into this object's data.
         * @param src Host pointer to copy data from.
         * @param byte_size The size in bytes to copy.
         * @param srcoffset The offset of the input buffer to start at.
         * @param dstoffset The offset of this buffer to start at.
         */
        void copyToDevice( const void* src, unsigned byte_size, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
        
        /** Method to copy an input buffer into this object's data.
         * @param size The size in bytes to copy.
         * @param buffer The buffer to copy from.
         * @param cmd_buff The command buffer to record the copy operation to.
         * @param srcoffset The offset of the input buffer to start at.
         * @param dstoffset The offset of this buffer to start at.
         */
        void copyToHost( const void* src, unsigned size, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;

        /** Method to retrieve whether or not this object is initialized.
         * @return Whether or not this buffer is initialized
         */
        bool initialized() const ;
        
        /** If the system supports it, returns a 64-bit address of the start of this buffer on the device.
         * @param offset The offset in bytes to start the address at.
         * @return The Device address starting at the beginning of this buffer.
         */
        nyx::DeviceAddress address( unsigned offset = 0 ) const ;
        
        /** Method to initialize this object using the input pre-allocated memory object. Does not allocate any extra data.
         * @param prealloc The pre-allocated memory object to use for this object's internal memory.
         * @param size The mount of the preallocated memory to use.
         * @return Whether or not this object was successfully initialized.
         */
        bool initialize( nyx::Memory<nyx::vkg::Vulkan>& prealloc, unsigned size ) ;
        
        /** Method to initialize this object using the input pre-allocated memory object. Does not allocate any extra data.
         * @param prealloc The pre-allocated memory object to use for this object's internal memory.
         * @param size The mount of the preallocated memory to use.
         * @return Whether or not this object was successfully initialized.
         */
        bool initialize( nyx::Memory<nyx::vkg::Vulkan>& prealloc, nyx::ArrayFlags, unsigned size ) ;
        
        /** Method to initialize this object using the input parameters.
         * @param gpu The device to use for all GPU calls.
         * @param size The size in bytes to allocate for this object.
         * @param host_local Whether to allocate a host-copy of this data.
         * @return Whether or not this buffer was successfully initialized.
         */
        bool initialize( unsigned gpu, unsigned size, bool host_local = false ) ;
        
        /** Method to initialize this object using the input parameters.
         * @param gpu The device to use for all GPU calls.
         * @param size The size in bytes to allocate for this object.
         * @param host_local Whether to allocate a host-copy of this data.
         * @return Whether or not this buffer was successfully initialized.
         */
        bool initialize( unsigned gpu, unsigned size, bool host_local, nyx::ArrayFlags flags ) ;
        
        /** Method to return the size of this object on the GPU.
         * @return The size in bytes of this object on the GPU.
         */
        unsigned size() const ;
        
        /** Method to retrieve a reference to this object's internal memory container.
         * @return Reference to this object's internal memory container.
         */
        nyx::Memory<nyx::vkg::Vulkan>& memory() ;
        
        /** Method to sync this buffer to the host.
         */
        void syncToHost() ;
        
        /** Method to reitreve the device used for this buffer.
         * @return Const-reference to the device used for this buffer.
         */
        unsigned device() const ;

        /** Method to sync this buffer to the device.
         */
        void syncToDevice() ;

        /** Method to retrieve a const-reference to this object's internal memory container.
         * @return Const-reference to this object's internal memory container.
         */
        const nyx::Memory<nyx::vkg::Vulkan>& memory() const ;
        
        /** Method to retrieve a const-reference to this object's internal Vulkan buffer.
         * @return Const-reference to this object's internal Vulkan buffer.
         */
        const vk::Buffer& buffer() const ;
        
        /** Method to reset this buffer and free allocated data.
         * @note if preallocated, unbinds memory and does not deallocate.
         */
        void reset() ;

        /** Method to set the Vulkan usage for this buffer.
         * @param flag The Vulkan usage flag to use for this object.
         */
        void setUsage( nyx::ArrayFlags flag  ) ;
        
        /** Method to provide a base for the variadic-template initialization function.
         * @param gpu The device to use for this buffer's data.
         * @param size The amount of data to allocate.
         * @param host_local Whether or not this object to allocate a host-copy.
         * @param buffer_flags The flags to use for buffer creation.
         * @return 
         */
        bool initializeBase( unsigned gpu, unsigned size, bool host_local, unsigned buffer_flags ) ;
        
        /** Forward-declared structure to contain this object's internal data.
         */
        struct BufferData *buffer_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        BufferData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const BufferData& data() const ;
    };
  }
}

