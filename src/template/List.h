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

/* 
 * File:   List.h
 * Author: Jordan Hendl
 *
 * Created on January 18, 2021, 3:26 AM
 */

#ifndef KGL_LIST_H
#define KGL_LIST_H

namespace kgl
{
  /** Template class for managing a list of items.
   * @tparma Type The type of object to contain in this list.
   */
  template<class Type>
  class List
  {
    public:
      
      /** Default constructor.
       */
      List() ;
      
      /** Default deconstructor.
       */
      ~List() ;
      
      /** Copy constructor. Copies the input object into this one.
       * @param list The list to copy.
       */
      List( const List<Type>& list ) ;
      
      /** Method to initialize all objects in this list.
       * @param count The amount of object to input into this object.
       * @param params The parameters to use for initializing the object of this list.
       */
      template< typename ... Params>
      void initialize( unsigned count, Params... params ) ;
      
      /** Assignment operator. Assigns this object to the input.
       * @param list The list to assign this object to.
       * @return Reference to this object after assignment.
       */
      List<Type>& operator=( const List<Type>& list ) ;
      
      /** Bracket operator to retrieve an element of this object at the specified index.
       * @param index The index of element to retrieve
       * @return Reference to the specified element.
       */
      Type& operator[]( unsigned index ) ;
      
      /** Bracket operator to retrieve an element of this object at the specified index.
       * @param index The index of element to retrieve
       * @return Reference to the specified element.
       */
      const Type& operator[]( unsigned index ) const ;
      
      /** Bracket operator to retrieve an element of this object at the specified index.
       * @param index The index of element to retrieve
       * @return Reference to the specified element.
       */
      Type& operator()( unsigned index ) ;
      
      /** Bracket operator to retrieve an element of this object at the specified index.
       * @param index The index of element to retrieve
       * @return Reference to the specified element.
       */
      const Type& operator()( unsigned index ) const ;
      
      /** Conversion operator to convert this object to the element at it's current position.
       * @return Reference to this object's current element.
       */
      operator const Type&() const ;
      
      /** Conversion operator to convert this object to the element at it's current position.
       * @return Reference to this object's current element.
       */
      operator Type&() ;
      
      /** Method to retrieve the current element of this object.
       * @return The current element of this object.
       */
      const Type& current() const ;
      
      /** Method to retrieve the current element of this object.
       * @return The current element of this object.
       */
      Type& current() ;
      
      /** Method to advance this object's current position in the list.
       */
      void advance() ;
      
      /** Method to reset this object and deallocate all data.
       */
      void reset() ;
    private:
      
      /** The pointer containing this object's data.
       */
      Type* ptr ;
      
      /** The amount of elements in this object.
       */
      unsigned amt ;
      
      /** The position among this object's elements this object is currently at.
       */
      unsigned current_pos ;
  };
  
  template<class Type>
  List<Type>::List()
  {
    this->ptr         = nullptr ;
    this->amt         = 0       ;
    this->current_pos = 0       ;
  }
  
  template<class Type>
  List<Type>::~List()
  {
    this->reset() ;
  }
  
  template<class Type>
  List<Type>::List( const List<Type>& list )
  {
    this->reset() ;
    
    this->amt         = list.amt         ;
    this->current_pos = list.current_pos ;

    this->ptr = new Type[ list.amt ] ;
    
    for( unsigned index = 0; index < list.amt; index++ )
    {
      this->ptr[ index ] = list.ptr[ index ] ;
    }
  }
  
  template<class Type>
  template< typename ... Params>
  void List<Type>::initialize( unsigned count, Params... params )
  {
    this->reset() ;
    this->ptr = new Type[ count ] ;
    this->amt = count             ;
    
    for( unsigned index = 0; index < count; index++ )
    {
      this->ptr[ index ].initialize( params... ) ;
    }
  }
  
  template<class Type>
  List<Type>& List<Type>::operator=( const List<Type>& list )
  {
    this->reset() ;
    
    this->amt         = list.amt         ;
    this->current_pos = list.current_pos ;

    this->ptr = new Type[ list.amt ] ;
    
    for( unsigned index = 0; index < list.amt; index++ )
    {
      this->ptr[ index ] = list.ptr[ index ] ;
    }
  }
  
  template<class Type>
  Type& List<Type>::operator[]( unsigned index )
  {
    static const Type dummy ;
    if( index < this->amt ) return this->ptr[ index ] ;
    
    return dummy ;
  }
  
  template<class Type>
  const Type& List<Type>::operator[]( unsigned index ) const
  {
    static const Type dummy ;
    if( index < this->amt ) return this->ptr[ index ] ;
    
    return dummy ;
  }
  
  template<class Type>
  Type& List<Type>::operator()( unsigned index )
  {
    static const Type dummy ;
    if( index < this->amt ) return this->ptr[ index ] ;
    
    return dummy ;
  }
  
  template<class Type>
  const Type& List<Type>::operator()( unsigned index ) const
  {
    static const Type dummy ;
    if( index < this->amt ) return this->ptr[ index ] ;
    
    return dummy ;
  }
  
  template<class Type>
  List<Type>::operator const Type&() const
  {
    return this->current() ;
  }
  
  template<class Type>
  List<Type>::operator Type&()
  {
    return this->current() ;
  }
  
  template<class Type>
  const Type& List<Type>::current() const
  {
    return this->ptr[ this->current_pos ] ;
  }
  
  template<class Type>
  Type& List<Type>::current()
  {
    return this->ptr[ this->current_pos ] ;
  }
  
  template<class Type>
  void List<Type>::advance()
  {
    this->current_pos = ( this->current_pos + 1 ) % this->amt ;
  }
  
  template<class Type>
  void List<Type>::reset()
  {
    for( unsigned index = 0; index < this->amt; index++ ) this->ptr[ index ].reset() ;

    if( this->ptr )
    {
      delete[] this->ptr ;
      this->ptr = nullptr ;
    }

    this->amt         = 0 ;
    this->current_pos = 0 ;
  }
}

#endif /* LIST_H */

