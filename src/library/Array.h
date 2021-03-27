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

namespace nyx
{
  template<typename Impl>
  class Memory ;
  
  template<typename Impl, typename Type>
  class Array ;
  
  /** Reflective enumeration for array flags.
   */
  class ArrayFlags
  {
    public:
      
      enum
      {
        TransferSrc   = 0x00000001,
        TransferDst   = 0x00000002,
        UniformBuffer = 0x00000010,
        StorageBuffer = 0x00000020,
        Index         = 0x00000040,
        Vertex        = 0x00000080,
      };
      
      /** Default constructor.
       */
      ArrayFlags() ;
      
      /** Copy constructor. Copies input into this object.
       * @param flags The object to copy into this one.
       */
      ArrayFlags( unsigned flags ) ;
      
      /** Assignment operator. Assigns this object to the input. 
       * @param flag The flag to assign this object to.
       * @return Reference to this object after assignment.
       */
      ArrayFlags& operator=( unsigned flag ) ;
      
      /** OR operator. Bitwise OR's this obect with the input.
       * @param flag The flag to bitwise OR this object with.
       * @return Reference to this object after the operation.
       */
      ArrayFlags& operator|( unsigned flag ) ;
      
      /** Static conversion operator.
       * @return The underlying value of this object.
       */
      operator unsigned() const ;
      
      /** Method to retrieve this object's enumeration.
       * @return This object's enumeration.
       */
      unsigned value() const ;
      
      /** Method to set this object's enumeration value.
       * @param value The value to set this object's' enumeration to.
       */
      void set( unsigned value ) ;
    private:
      
      /** The underlying enumeration bit.
       */
      unsigned bit ;
  };
  
  template<typename Impl, typename Type>
  class Iterator
  {
    public:
      Iterator() ;
      void seek( unsigned idx ) ;
      unsigned size() const ;
      bool initialized() const ;
    private:
      Iterator( typename Impl::DeviceAddress dev_address, unsigned size, unsigned element_size ) ;
      
      template<typename Impl2, typename Type2>
      friend class nyx::Array ;

      typename Impl::DeviceAddress device_address ;
      unsigned                     count          ;
      unsigned                     element_size   ;
      unsigned                     position       ;
  };

  template<typename Impl, class Type>
  class Array
  {
    public:
      
      /** Default constructor.
       */
      inline Array() ;
      
      /** Copy constructor.
       */
      inline Array( const Array<Impl, Type>& array ) ;
      
      /** Deconstructor. Releases allocated data.
       */
      inline ~Array() = default ;
      
      /** Equals Operator for copying the input array
       * @return Reference to this object after assignment.
       */
      inline Array& operator=( const Array<Impl, Type>& array ) ;
      
      /** Method to create an iterator for this object at the input location.
       * @param idx The index to start the iterator at.
       * @return An iterator on this object.
       */
      inline Iterator<Impl, Type> iterator( unsigned idx = 0 ) ;

      /** Implicit conversion to turn an this object into an implementation-specific version.
       * @return The underlying implmentation-specific container of this object.
       */
      inline operator const typename Impl::Buffer&() const ;
      
      /** Implicit conversion to turn an this object into an implementation-specific version.
       * @return The underlying implmentation-specific container of this object.
       */
      inline operator typename Impl::Buffer&() ;
      
      /** Method to retrieve the reference to the data at the specified index on the host.
       * @param index The index of data to retrieve.
       * @return The reference to this host-data at the specified index.
       */
      inline const Type& operator[]( unsigned index ) ;
      
      /** Method to return the element size of this object.
       * @return The size of each element of this object.
       */
      inline unsigned elementSize() const ;
      
      /** Method to synchronize this object's host data to the device.
       */
      inline void syncToDevice() ;
      
      /** Method to synchronize this object's device data to the host.
       */
      inline void syncToHost() ;
      
      /** Whether the data in this object is dirty and needs to be synchronized.
       * @return Whether the data in this object is dirty.
       */
      inline bool dirty() ;
      
      /** Method to retrieve the number of elements in this array.
       * @return The number of elements in this array.
       */
      inline unsigned size() const ;
      
      /** Method to retrieve the amount of bytes stored by this object.
       * @return The amount of bytes this object takes up.
       */
      inline unsigned byteSize() const ;

