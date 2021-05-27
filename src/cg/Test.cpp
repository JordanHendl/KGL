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
#include <library/Array.h>
#include <library/Memory.h>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <Athena/Manager.h>

using Impl = ::nyx::cg::Cuda ;
const unsigned device = 0 ;

bool test_memory_sync_to_host_copy()
{
  nyx::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
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
  nyx::CudaArray<float> array ;
  
  array.initialize( device, 500, true ) ;
  
  return true ;
}

int main()
{
  athena::Manager manager ;
  Impl::initialize() ;
  std::cout << "Testing NYX CUDA Library" << std::endl ;
  
  manager.initialize( "Nyx CUDA Library" ) ;
  manager.add( "1) Memory Host-GPU Copy", &test_memory_sync_to_host_copy ) ;
  manager.add( "2) Testing Cuda Array"  , &simpleArrayTest       ) ;
  
  return manager.test( athena::Output::Verbose ) ;
}