#ifndef KGL_MEMORY_H
#define KGL_MEMORY_H

namespace kgl
{
  class List ;

  /** Template function to combine parameters.
   * @return The parameters bitwise OR'd together.
   */
  template<typename T>
  static T combine( T first, T second )
  {
    return first | second ;
  }

  /** Template function to combine parameters.
   * @return The parameters bitwise OR'd together.
   */
  template<typename T, typename... TYPES>
  static T combine( T first, T second, TYPES... types )
  {
    return first | second | combine( types... ) ;
  }
    
  /** Class to manage memory on the GPU & CPU.
   */
  template<typename IMPL>
  class Memory
  {
    public:
      
      /** Default constructor
       */
      Memory() ;
      
      /** Default Deconstructor.
       */
      ~Memory() = default ;
      
      /** Method to initialize this memory object with the input parameters.
       * @param sz The number of elements to store in this memory object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param mem_flags The implementation-specific memory flags. OR'd together to get the flags to use.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */
      template<typename TYPE, typename ... BUFFER_FLAGS, typename ... MEMORY_FLAGS>
      void initialize( unsigned sz, typename IMPL::Device& gpu, MEMORY_FLAGS... mem_flags, bool host_alloc = true ) ;

      /** Method to initialize this memory object with the input parameters.
       * @param sz The number of elements to store in this memory object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */      
      template<typename TYPE>
      void initialize( unsigned sz, typename IMPL::Device& gpu, bool host_alloc = true ) ;
        
      /** Method to copy the input memory object into this.
       * @param src The memory object of the same implementation to copy.
       * @param amt_to_copy The amount of memory, in bytes, to copy. Defaults to entire object.
       * @param offset The offset into the input memory object to start copying at. Defaults to the start.
       */
      void copy( const Memory<IMPL>& src, unsigned amt_to_copy = 0, unsigned offset = 0 ) ;
      
      /** Method to copy the input memory object into this.
       * @param src The memory object of the same implementation to copy.
       * @param cmd_buff The implementation-specific command record to push the copy command to.
       * @param amt_to_copy The amount of memory, in bytes, to copy. Defaults to entire object.
       * @param offset The offset into the input memory object to start copying at. Defaults to the start.
       */
      void copy( const Memory<IMPL>& src, typename IMPL::CommandRecord& cmd_buff, unsigned amt_to_copy = 0, unsigned offset = 0 ) ;
      
      /** Method to copy the input host ( CPU-side ) data into this object's host & GPU copies.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param offset The offset in indices of this object's memory to copy to.
       * @param src_offset The offset in indices of the input data to copy from.
       */
      template<typename TYPE>
      void copySynced( const TYPE* src, unsigned amt, unsigned offset = 0, unsigned src_offset = 0 ) ;

      /** Method to copy the input host ( CPU-side ) data into this object's host data.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param offset The offset in indices of this object's memory to copy to.
       * @param src_offset The offset in indices of the input data to copy from.
       */      
      template<typename TYPE>
      void copyToHost( const TYPE* src, unsigned amt, unsigned offset = 0, unsigned src_offset = 0 ) ;
      
      /** Method to copy the input host ( CPU-side ) data into this object's GPU data.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param offset The offset in indices of this object's memory to copy to.
       * @param src_offset The offset in indices of the input data to copy from.
       */
      template<typename TYPE>
      void copyToDevice( const TYPE* src, unsigned amt, unsigned offset = 0, unsigned src_offset = 0 ) ;
      
      /** Method to retrieve a const reference to this object's internal implementation-specific memory.
       * @return 
       */
      const typename IMPL::Memory& memory() const ;
      
      /** Function to deallocate all memory allocated by this object.
       */
      void deallocate() ;
      
      /** Method to copy this object's host data to the GPU.
       */
      void syncToDevice() ;
      
      /** Method to copy this object's GPU data to the host ( RAM ).
       */
      void syncToHost() ;
      
      /** Returns whether this object is dirty and needs to be synced.
       * @return 
       */
      bool dirty() const ;
      
      /** Returns the size in elements of this object.
       * @return The size in elements of this object.
       */
      unsigned size() const ;
      
      /** Method to retrieve the size in bytes of this object.
       * @return The size of this object's memory in bytes.
       */
      unsigned byteSize() const ;
      
    private:
      typedef unsigned Size   ;
      typedef bool     Flag   ;
      typedef void*    Data   ;
      
