#ifndef KGL_VULKAN_H
#define KGL_VULKAN_H

#ifdef KGL_VULKAN_FOUND // Won't get compiler errors when building this without vulkan.

namespace vk
{
  class Buffer         ;
  class DeviceMemory   ;
  class Device         ;
  class PhysicalDevice ;
  class CommandBuffer  ;
  class Instance       ;
}

namespace kgl
{
  template<typename IMPL>
  class Memory ;

  namespace vkg
  {
    class Vulkan
    {
      public:
        using Buffer        = ::vk::Buffer        ;
        using Memory        = ::vk::DeviceMemory  ;
        using Device        = ::vk::Device        ;
        using CommandRecord = ::vk::CommandBuffer ;
        
        /** Static method to initialize this implementation with a vulkan instance.
         * @param instance
         */
        static void initialize( const ::vk::Instance& instance ) ;
        
        /** Static method to convert a vulkan error to an error of the library.
         * @param error An error defined by vulkan.
         * @return An error defined by the library.
         */
        static unsigned convertError( unsigned error ) ;

      private:
        
        class MemoryFlags
        {
          public:
            MemoryFlags() ;
            unsigned val() ;
            unsigned operator()() ;
            MemoryFlags& operator=( unsigned flags ) ;
          private:
            unsigned flag ;
        };
  
        class BufferFlags
        {
          public:
            BufferFlags() ;
            unsigned val() ;
            unsigned operator()() ;
            BufferFlags& operator=( unsigned flags ) ;
          private:
            unsigned flag ;
        };
  
        typedef void* Data ;
        friend class ::kgl::Memory<Vulkan> ;
  
        Vulkan() ;
  
        ~Vulkan() ;
  
        void copyTo( Data src, Memory* dst, vk::Device& gpu, unsigned amt ) ;
  
        void copyTo( Memory* src, Data dst, vk::Device& gpu, unsigned amt ) ;
        
        void copyTo( Memory* src, Memory* dst, unsigned amt ) ;
        
        void copyTo( Memory* src, Memory* dst, CommandRecord& record, unsigned amt ) ;
  
        void free( Memory* mem, vk::Device& gpu ) ;
  
        Memory* createMemory( unsigned size, vk::Device& gpu ) ;
        
        Memory* createMemory( unsigned size, vk::Device& gpu, MemoryFlags mem_flags ) ;
        
        struct VulkanData* vulkan_data ;
        
        VulkanData& data() ;
  
        const VulkanData& data() const ;
    };
  }
}

#endif

#endif

