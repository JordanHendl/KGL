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

#include "Array.h"

namespace nyx
{
  ArrayFlags::ArrayFlags()
  {
    this->bit = ArrayFlags::TransferDst | ArrayFlags::TransferSrc ;
  }
  
  ArrayFlags::ArrayFlags( unsigned flags )
  {
    this->set( flags ) ;
  }

  ArrayFlags& ArrayFlags::operator=( unsigned flag )
  {
    this->set( flag ) ;
    
    return *this ;
  }
  
  ArrayFlags& ArrayFlags::operator|( unsigned flag )
  {
    this->set( this->bit | flag ) ;
    
    return *this ;
  }
  
  ArrayFlags::operator unsigned() const
  {
    return this->value() ;
  }
  
  unsigned ArrayFlags::value() const
  {
    return this->bit ;
  }
  
  void ArrayFlags::set( unsigned value )
  {
    this->bit = value ;
  }
}      

