/*
 * The MIT License
 *
 * Copyright 2020 Jordan Hendl.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   Test.cpp
 * Author: jhendl
 *
 * Created on December 23, 2020, 10:18 PM
 */

#include "vkg/Vulkan.h"
#include "vkg/Device.h"
#include "vkg/Instance.h"
#include "vkg/Buffer.h"
#include "Memory.h"
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
  
  memory.initialize<unsigned>( 200, device, true, ::vk::MemoryPropertyFlagBits::eHostVisible | ::vk::MemoryPropertyFlagBits::eHostCoherent ) ;
  host_mem.resize( 200 ) ;
  std::fill( host_mem.begin(), host_mem.end(), 2503 ) ;
  
  memory.copyToDevice( host_mem.data(), 200 ) ;
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
  
            memory.initialize<unsigned>( 500, device         ) ;
  binded += buffer.initialize          ( memory, buffer_size ) ;
  
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
  
            memory_one.initialize<unsigned>( 800, device             ) ;
  binded += buffer_one.initialize          ( memory_one, buffer_size ) ;
  
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

int main()
{
  instance.setApplicationName( "KGL-VKG Test App" ) ;
  
  instance.initialize() ;
  device.initialize( instance.device( 0 ) ) ;
  
  std::cout << "Testing Memory Host-GPU copy...\n"  ;
  assert( testMemoryHostGPUCopy() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Buffer Creation & Allocation...\n"  ;
  assert( testBufferSingleAllocation() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Single Buffer Creation | Preallocated \n"  ;
  assert( testBufferPreallocatedSingle() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "Testing Multiple Buffer Creation | Preallocated \n"  ;
  assert( testBufferPreallocatedMultiple() ) ;
  std::cout << "Passed! \n\n"  ;
  
  std::cout << "All Tests Passed!" << std::endl ;
  return 0 ;
}