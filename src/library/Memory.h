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

#ifndef KGL_MEMORY_H
#define KGL_MEMORY_H

namespace kgl
{
  /** Template function to combine parameters.
   * @return The parameters bitwise OR'd together.
   */
  template<typename T>
  static T combine( T first )
  {
    return first ;
  }

  /** Template function to combine parameters.
   * @return The parameters bitwise OR'd together.
   */
  template<typename T, typename... TYPES>
  static T combine( T first, TYPES... types )
  {
    return( first | combine( types... ) ) ;
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
      
      /** Copy constructor. Copies the values of the input, does not perform a deep copy.
       * @param src The input to copy.
       */
      Memory( const Memory<IMPL>& src ) ;
      
      /** Default Deconstructor.
       */
      ~Memory() = default ;
      
      /** Operator to assign this object to another memory object.
       * @param src The memory to copy.
       * @return Reference to this object after the assignment.
       */
      Memory<IMPL>& operator=( const Memory<IMPL>& src ) ;
      
      /** Operator to return a copy of this object at the specified offset
       * @param offset The amount to offset this object into the memory.
       */
      Memory<IMPL> operator+( unsigned offset ) const ;

      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename IMPL::Memory&() const ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename IMPL::Memory() const ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename IMPL::Memory&() ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename IMPL::Memory() ;
        
      /** Method to initialize this memory object with the input parameters.
       * @param sz The size in bytes to store in this object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param mem_flags The implementation-specific memory flags. OR'd together to get the flags to use.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */
      template<typename ... MEMORY_FLAGS>
      void initialize( const typename IMPL::Device& gpu, unsigned sz, bool host_alloc, MEMORY_FLAGS... mem_flags ) ;
      
      /** Method to initialize this memory object with the input parameters.
       * @param sz The size in bytes to store in this object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */      
      void initialize( const typename IMPL::Device& gpu, unsigned sz, bool host_alloc = true ) ;
        
      /** Method to retrieve the host buffer of this object's data.
       * @return The host-buffer containing this object's data.
       */
      const void* hostData() const ;

      /** Method to retrieve the implementation-specific device used by this object.
       * @return The implementation-specific device used by the object.
       */
      const typename IMPL::Device& device() const ;

      /** Method to retrieve the offset into the memory handle this object's allowed to use.
       * @return The offset into the memory handle this object is allowed to use.
       */
      unsigned offset() const ;
      
      /** Method to copy the input memory object into this.
       * @param src The memory object of the same implementation to copy.
       * @param amt_to_copy The amount of memory, in bytes, to copy. Defaults to entire object.
       * @param offset The offset into the input memory object to start copying at. Defaults to the start.
       */
      void copy( const Memory<IMPL>& src, unsigned amt_to_copy = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the input host ( CPU-side ) data into this object's host & GPU copies.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param offset The offset in indices of this object's memory to copy to.
       * @param src_offset The offset in indices of the input data to copy from.
       */
      template<typename TYPE>
      void copySynced( const TYPE* src, unsigned amt, unsigned offset = 0, unsigned src_offset = 0, unsigned dstoffset = 0 ) ;

      /** Method to copy the input host ( CPU-side ) data into this object's host data.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param offset The offset in indices of this object's memory to copy to.
       * @param src_offset The offset in indices of the input data to copy from.
       */      
      template<typename TYPE>
      void copyToHost( const TYPE* src, unsigned amt, unsigned offset = 0, unsigned src_offset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the input host ( CPU-side ) data into this object's GPU data.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param offset The offset in indices of this object's memory to copy to.
       * @param src_offset The offset in indices of the input data to copy from.
       */
      template<typename TYPE>
      void copyToDevice( const TYPE* src, unsigned amt, unsigned src_offset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to retrieve a const reference to this object's internal implementation-specific memory.
       * @return 
       */
      const typename IMPL::Memory& memory() const ;
      
      /** Function to deallocate all memory allocated by this object.
       */
      void deallocate() ;
      
      /** Method to retrieve the element size of this object.
       * @return The size of each element of this object.
       */
      unsigned elementSize() const ;

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
      
      /** Returns the size in bytes of this object.
       * @return The size in bytes of this object.
       */
      unsigned size() const ;
      
    private:
      typedef unsigned       Size   ;
      typedef bool           Flag   ;
      typedef unsigned char* Data   ;
      
      Flag                  dirty_bit  ;
      Flag                  host_alloc ;
      Data                  data       ;
      Size                  byte_size  ;
      Size                  mem_offset ;
      IMPL                  impl       ;
      typename IMPL::Device gpu        ;
      typename IMPL::Memory memory_ptr ;
  };
  
  template<typename IMPL>
  Memory<IMPL>::Memory()
  {
    this->dirty_bit  = false   ;
    this->data       = nullptr ;
    this->byte_size  = 0       ;
    this->mem_offset = 0       ;
    this->memory_ptr = nullptr ;
    this->host_alloc = false   ;
  }
  
  template<typename IMPL>
  Memory<IMPL>::Memory( const Memory<IMPL>& src )
  {
    *this = src ;
  }
  
  template<typename IMPL>
  Memory<IMPL>& Memory<IMPL>::operator=( const Memory<IMPL>& src )
  {
    this->dirty_bit  = src.dirty_bit  ;
    this->data       = src.data       ;
    this->byte_size  = src.byte_size  ;
    this->mem_offset = src.mem_offset ;
    this->memory_ptr = src.memory_ptr ;
    this->host_alloc = src.host_alloc ;
    this->gpu        = src.gpu        ;
    this->impl       = src.impl       ;
    
    return *this ;
  }
  
  template<typename IMPL>
  const void* Memory<IMPL>::hostData() const
  {
    return static_cast<const void*>( this->data ) ;
  }

  template<typename IMPL>
  Memory<IMPL> Memory<IMPL>::operator+( unsigned offset ) const
  {
    const unsigned next_offset = this->mem_offset + offset ;
    Memory<IMPL> memory ;
    
    memory            = *this                                                     ;
    memory.mem_offset = next_offset >= this->size() ? this->size() :  next_offset ;

    return memory ;
  }
  
  template<typename IMPL>
  Memory<IMPL>::operator const typename IMPL::Memory&() const
  {
    return this->memory_ptr ;
  }
  
  template<typename IMPL>
  Memory<IMPL>::operator typename IMPL::Memory() const
  {
    return this->memory_ptr ;
  }
  
  template<typename IMPL>
  Memory<IMPL>::operator const typename IMPL::Memory&()
  {
    return this->memory_ptr ;
  }
  
  template<typename IMPL>
  Memory<IMPL>::operator typename IMPL::Memory()
  {
    return this->memory_ptr ;
  }

  template<typename IMPL>
  void Memory<IMPL>::deallocate()
  {
    if( this->data )
    {
      delete this->data ;
    }

    impl.free( this->memory_ptr, this->gpu ) ;
  }

  template<typename IMPL>
  template< typename ... MEMORY_FLAGS>
  void Memory<IMPL>::initialize( const typename IMPL::Device& gpu, unsigned sz, bool host_alloc, MEMORY_FLAGS... mem_flags ) 
  {
    this->byte_size  = sz  ;
    this->gpu        = gpu ;
    
    this->memory_ptr = impl.createMemory( gpu, sz, static_cast<typename IMPL::MemoryFlags>( ::kgl::combine( mem_flags... ) ) ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz ] ;
    }
  }

  template<typename IMPL>
  void Memory<IMPL>::initialize( const typename IMPL::Device& gpu, unsigned sz, bool host_alloc ) 
  {
    this->byte_size  = sz  ;
    this->gpu        = gpu ;
    
    this->memory_ptr = impl.createMemory( gpu, sz ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz ] ;
    }
  }

  template<typename IMPL>
  void Memory<IMPL>::copy( const Memory<IMPL>& src, unsigned amt_to_copy, unsigned srcoffset, unsigned dstoffset )
  {
    this->impl.copyTo( src.memory_ptr, this->memory_ptr, offset ) ;
  }
  
  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::copySynced( const TYPE* src, unsigned amt, unsigned offset, unsigned src_offset, unsigned dstoffset )
  {
    this->copyToDevice( src, amt, offset, src_offset ) ;
    this->copyToHost  ( src, amt, offset, src_offset ) ;
  }

  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::copyToHost( const TYPE* src, unsigned token_amt, unsigned offset, unsigned src_offset, unsigned dstoffset )
  {
    auto amt = token_amt > this->byte_size ? this->byte_size : token_amt ;
    
    memcpy( this->data, src, amt * this->element_sz ) ;
  }

  template<typename IMPL>
  template<typename TYPE>
  void Memory<IMPL>::copyToDevice( const TYPE* src, unsigned byte_amt, unsigned src_offset, unsigned dstoffset )
  {
    auto amt = byte_amt > this->byte_size ? this->byte_size : byte_amt ;
    
    this->impl.copyToDevice( static_cast<const void*>( src ), this->memory_ptr, this->gpu, amt ) ;
  }

  template<typename IMPL>
  const typename IMPL::Device& Memory<IMPL>::device() const
  {
    return this->gpu ;
  }
  
  template<typename IMPL>
  unsigned Memory<IMPL>::offset() const
  {
    return this->mem_offset ;
  }
  
  template<typename IMPL>
  const typename IMPL::Memory& Memory<IMPL>::memory() const
  {
    return this->memory_ptr ;
  }
  
  template<typename IMPL>
  void Memory<IMPL>::syncToDevice() 
  {
    this->impl.copyToDevice( this->data, this->memory_ptr, this->gpu, this->element_sz * this->byte_size ) ;
  }
  
  template<typename IMPL>
  void Memory<IMPL>::syncToHost() 
  {
    this->impl.copyToHost( static_cast<typename IMPL::Memory>( this->memory_ptr ), this->data, this->gpu, this->byte_size ) ;
  } 

  template<typename IMPL>
  unsigned Memory<IMPL>::size() const
  {
    return this->byte_size ;
  }
}
#endif 