      /** Method to initialize this object & allocate data.
       * @param buffer The buffer to use for this object's internal buffer.
       */
      inline void initialize( const typename Impl::Buffer& buffer ) ;
      
      /** Method to check if this object is initialized or not.
       * @return Whether or not this object is initialized.
       */
      inline bool initialized() const ;

      /** Method to initialize this object & allocate data.
       * @param size The number of elements allocated to this object.
       * @param device The implementation-specific device to use for this object.
       * @param host_alloc Whether or not to allocate this object on the host as well.
       */
      inline void initialize( unsigned device, unsigned size, bool host_alloc = false ) ;
      
      /** Method to initialize this object & allocate data.
       * @param size The number of elements allocated to this object.
       * @param device The implementation-specific device to use for this object.
       * @param host_alloc Whether or not to allocate this object on the host as well.
       * @param mem_flags The implementation-specific memory flags to use for the memory creation.
       * @param buffer_flags The implementation-specific buffer flags to use for buffer creation.
       */
      template<typename ... ARRAY_FLAGS>
      inline void initialize( unsigned device, unsigned size, bool host_alloc, ARRAY_FLAGS... array_flags ) ;
      
      /** Method to initialize this object & allocate data.
       * @note See Array::initialized() for whether allocation was successful.
       * @param device The implementation-specific device to use for this object.
       * @param prealloc The preallocated memory to use, if possible, for this array creation. 
       * @param size The number of elements allocated to this object.
       * @param host_alloc Whether or not to allocate this object on the host as well.
       */
      inline bool initialize( nyx::Memory<Impl>& prealloc, unsigned size ) ;
      
      /** Method to initialize this object & allocate data.
       * @note See Array::initialized() for whether allocation was successful.
       * @param device The implementation-specific device to use for this object.
       * @param prealloc The preallocated memory to use, if possible, for this array creation. 
       * @param size The number of elements allocated to this object.
       * @param host_alloc Whether or not to allocate this object on the host as well.
       * @param mem_flags The implementation-specific memory flags to use for the memory creation.
       * @param buffer_flags The implementation-specific buffer flags to use for buffer creation.
       */
      template<typename ... ARRAY_FLAGS>
      inline bool initialize( nyx::Memory<Impl>& prealloc, unsigned size, ARRAY_FLAGS... array_flags ) ;
      
      /** Method to retrieve a const reference to this object's implementation-specific buffer.
       * @return Const-reference to this object's internal buffer.
       */
      inline const typename Impl::Buffer& buffer() const ;
      
      /** Method to retrieve a const reference to this object's implementation-specific GPU device.
       * @return Const-reference to this object's internal device.
       */
      inline unsigned device() const ;
      
      /** Method to retrieve a reference to this object's internal memory.
       * @return Reference to this object's internal memory.
       */
      inline Memory<Impl>& memory() ;
      
      /** Method to reset and release any allocated data.
       */
      void reset() ;

    private:
      typename Impl::Buffer arr_buffer ; ///< The implementation specific buffer to associate with this object.
      unsigned              count      ; ///< The amount of elements in this object.
  };
  
  template<typename Impl, typename Type>
  Iterator<Impl, Type>::Iterator()
  {
    this->count          = 0 ;
    this->element_size   = 0 ;
  }
  
  template<typename Impl, typename Type>
  Iterator<Impl, Type>::Iterator( typename Impl::DeviceAddress dev_address, unsigned size, unsigned element_size )
  {
    this->device_address = dev_address  ;
    this->count          = size         ;
    this->element_size   = element_size ;
  }
  
  template<typename Impl, typename Type>
  bool Iterator<Impl, Type>::initialized() const
  {
    return this->count != 0 ;
  }

  template<typename Impl, typename Type>
  void Iterator<Impl, Type>::seek( unsigned idx )
  {
    this->position = idx < this->count ? idx : this->count - 1 ;
  }
  
  template<typename Impl, typename Type>
  unsigned Iterator<Impl, Type>::size() const
  {
    return this->count ;
  }

  template<typename Impl, class Type>
  Array<Impl, Type>::Array()
  {
    this->count = 0 ;
  }
  
  template<typename Impl, class Type>
  Array<Impl, Type>::Array( const Array<Impl, Type>& array )
  {
    *this = array ;
  }
  
