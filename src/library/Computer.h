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
 * File:   Computer.h
 * Author: jhendl
 *
 * Created on February 6, 2021, 9:41 PM
 */
#pragma once

#include "Array.h"

namespace nyx
{
  template<typename Framework>
  class Computer
  {
    public:
      Computer() ;
      void initialize( const char* nyx_file_path ) ;
      void initialize( const unsigned char* nyx_file_bytes ) ;
      void dispatch() ;
      operator const typename Framework::Computer&() const ;
      template<typename Type>
      void bind( const char* name, const nyx::Array<Framework, Type>& array ) ;
      template<typename Type>
      void push( const Type& value ) ;

      void reset() ;
    private:
      typename Framework::Computer impl ;
  };
}

