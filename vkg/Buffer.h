#ifndef KGL_VKG_BUFFER_H
#define KGL_VKG_BUFFER_H

namespace vk
{
       template <typename BitType, typename MaskType>
       class Flags ;
       
       class Buffer              ;
       class CommandBuffer       ;
  enum class BufferUsageFlagBits ;
}

namespace kgl
{
  template<class T>
  class Memory ;
  
  namespace vkg
  {
    class Vulkan ;
    class Device ;
    class Buffer
    {
      public:
        
        /** Alias for Vulkan flags.
         */
        using UsageFlags = ::vk::Flags<::vk::BufferUsageFlagBits, unsigned> ;

        /** Default Constructor.
         */
        Buffer() ;
        
        /** Copy constructor. Does a surface copy of the input source.
         */
        Buffer( const Buffer& src ) ;
        
        /** Default Constructor
         */
        ~Buffer() ;
        
        /** Equals operator. Performs a surface copy of the input source
         * @return A reference to this object after the surface copy.
         */
        Buffer& operator=( const Buffer& src ) ;
        
        /** Method to copy an input buffer into this object's data.
         * @param size The size in bytes to copy.
         * @param buffer The buffer to copy from.
         * @param cmd_buff The command buffer to record the copy operation to.
         * @param srcoffset The offset of the input buffer to start at.
         * @param dstoffset The offset of this buffer to start at.
         */
        void copy( const Buffer& buffer, unsigned size, ::vk::CommandBuffer cmd_buff, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
        
        /** Method to initialize this object using the input pre-allocated memory object. Does not allocate any extra data.
         * @param prealloc The pre-allocated memory object to use for this object's internal memory.
         * @param size The mount of the preallocated memory to use.
         * @return Whether or not this object was successfully initialized.
         */
        bool  initialize( kgl::Memory<kgl::vkg::Vulkan>& prealloc, unsigned size = 0 ) ;
        
        /** Method to initialize this object using the input parameters.
         * @param gpu The device to use for all GPU calls.
         * @param size The size in bytes to allocate for this object.
         * @param host_local Whether to allocate a host-copy of this data.
         * @return Whether or not this buffer was successfully initialized.
         */
        bool initialize( const kgl::vkg::Device& gpu, unsigned size, bool host_local = false ) ;
        
        /** Method to return the size of this object on the GPU.
         * @return The size in bytes of this object on the GPU.
         */
        unsigned size() const ;
        
        /** Method to retrieve a reference to this object's internal memory container.
         * @return Reference to this object's internal memory container.
         */
        kgl::Memory<kgl::vkg::Vulkan>& memory() ;
        
        /** Method to retrieve a const-reference to this object's internal memory container.
         * @return Const-reference to this object's internal memory container.
         */
        const kgl::Memory<kgl::vkg::Vulkan>& memory() const ;
        
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
        void setUsage( vk::BufferUsageFlagBits flag  ) ;
        
        /** Method to set the Vulkan usage for this buffer.
         * @param flag The Vulkan usage flag to use for this object.
         */
        void setUsage( Buffer::UsageFlags flag  ) ;

      private:
        
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

#endif

