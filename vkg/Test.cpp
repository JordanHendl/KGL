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

#include "Vulkan.h"
#include "Device.h"
#include "Instance.h"
#include "Buffer.h"
#include "Image.h"
#include "../Array.h"
#include "../Memory.h"
#include "../Image.h"
#include "../Window.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>

using Impl = ::kgl::vkg::Vulkan ;

static kgl::vkg::Instance instance ;
static kgl::vkg::Device   device   ;

bool testMemoryHostGPUCopy()
{
  kgl::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  memory.initialize( sizeof( unsigned ) * 200, device, true, ::vk::MemoryPropertyFlagBits::eHostVisible | ::vk::MemoryPropertyFlagBits::eHostCoherent ) ;
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

bool testBufferSingleAllocation()
{
  kgl::vkg::Buffer buffer ;
  
  buffer.initialize( device, 200, false ) ;
  
  

  if( buffer.buffer() && buffer.memory().memory() ) 
  {
    buffer.reset() ;
    return true ;
  }

  buffer.reset() ;
  return false ;
}

bool testBufferPreallocatedSingle()
{
  const unsigned buffer_size = sizeof( unsigned ) * 200 ;
  kgl::vkg::Buffer  buffer ;
  kgl::Memory<Impl> memory ;
  unsigned          binded ;
  
  binded = 0 ;
  
            memory.initialize( sizeof( unsigned ) * 1000, device ) ;
  binded += buffer.initialize( memory, buffer_size               ) ;
  
  buffer.reset()      ;
  memory.deallocate() ;

  if( binded == 1 )
  {
    return true ;
  }
  
  return false ;
}

bool testBufferPreallocatedMultiple()
{
  const unsigned buffer_size = sizeof( unsigned ) * 200 ;
  kgl::vkg::Buffer  buffer_one ;
  kgl::vkg::Buffer  buffer_two ;
  kgl::Memory<Impl> memory_one ;
  kgl::Memory<Impl> memory_two ;
  unsigned          binded     ;
  
  binded = 0 ;
  
            memory_one.initialize( sizeof( unsigned ) * 1000, device ) ;
  binded += buffer_one.initialize( memory_one, buffer_size           ) ;
  
  memory_two = memory_one + buffer_one.size() ;

  binded += buffer_two.initialize( memory_two, buffer_size ) ;
  
  buffer_one.reset() ;
  buffer_two.reset() ;
  memory_one.deallocate() ;

  if( binded == 2 )
  {
    return true ;
  }
  
  return false ;
}

bool simpleArrayTest()
{
  kgl::VkArray<float> array ;
  
  array.initialize( device, 500, true ) ;
  array.reset() ;
  return true ;
}

bool simpleImageTest()
{
  kgl::CharVkImage image ;
  
  if( image.initialize( device, 1280, 720 ) )
  {
    image.reset() ;
    return true ;
  }

  return false ;
}

bool windowSurfaceTest()
{
  kgl::Window<Impl> window ;
  
  window.initialize( "Test", 1240, 720 ) ;
  
  return true ;
}

int main()
{
  instance.setApplicationName( "KGL-VKG Test App" ) ;
  
  instance.initialize() ;
  Impl::initialize( instance ) ;
  device.initialize( instance.device( 0 ) ) ;
  
  std::cout << "Testing VKG Buffer Creation & Allocation...\n"  ;
  assert( testBufferSingleAllocation() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing VKG Single Buffer Creation | Preallocated \n"  ;
  assert( testBufferPreallocatedSingle() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing VKG Multiple Buffer Creation | Preallocated \n"  ;
  assert( testBufferPreallocatedMultiple() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Memory Host-GPU copy...\n"  ;
  assert( testMemoryHostGPUCopy() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Simple Array Test... \n"  ;
  assert( simpleArrayTest() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Simple Image Test... \n"  ;
  assert( simpleImageTest() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Vulkan Window... \n"  ;
  assert( windowSurfaceTest() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "All Tests Passed!" << std::endl ;
  return 0 ;
}