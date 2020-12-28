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
 * File:   Test.cpp
 * Author: jhendl
 *
 * Created on December 23, 2020, 10:18 PM
 */

#include "Cuda.h"
#include "../Array.h"
#include "../Memory.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>

using Impl = ::kgl::cg::Cuda ;
const unsigned device = 0 ;

bool testMemoryHostGPUCopy()
{
  kgl::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  memory.initialize( sizeof( unsigned ) * 200, device, true ) ;
  host_mem.resize( 200 ) ;
  std::fill( host_mem.begin(), host_mem.end(), 2503 ) ;
  
  memory.copyToDevice( host_mem.data(), sizeof( unsigned ) * 200 ) ;
  memory.syncToHost() ;
  
  for( unsigned i = 0; i < 200; i++ )
  {
    auto array= static_cast<const unsigned*>( memory.hostData() ) ;
    if( array[ i ] != 2503 )
    {
      memory.deallocate() ;
      return false ;
    }
  }
  
  memory.deallocate() ;
  return true ;
}

bool simpleArrayTest()
{
  kgl::CudaArray<float> array ;
  
  array.initialize( device, 500, true ) ;
  
  return true ;
}

int main()
{
  std::cout << "Testing Memory Host-GPU copy...\n"  ;
  assert( testMemoryHostGPUCopy() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Simple Array Test... \n"  ;
  assert( simpleArrayTest() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "All Tests Passed!" << std::endl ;
  return 0 ;
}