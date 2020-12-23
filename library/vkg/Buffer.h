#ifndef KGL_VKG_BUFFER_H
#define KGL_VKG_BUFFER_H

namespace vk
{
       template <typename BitType, typename MaskType>
       class Flags ;
       
       class Device              ;
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
         * @param buffer The buffer to copy from.
         * @param cmd_buff The command buffer to record the copy operation to.
         */
        void copy( const Buffer& buffer, ::vk::CommandBuffer cmd_buff ) ;
        
        /** Method to initialize this object using the input pre-allocated memory object. Does not allocate any extra data.
         * @param prealloc The pre-allocated memory object to use for this object's internal memory.
         */
        void initialize( kgl::Memory<kgl::vkg::Vulkan>& prealloc ) ;
        
        /** Method to initialize this object using the input parameters.
         * @param gpu The device to use for all GPU calls.
         * @param size The size in bytes to allocate for this object.
         * @param host_local Whether to allocate a host-copy of this data.
         */
        void initialize( const vk::Device& gpu, unsigned size, bool host_local = false ) ;
        
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

