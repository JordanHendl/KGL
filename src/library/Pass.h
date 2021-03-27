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
 * File:   Pass.h
 * Author: jhendl
 *
 * Created on March 21, 2021, 3:43 PM
 */

#pragma once

#include "Image.h"
#include "Array.h"

namespace nyx
{
  template<typename Framework>
  class Memory ;
  
  template<typename Framework, typename Type>
  class Array ;
  
  template<typename Framework>
  class Chain ;
  
  template<typename Framework>
  class Pass
  {
    public:
      /** Default constructor.
       */
      Pass() = default ;
      
      /** Default deconstructor.
       */
      ~Pass() = default ;
      
      /** Method to convert this object to the underlying implementation-specific version.
       * @return The implementation-specific version of this object.
       */
      operator const typename Framework::Pass&() const ;
      
      /** Method to create a chain to allow GPU operations to happen as a part of this render pass.
       * @return The chain to allow GPU operations to be chained to this render pass.
       */
      nyx::Chain<Framework>& makeChain() ;

      /** Method to initialize this object.
       * @param device The device to use for all GPU operations.
       * @param nyx_file_path The path to the .nyx file on the filesystem to use.
       * @param context The Framework context for this renderer to draw to.
       */
      void initialize( unsigned device ) ;
      
      /**
       * @return 
       */
      bool initialized() const ;
 
      /** Method to retrieve the device used by this renderer.
       * @return Const reference to the device used for this renderer.
       */
      unsigned device() const ;

      void reset() ;
    private:
      typename Framework::Pass impl ;
  };
 
  template<typename Framework>
  Pass<Framework>::operator const typename Framework::Pass& () const
  {
    return this->impl ;
  }
 
  template<typename Framework>
  void Pass<Framework>::initialize( unsigned device )
  {
    this->impl.initialize( device ) ;
  }
 
  template<typename Framework>
  unsigned Pass<Framework>::device() const
  {
    return this->impl.device() ;
  }
}