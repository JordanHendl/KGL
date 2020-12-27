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

#ifndef KGL_ARRAY_H
#define KGL_ARRAY_H

namespace kgl
{
  template<typename IMPL>
  class Memory ;
  
  template<typename IMPL, class TYPE>
  class Array
  {
    public:
      
      /** Default constructor.
       */
      Array() ;
      
      /** Copy constructor.
       */
      Array( const Array<IMPL, TYPE>& array ) ;
      
      /** Deconstructor. Releases allocated data.
       */
      ~Array() = default ;
      
      /** Equals Operator for copying the input array
       * @return Reference to this object after assignment.
       */
      Array& operator=( const Array<IMPL, TYPE>& array ) ;
      
      /** Method to retrieve the reference to the data at the specified index on the host.
       * @param index The index of data to retrieve.
       * @return The reference to this host-data at the specified index.
       */
      TYPE& operator[]( unsigned index ) ;
      
      /** Method to copy the input array into this object.
       * @param src The source buffer to copy from.
       * @param amount The amount to copy.
       * @param The source offset to start at.
       * @param The destination offset to start the copy at.
       */
      void copy( const Array<IMPL, TYPE>& src, unsigned amount = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the input array into this object.
       * @param src The source buffer to copy from.
       * @param record The record to use for recording this command.
       * @param amount The amount to copy.
       * @param The source offset to start at.
       * @param The destination offset to start the copy at.
       */
      void copy( const Array<IMPL, TYPE>& src, typename IMPL::CommandRecord& record, unsigned amount = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the date of the input host pointer to the device.
       * @param src The host-pointer to copy from.
       * @param amount The amount to copy.
       * @param srcoffset The offset of the input to start copying from.
       * @param dstoffset The offset of the device array to start writing to.
       */
      void copyToDevice( const TYPE* src, unsigned amount = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the date of the input host pointer to this object's host-data.
       * @param src The host-pointer to copy from.
       * @param amount The amount to copy.
       * @param srcoffset The offset of the input to start copying from.
       * @param dstoffset The offset of the host-pointer to start writing to.
       */
      void copyToHost( const TYPE* src, unsigned amount = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to copy the date of the input host pointer to both of this object's data members.
       * @param src The host-pointer to copy from.
       * @param amount The amount to copy.
       * @param srcoffset The offset of the input to start copying from.
       * @param dstoffset The offset of the destination pointers to start copying to.
       */
      void copySynced( const TYPE* src, unsigned amount = 0, unsigned srcoffset = 0, unsigned dstoffset = 0 ) ;
      
      /** Method to return the element size of this object.
       * @return The size of each element of this object.
       */
      unsigned elementSize() const ;
      
      /** Method to synchronize this object's host data to the device.
       */
      void syncToDevice() ;
      
      /** Method to synchronize this object's device data to the host.
       */
      void syncToHost() ;
      
      /** Whether the data in this object is dirty and needs to be synchronized.
       * @return Whether the data in this object is dirty.
       */
      bool dirty() ;
      
      /** Method to retrieve the number of elements in this array.
       * @return The number of elements in this array.
       */
      unsigned size() const ;
      
      /** Method to retrieve the amount of bytes stored by this object.
       * @return The amount of bytes this object takes up.
       */
      unsigned byteSize() const ;

      /** Method to initialize this object & allocate data.
       * @param buffer The buffer to use for this object's internal buffer.
       */
      void initialize( const typename IMPL::Buffer& buffer ) ;
      
      /** Method to initialize this object & allocate data.
       * @param size The number of elements allocated to this object.
       * @param device The implementation-specific device to use for this object.
       * @param host_alloc Whether or not to allocate this object on the host as well.
       */
      void initialize( const typename IMPL::Device& device, unsigned size, bool host_alloc = true ) ;
      
      /** Method to initialize this object & allocate data.
       * @param size The number of elements allocated to this object.
       * @param device The implementation-specific device to use for this object.
       * @param host_alloc Whether or not to allocate this object on the host as well.
       * @param mem_flags The implementation-specific memory flags to use for the memory creation.
       * @param buffer_flags The implementation-specific buffer flags to use for buffer creation.
       */
      template<typename ... BUFFER_FLAGS>
      void initialize( const typename IMPL::Device& device, unsigned size, bool host_alloc, BUFFER_FLAGS... buffer_flags ) ;
      
      /** Method to retrieve a const reference to this object's implementation-specific buffer.
       * @return Const-reference to this object's internal buffer.
       */
      const typename IMPL::Buffer& buffer() const ;
      
      /** Method to retrieve a const reference to this object's implementation-specific GPU device.
       * @return Const-reference to this object's internal device.
       */
      const typename IMPL::Device& device() const ;
      
      /** Method to retrieve a reference to this object's internal memory.
       * @return Reference to this object's internal memory.
       */
      Memory<IMPL>& memory() ;

    private:
      typename IMPL::Buffer arr_buffer ; ///< The implementation specific buffer to associate with this object.
      unsigned              count      ; ///< The amount of elements in this object.
  };
  
  template<typename IMPL, class TYPE>
  Array<IMPL, TYPE>::Array()
  {
    this->count = 0 ;
  }
  
  template<typename IMPL, class TYPE>
  Array<IMPL, TYPE>::Array( const Array<IMPL, TYPE>& array )
  {
    *this = array ;
  }
  
  template<typename IMPL, class TYPE>
  Array<IMPL, TYPE>& Array<IMPL, TYPE>::operator=( const Array<IMPL, TYPE>& array )
  {
    this->arr_buffer = array.arr_buffer ;
    this->count      = array.count      ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::copy( const Array<IMPL, TYPE>& src, unsigned amount, unsigned srcoffset, unsigned dstoffset )
  {
    this->arr_buffer.copy( src.arr_buffer, amount, srcoffset, dstoffset ) ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::copy( const Array<IMPL, TYPE>& src, typename IMPL::CommandRecord& record, unsigned amount, unsigned srcoffset, unsigned dstoffset )
  {
    this->arr_buffer.copy( src.arr_buffer, record, amount, srcoffset, dstoffset ) ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::copyToDevice( const TYPE* src, unsigned amount, unsigned srcoffset, unsigned dstoffset )
  {
    this->arr_buffer.copy( src, amount, srcoffset, dstoffset ) ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::copyToHost( const TYPE* src, unsigned amount, unsigned srcoffset, unsigned dstoffset )
  {
    this->arr_buffer.copyToHost( static_cast<const unsigned char*>( src ), amount * sizeof( TYPE ), srcoffset, dstoffset ) ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::copySynced( const TYPE* src, unsigned amount, unsigned srcoffset, unsigned dstoffset )
  {
    this->copyToDevice( src, amount, srcoffset, dstoffset ) ;
    this->copyToHost  ( src, amount, srcoffset, dstoffset ) ;
  }

  
  template<typename IMPL, class TYPE>
  unsigned Array<IMPL, TYPE>::elementSize() const
  {
    return this->arr_memory.elementSize() ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::syncToDevice()
  {
    this->arr_buffer.syncToDevice() ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::syncToHost()
  {
    this->arr_buffer.syncToHost() ;
  }

  
  template<typename IMPL, class TYPE>
  bool Array<IMPL, TYPE>::dirty()
  {
    return this->arr_memory.dirty() ;
  }

  
  template<typename IMPL, class TYPE>
  unsigned Array<IMPL, TYPE>::size() const
  {
    return this->count ;
  }

  
  template<typename IMPL, class TYPE>
  unsigned Array<IMPL, TYPE>::byteSize() const
  {
    return this->arr_memory.byteSize() ;
  }

  
  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::initialize( const typename IMPL::Buffer& buffer )
  {
    this->arr_buffer = buffer ;
  }

  template<typename IMPL, class TYPE>
  void Array<IMPL, TYPE>::initialize( const typename IMPL::Device& device, unsigned size, bool host_alloc )
  {
    this->arr_buffer.initialize( device, size, host_alloc ) ;
  }


  template<typename IMPL, class TYPE>
  template<typename ... BUFFER_FLAGS>
  void Array<IMPL, TYPE>::initialize( const typename IMPL::Device& device, unsigned size, bool host_alloc, BUFFER_FLAGS... buffer_flags )
  {
    this->arr_buffer.initialize( device, size, host_alloc, buffer_flags... ) ;
  }


  template<typename IMPL, class TYPE>
  const typename IMPL::Buffer& Array<IMPL, TYPE>::buffer() const
  {
    return this->arr_buffer ;
  }

  
  template<typename IMPL, class TYPE>
  const typename IMPL::Device& Array<IMPL, TYPE>::device() const
  {
    return this->arr_memory.device() ;
  }
}

#endif 