      Flag                  dirty_bit  ;
      Flag                  host_alloc ;
      Data                  data       ;
      Size                  element_sz ;
      Size                  count      ;
      Size                  offset     ;
      IMPL                  impl       ;
      typename IMPL::Device gpu        ;
      typename IMPL::Memory *memory_ptr;
  };
  
  /** Class to manage memory on the GPU & CPU.
   */
  template<typename IMPL>
  Memory<IMPL>::Memory()
  {
    this->dirty_bit  = false   ;
    this->data       = nullptr ;
    this->count      = 0       ;
    this->element_sz = 0       ;
    this->offset     = 0       ;
    this->memory_ptr = nullptr ;
    this->host_alloc = false   ;
  }
  
  template<typename IMPL>
  void Memory<IMPL>::deallocate()
  {
    if( this->data )
    {
      delete this->data ;
    }

    impl.free( this->memory, this->gpu ) ;
    
    this->memory = nullptr ;
  }

  template<typename IMPL>
  template<typename TYPE, typename ... BUFFER_FLAGS, typename ... MEMORY_FLAGS>
  void Memory<IMPL>::initialize( unsigned sz, typename IMPL::Device& gpu, MEMORY_FLAGS... mem_flags, bool host_alloc ) 
  {
    this->count      = sz             ;
    this->element_sz = sizeof( TYPE ) ;
    this->gpu        = gpu            ;
    
    this->memory_ptr = impl.createMemory( sz * sizeof( TYPE ), gpu, ::kgl::combine( 0, mem_flags... ) ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz * sizeof( TYPE ) ] ;
    }
  }

  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::initialize( unsigned sz, typename IMPL::Device& gpu, bool host_alloc ) 
  {
    this->count      = sz             ;
    this->element_sz = sizeof( TYPE ) ;
    this->gpu        = gpu            ;
    
    this->memory_ptr = impl.createMemory( sz * sizeof( TYPE ), gpu ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz * sizeof( TYPE ) ] ;
    }
  }

  template<typename IMPL>
  void Memory<IMPL>::copy( const Memory<IMPL>& src, unsigned amt_to_copy, unsigned offset )
  {
    this->impl.copyTo( src.memory_ptr, this->memory_ptr, offset ) ;
  }
  
  template<typename IMPL>
  void Memory<IMPL>::copy( const Memory<IMPL>& src, typename IMPL::CommandRecord& cmd_buff, unsigned amt_to_copy, unsigned offset )
  {
    this->impl.copyTo( this->memory_ptr, src.memory_ptr, cmd_buff, offset ) ;
  }
  
  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::copySynced( const TYPE* src, unsigned amt, unsigned offset, unsigned src_offset )
  {
    this->copyToDevice( src, amt, offset, src_offset ) ;
    this->copyToHost  ( src, amt, offset, src_offset ) ;
  }

  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::copyToHost( const TYPE* src, unsigned token_amt, unsigned offset, unsigned src_offset )
  {
    auto amt = token_amt > this->count ? this->count : token_amt ;
    
    static_assert( sizeof( TYPE ) == this->element_sz, "The size of the input is different than this object. Stop." ) ;

    memcpy( this->data, src, amt * this->element_sz ) ;
  }

  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::copyToDevice( const TYPE* src, unsigned token_amt, unsigned offset, unsigned src_offset )
  {
    auto amt = token_amt > this->count ? this->count : token_amt ;
    
    static_assert( sizeof( TYPE ) == this->element_sz, "The size of the input is different than this object. Stop." ) ;

    this->impl.copyTo( static_cast<void*>( src ), this->memory_ptr, amt ) ;
  }

  template<typename IMPL>
  const typename IMPL::Memory& Memory<IMPL>::memory() const
  {
    return this->memory ;
  }
  
  template<typename IMPL>
  void Memory<IMPL>::syncToDevice() 
  {
    this->impl.copyTo( this->data, this->memory_ptr, this->element_sz * this->count ) ;
  }
  
  template<typename IMPL>
  void Memory<IMPL>::syncToHost() 
  {
    this->impl.copyTo( this->memory_ptr, this->data, this->element_sz * this->count ) ;
  } 

  template<typename IMPL>
  unsigned Memory<IMPL>::size() const
  {
    return this->count * this->element_sz ;
  }
  
  template<typename IMPL>
  unsigned Memory<IMPL>::byteSize() const
  {
    return this->count * this->element_sz ;
  }
}
#endif 

