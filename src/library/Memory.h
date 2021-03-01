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

#ifndef NYX_MEMORY_H
#define NYX_MEMORY_H

namespace nyx
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
  
  class MemoryFlags
  {
    public:
      
      enum
      {
        DeviceLocal   = 0x00000001,
        HostVisible   = 0x00000002,
        HostCoherent  = 0x00000004,
        HostCached    = 0x00000008,
      };
      
      /** Default constructor.
       */
      MemoryFlags() ;
      
      /** Copy constructor. Copies input into this object.
       * @param flags The object to copy into this one.
       */
      MemoryFlags( int flags ) ;
      
      /** Assignment operator. Assigns this object to the input. 
       * @param flag The flag to assign this object to.
       * @return Reference to this object after assignment.
       */
      MemoryFlags& operator=( int flag ) ;
      
      /** OR operator. Bitwise OR's this obect with the input.
       * @param flag The flag to bitwise OR this object with.
       * @return Reference to this object after the operation.
       */
      MemoryFlags& operator|( int flag ) ;
      
      /** Static conversion operator.
       * @return The underlying value of this object.
       */
      operator int() const ;
      
      /** Method to retrieve this object's enumeration.
       * @return This object's enumeration.
       */
      int value() const ;
      
      /** Method to set this object's enumeration value.
       * @param value The value to set this object's' enumeration to.
       */
      void set( int value ) ;
    private:
      
      /** The underlying enumeration bit.
       */
      int bit ;
  };

  /** Class to manage memory on the GPU & CPU.
   */
  template<typename Framework>
  class Memory
  {
    public:
      
      /** Default constructor
       */
      Memory() ;
      
      /** Copy constructor. Copies the values of the input, does not perform a deep copy.
       * @param src The input to copy.
       */
      Memory( const Memory<Framework>& src ) ;
      
      /** Default Deconstructor.
       */
      ~Memory() = default ;
      
      /** Operator to assign this object to another memory object.
       * @param src The memory to copy.
       * @return Reference to this object after the assignment.
       */
      Memory<Framework>& operator=( const Memory<Framework>& src ) ;
      
      /** Operator to return a copy of this object at the specified offset
       * @param offset The amount to offset this object into the memory.
       */
      Memory<Framework> operator+( unsigned offset ) const ;

      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename Framework::Memory&() const ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename Framework::Memory() const ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator const typename Framework::Memory&() ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      operator typename Framework::Memory() ;
        
      /** Method to retrieve whether or not this object is initialized.
       * @return Whether or not this object is initialized.
       */
      bool initialized() const ;

      /** Method to initialize this memory object with the input parameters.
       * @param sz The size in bytes to store in this object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param mem_flags The implementation-specific memory flags. OR'd together to get the flags to use.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */
      template<typename ... MEMORY_FLAGS>
      void initialize( unsigned gpu, unsigned sz, bool host_alloc, MEMORY_FLAGS... mem_flags ) ;
      
      /** Method to initialize this memory object with the input parameters.
       * @param sz The size in bytes to store in this object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */      
      void initialize( unsigned gpu, unsigned sz, bool host_alloc = true ) ;
      
      /** Method to initialize this memory object with the input parameters.
       * @param sz The size in bytes to store in this object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param mem_flags The implementation-specific memory flags. OR'd together to get the flags to use.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */
      template<typename ... MEMORY_FLAGS>
      void initialize( unsigned gpu, unsigned sz, unsigned filter, bool host_alloc, MEMORY_FLAGS... mem_flags ) ;
      
      /** Method to initialize this memory object with the input parameters.
       * @param sz The size in bytes to store in this object.
       * @param gpu The implementation-specific GPU to use for all gpu operations.
       * @param host_alloc Flag whether or not to allocate a copy of the date on the host ( CPU-side ).
       */      
      void initialize( unsigned gpu, unsigned sz, unsigned filter, bool host_alloc = true ) ;
        
      /** Method to retrieve the host buffer of this object's data.
       * @return The host-buffer containing this object's data.
       */
      const void* hostData() const ;

      /** Method to retrieve the implementation-specific device used by this object.
       * @return The implementation-specific device used by the object.
       */
      unsigned device() const ;

      /** Method to retrieve the offset into the memory handle this object's allowed to use.
       * @return The offset into the memory handle this object is allowed to use.
       */
      unsigned offset() const ;
      
      /** Method to copy the input memory object into this.
       * @param src The memory object of the same implementation to copy.
       * @param amt_to_copy The amount of memory, in bytes, to copy. Defaults to entire object.
       * @param srcoffset The offset into the input memory object to start copying at. Defaults to the start.
       * @param dstoffset The offset into the output memory object to start copying at. Defaults to the start.
       */
      void copy( const Memory<Framework>& src, unsigned amt_to_copy = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the input host ( CPU-side ) data into this object's host & GPU copies.
       * @param src Pointer to the input data to copy from.
       * @param amt The amount of data to copy.
       * @param src_offset The offset in indices of this object's memory to copy to.
       * @param dst_offset The offset in indices of the input data to copy from.
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
      const typename Framework::Memory& memory() const ;
      
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
      
      Flag                       dirty_bit  ;
      Flag                       host_alloc ;
      Data                       data       ;
      Size                       byte_size  ;
      Size                       mem_offset ;
      Framework                  impl       ;
      unsigned                   gpu        ;
      typename Framework::Memory memory_ptr ;
  };
  
  template<typename Framework>
  Memory<Framework>::Memory()
  {
    this->dirty_bit  = false   ;
    this->data       = nullptr ;
    this->byte_size  = 0       ;
    this->mem_offset = 0       ;
    this->memory_ptr = nullptr ;
    this->host_alloc = false   ;
  }
  
  template<typename Framework>
  Memory<Framework>::Memory( const Memory<Framework>& src )
  {
    *this = src ;
  }
  
  template<typename Framework>
  Memory<Framework>& Memory<Framework>::operator=( const Memory<Framework>& src )
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
  
  template<typename Framework>
  const void* Memory<Framework>::hostData() const
  {
    return static_cast<const void*>( this->data ) ;
  }

  template<typename Framework>
  Memory<Framework> Memory<Framework>::operator+( unsigned offset ) const
  {
    const unsigned next_offset = this->mem_offset + offset ;
    Memory<Framework> memory ;
    
    memory            = *this                                                     ;
    memory.mem_offset = next_offset >= this->size() ? this->size() :  next_offset ;

    return memory ;
  }
  
  template<typename Framework>
  Memory<Framework>::operator const typename Framework::Memory&() const
  {
    return this->memory_ptr ;
  }
  
  template<typename Framework>
  Memory<Framework>::operator typename Framework::Memory() const
  {
    return this->memory_ptr ;
  }
  
  template<typename Framework>
  Memory<Framework>::operator const typename Framework::Memory&()
  {
    return this->memory_ptr ;
  }
  
  template<typename Framework>
  Memory<Framework>::operator typename Framework::Memory()
  {
    return this->memory_ptr ;
  }

  template<typename Framework>
  void Memory<Framework>::deallocate()
  {
    if( this->data )
    {
      delete this->data ;
    }

    impl.free( this->memory_ptr, this->gpu ) ;
  }

  template<typename Framework>
  bool Memory<Framework>::initialized() const
  {
    return ( this->memory_ptr ) ;
  }

  template<typename Framework>
  template< typename ... MEMORY_FLAGS>
  void Memory<Framework>::initialize( unsigned gpu, unsigned sz, bool host_alloc, MEMORY_FLAGS... mem_flags ) 
  {
    this->byte_size  = sz  ;
    this->gpu        = gpu ;
    
    this->memory_ptr = impl.createMemory( gpu, sz, static_cast<nyx::MemoryFlags>( nyx::combine( mem_flags... ) ) ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz ] ;
    }
  }

  template<typename Framework>
  void Memory<Framework>::initialize( unsigned gpu, unsigned sz, bool host_alloc ) 
  {
    this->byte_size  = sz  ;
    this->gpu        = gpu ;
    
    this->memory_ptr = impl.createMemory( gpu, sz ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz ] ;
    }
  }

  template<typename Framework>
  template< typename ... MEMORY_FLAGS>
  void Memory<Framework>::initialize( unsigned gpu, unsigned sz, unsigned filter, bool host_alloc, MEMORY_FLAGS... mem_flags ) 
  {
    this->byte_size  = sz  ;
    this->gpu        = gpu ;
    
    this->memory_ptr = impl.createMemory( gpu, sz, ::nyx::combine( mem_flags... ), filter ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz ] ;
    }
  }

  template<typename Framework>
  void Memory<Framework>::initialize( unsigned gpu, unsigned sz, unsigned filter, bool host_alloc ) 
  {
    this->byte_size  = sz  ;
    this->gpu        = gpu ;
    
    this->memory_ptr = impl.createMemory( gpu, sz, filter ) ;
    
    if( host_alloc )
    {
      this->data = new unsigned char[ sz ] ;
    }
  }

  template<typename Framework>
  void Memory<Framework>::copy( const Memory<Framework>& src, unsigned amt_to_copy, unsigned srcoffset, unsigned dstoffset )
  {
    this->impl.copyTo( src.memory_ptr, this->memory_ptr, offset ) ;
  }
  
  template<typename Framework>
  template<typename TYPE>
  void Memory<Framework>::copySynced( const TYPE* src, unsigned amt, unsigned offset, unsigned src_offset, unsigned dstoffset )
  {
    this->copyToDevice( src, amt, offset, src_offset ) ;
    this->copyToHost  ( src, amt, offset, src_offset ) ;
  }

  template<typename Framework>
  template<typename TYPE>
  void Memory<Framework>::copyToHost( const TYPE* src, unsigned token_amt, unsigned offset, unsigned src_offset, unsigned dstoffset )
  {
    auto amt = token_amt > this->byte_size ? this->byte_size : token_amt ;
    
    memcpy( this->data, src, amt * this->element_sz ) ;
  }

  template<typename Framework>
  template<typename TYPE>
  void Memory<Framework>::copyToDevice( const TYPE* src, unsigned byte_amt, unsigned src_offset, unsigned dst_offset )
  {
    auto amt = byte_amt > this->byte_size ? this->byte_size : byte_amt ;
    this->impl.copyToDevice( static_cast<const void*>( src ), this->memory_ptr, this->gpu, amt, src_offset, dst_offset ) ;
  }

  template<typename Framework>
  unsigned Memory<Framework>::device() const
  {
    return this->gpu ;
  }
  
  template<typename Framework>
  unsigned Memory<Framework>::offset() const
  {
    return this->mem_offset ;
  }
  
  template<typename Framework>
  const typename Framework::Memory& Memory<Framework>::memory() const
  {
    return this->memory_ptr ;
  }
  
  template<typename Framework>
  void Memory<Framework>::syncToDevice() 
  {
    this->impl.copyToDevice( this->data, this->memory_ptr, this->gpu, this->byte_size ) ;
  }
  
  template<typename Framework>
  void Memory<Framework>::syncToHost() 
  {
    this->impl.copyToHost( this->memory_ptr, this->data, this->gpu, this->byte_size ) ;
  } 

  template<typename Framework>
  unsigned Memory<Framework>::size() const
  {
    return this->byte_size ;
  }
}
#endif 

