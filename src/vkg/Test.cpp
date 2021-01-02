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
#include <library/Array.h>
#include <library/Memory.h>
#include <library/Image.h>
#include <library/Window.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <KT/Manager.h>

using Impl = ::kgl::vkg::Vulkan ;

static kgl::vkg::Instance   instance       ;
static kgl::vkg::Device     compute_device ;
static kgl::vkg::Device     surface_device ;
static kgl::Window<Impl>    window         ;
static karma::test::Manager manager        ;

bool testMemoryHostGPUCopy()
{
  kgl::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  memory.initialize( sizeof( unsigned ) * 200, compute_device, true, ::vk::MemoryPropertyFlagBits::eHostVisible | ::vk::MemoryPropertyFlagBits::eHostCoherent ) ;
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
  
  buffer.initialize( compute_device, 200, false ) ;
  
  

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
  
            memory.initialize( sizeof( unsigned ) * 1000, compute_device ) ;
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
  
            memory_one.initialize( sizeof( unsigned ) * 1000, compute_device ) ;
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
  kgl::vkg::VkArray<float> array ;
  
  array.initialize( compute_device, 500, true ) ;
  array.reset() ;
  return true ;
}

bool simpleImageTest()
{
  kgl::vkg::CharVkImage image ;
  
  if( image.initialize( compute_device, 1280, 720 ) )
  {
    image.reset() ;
    return true ;
  }

  return false ;
}

int main()
{
  instance.setApplicationName( "KGL-VKG Test App" ) ;
  
  instance.initialize() ;
  window.initialize( "Test", 1024, 720 ) ;
  
  compute_device.initialize( instance.device( 0 ) ) ;
  surface_device.initialize( instance.device( 0 ), window.context() ) ;
  manager.add( "VKG Buffer Creation & Allocation"         , &testBufferSingleAllocation     ) ;
  manager.add( "VKG Preallocated Buffer Creation"         , &testBufferPreallocatedSingle   ) ;
  manager.add( "VKG Multiple Preallocated Buffer Creation", &testBufferPreallocatedMultiple ) ;
  manager.add( "Memory Host-GPU Copy"                     , &testMemoryHostGPUCopy          ) ;
  manager.add( "Array Test"                               , &simpleArrayTest                ) ;
  manager.add( "Image Test"                               , &simpleImageTest                ) ;
  
  std::cout << "\nTesting VKG Library" << std::endl ;
  
  return manager.test( karma::test::Output::Verbose ) ;
}