  template<typename Impl, class Type>
  Array<Impl, Type>& Array<Impl, Type>::operator=( const Array<Impl, Type>& array )
  {
    this->arr_buffer = array.arr_buffer ;
    this->count      = array.count      ;
  }
  
  template<typename Impl, class Type>
  const Type& Array<Impl, Type>::operator []( unsigned index )
  {
    static Type dummy ;
    if( index * sizeof( Type ) < this->arr_buffer.size() )
    {
      return *( reinterpret_cast<const Type*>( this->arr_buffer.host() ) + index ) ;
    }
    
    return dummy ;
  }
  
  template<typename Impl, class Type>
  Array<Impl, Type>::operator const typename Impl::Buffer&() const
  {
    return this->arr_buffer ;
  }
  
  template<typename Impl, class Type>
  Array<Impl, Type>::operator typename Impl::Buffer&()
  {
    return this->arr_buffer ;
  }

  template<typename Impl, class Type>
  nyx::Iterator<Impl, Type> Array<Impl, Type>::iterator( unsigned idx )
  {
    nyx::Iterator< Impl, Type > iter( this->arr_buffer.address(), this->count, sizeof( Type ) ) ;
   
    iter.seek( idx ) ;
    return iter ;
  }
  
  template<typename Impl, class Type>
  unsigned Array<Impl, Type>::elementSize() const
  {
    return sizeof( Type ) ;
  }

  template<typename Impl, class Type>
  void Array<Impl, Type>::syncToDevice()
  {
    this->arr_buffer.syncToDevice() ;
  }
  
  template<typename Impl, class Type>
  void Array<Impl, Type>::syncToHost()
  {
    this->arr_buffer.syncToHost() ;
  }

  template<typename Impl, class Type>
  bool Array<Impl, Type>::dirty()
  {
    return this->arr_buffer.dirty() ;
  }
  
  template<typename Impl, class Type>
  unsigned Array<Impl, Type>::size() const
  {
    return this->count ;
  }
  
  template<typename Impl, class Type>
  void Array<Impl, Type>::reset()
  {
    this->arr_buffer.reset() ;
  }
  
  template<typename Impl, class Type>
  unsigned Array<Impl, Type>::byteSize() const
  {
    return this->arr_buffer.size() ;
  }
  
  template<typename Impl, class Type>
  void Array<Impl, Type>::initialize( const typename Impl::Buffer& buffer )
  {
    this->arr_buffer = buffer ;
  }

  template<typename Impl, class Type>
  void Array<Impl, Type>::initialize( unsigned device, unsigned size, bool host_alloc )
  {
    this->count = size ;
    this->arr_buffer.initialize( device, size * sizeof( Type ), host_alloc ) ;
  }
  
  template<typename Impl, class Type>
  template<typename ... ARRAY_FLAGS>
  void Array<Impl, Type>::initialize( unsigned device, unsigned size, bool host_alloc, ARRAY_FLAGS... array_flags )
  {
    this->count = size ;
    this->arr_buffer.initialize( device, size * sizeof( Type ), host_alloc, array_flags... ) ;
  }
  
  template<typename Impl, class Type>
  bool Array<Impl, Type>::initialize( nyx::Memory<Impl>& prealloc, unsigned size )
  {
    this->count = size ;
    return this->arr_buffer.initialize( prealloc, size * sizeof( Type ) ) ;
  }
  
  template<typename Impl, class Type>
  template<typename ... ARRAY_FLAGS>
  bool Array<Impl, Type>::initialize( nyx::Memory<Impl>& prealloc, unsigned size, ARRAY_FLAGS... array_flags )
  {
    this->count = size ;
    return this->arr_buffer.initialize( prealloc, array_flags..., size * sizeof( Type ) ) ;
  }

  template<typename Impl, class Type>
  bool Array<Impl, Type>::initialized() const 
  {
    return this->arr_buffer.initialized() ;
  }


  template<typename Impl, class Type>
  const typename Impl::Buffer& Array<Impl, Type>::buffer() const
  {
    return this->arr_buffer ;
  }
  
  template<typename Impl, class Type>
  unsigned Array<Impl, Type>::device() const
  {
    return this->arr_buffer.device() ;
  }
}
