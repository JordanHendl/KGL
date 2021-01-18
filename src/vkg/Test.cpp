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
#include "Queue.h"
#include "CommandBuffer.h"
#include "Synchronization.h"
#include "KgShader.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "Swapchain.h"
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
static kgl::vkg::Device     device         ;
static kgl::vkg::Queue      graphics_queue ;
static kgl::vkg::Swapchain  swapchain      ;
static kgl::Window<Impl>    window         ;
static karma::test::Manager manager        ;
static std::vector<unsigned> test_array ;


const unsigned test_vert[] = 
{
	0x07230203,0x00010000,0x00080008,0x00000020,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0008000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000c,0x00000014,
	0x00030003,0x00000002,0x000001c2,0x00090004,0x415f4c47,0x735f4252,0x72617065,0x5f657461,
	0x64616873,0x6f5f7265,0x63656a62,0x00007374,0x00040005,0x00000004,0x6e69616d,0x00000000,
	0x00040005,0x00000009,0x726f6f63,0x00007364,0x00040005,0x0000000c,0x74726576,0x00007865,
	0x00060005,0x00000012,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000012,
	0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x00000012,0x00000001,0x505f6c67,
	0x746e696f,0x657a6953,0x00000000,0x00070006,0x00000012,0x00000002,0x435f6c67,0x4470696c,
	0x61747369,0x0065636e,0x00070006,0x00000012,0x00000003,0x435f6c67,0x446c6c75,0x61747369,
	0x0065636e,0x00030005,0x00000014,0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,
	0x00040047,0x0000000c,0x0000001e,0x00000000,0x00050048,0x00000012,0x00000000,0x0000000b,
	0x00000000,0x00050048,0x00000012,0x00000001,0x0000000b,0x00000001,0x00050048,0x00000012,
	0x00000002,0x0000000b,0x00000003,0x00050048,0x00000012,0x00000003,0x0000000b,0x00000004,
	0x00030047,0x00000012,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
	0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000002,0x00040020,
	0x00000008,0x00000003,0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,
	0x0000000a,0x00000006,0x00000004,0x00040020,0x0000000b,0x00000001,0x0000000a,0x0004003b,
	0x0000000b,0x0000000c,0x00000001,0x00040015,0x0000000f,0x00000020,0x00000000,0x0004002b,
	0x0000000f,0x00000010,0x00000001,0x0004001c,0x00000011,0x00000006,0x00000010,0x0006001e,
	0x00000012,0x0000000a,0x00000006,0x00000011,0x00000011,0x00040020,0x00000013,0x00000003,
	0x00000012,0x0004003b,0x00000013,0x00000014,0x00000003,0x00040015,0x00000015,0x00000020,
	0x00000001,0x0004002b,0x00000015,0x00000016,0x00000000,0x0004002b,0x00000006,0x00000019,
	0x00000000,0x0004002b,0x00000006,0x0000001a,0x3f800000,0x00040020,0x0000001e,0x00000003,
	0x0000000a,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,
	0x0004003d,0x0000000a,0x0000000d,0x0000000c,0x0007004f,0x00000007,0x0000000e,0x0000000d,
	0x0000000d,0x00000002,0x00000003,0x0003003e,0x00000009,0x0000000e,0x0004003d,0x0000000a,
	0x00000017,0x0000000c,0x0007004f,0x00000007,0x00000018,0x00000017,0x00000017,0x00000000,
	0x00000001,0x00050051,0x00000006,0x0000001b,0x00000018,0x00000000,0x00050051,0x00000006,
	0x0000001c,0x00000018,0x00000001,0x00070050,0x0000000a,0x0000001d,0x0000001b,0x0000001c,
	0x00000019,0x0000001a,0x00050041,0x0000001e,0x0000001f,0x00000014,0x00000016,0x0003003e,
	0x0000001f,0x0000001d,0x000100fd,0x00010038
};

const unsigned test_frag[] = 
{
	0x07230203,0x00010000,0x00080008,0x00000021,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000017,0x00000020,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00090004,0x415f4c47,0x735f4252,
	0x72617065,0x5f657461,0x64616873,0x6f5f7265,0x63656a62,0x00007374,0x00040005,0x00000004,
	0x6e69616d,0x00000000,0x00050005,0x00000007,0x6f6c6f43,0x666e4972,0x0000006f,0x00070006,
	0x00000007,0x00000000,0x65756c62,0x5f726f5f,0x65657267,0x0000006e,0x00040005,0x00000009,
	0x6f666e69,0x00000000,0x00050005,0x00000017,0x4374756f,0x726f6c6f,0x00000000,0x00040005,
	0x00000020,0x726f6f63,0x00007364,0x00050048,0x00000007,0x00000000,0x00000023,0x00000000,
	0x00030047,0x00000007,0x00000002,0x00040047,0x00000009,0x00000022,0x00000000,0x00040047,
	0x00000009,0x00000021,0x00000000,0x00040047,0x00000017,0x0000001e,0x00000000,0x00040047,
	0x00000020,0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
	0x00040015,0x00000006,0x00000020,0x00000000,0x0003001e,0x00000007,0x00000006,0x00040020,
	0x00000008,0x00000002,0x00000007,0x0004003b,0x00000008,0x00000009,0x00000002,0x00040015,
	0x0000000a,0x00000020,0x00000001,0x0004002b,0x0000000a,0x0000000b,0x00000000,0x00040020,
	0x0000000c,0x00000002,0x00000006,0x00020014,0x0000000f,0x0004002b,0x00000006,0x00000010,
	0x00000000,0x00030016,0x00000014,0x00000020,0x00040017,0x00000015,0x00000014,0x00000004,
	0x00040020,0x00000016,0x00000003,0x00000015,0x0004003b,0x00000016,0x00000017,0x00000003,
	0x0004002b,0x00000014,0x00000018,0x00000000,0x0004002b,0x00000014,0x00000019,0x3f800000,
	0x0007002c,0x00000015,0x0000001a,0x00000018,0x00000019,0x00000018,0x00000019,0x0007002c,
	0x00000015,0x0000001c,0x00000018,0x00000018,0x00000019,0x00000019,0x0007002c,0x00000015,
	0x0000001d,0x00000019,0x00000018,0x00000018,0x00000019,0x00040017,0x0000001e,0x00000014,
	0x00000002,0x00040020,0x0000001f,0x00000001,0x0000001e,0x0004003b,0x0000001f,0x00000020,
	0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,
	0x00050041,0x0000000c,0x0000000d,0x00000009,0x0000000b,0x0004003d,0x00000006,0x0000000e,
	0x0000000d,0x000500ab,0x0000000f,0x00000011,0x0000000e,0x00000010,0x000300f7,0x00000013,
	0x00000000,0x000400fa,0x00000011,0x00000012,0x0000001b,0x000200f8,0x00000012,0x0003003e,
	0x00000017,0x0000001a,0x000200f9,0x00000013,0x000200f8,0x0000001b,0x0003003e,0x00000017,
	0x0000001c,0x000200f9,0x00000013,0x000200f8,0x00000013,0x0003003e,0x00000017,0x0000001d,
	0x000100fd,0x00010038
};

bool testMemoryHostGPUCopy()
{
  kgl::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  memory.initialize( device, sizeof( unsigned ) * 200, true, ::vk::MemoryPropertyFlagBits::eHostVisible | ::vk::MemoryPropertyFlagBits::eHostCoherent ) ;
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
  
            memory.initialize( device, sizeof( unsigned ) * 1000 ) ;
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
  
            memory_one.initialize( device, sizeof( unsigned ) * 1000 ) ;
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
  
  array.initialize( device, 500, true ) ;
  array.reset() ;
  return true ;
}

bool simpleImageTest()
{
  kgl::vkg::CharVkImage image ;
  
  if( image.initialize( device, 1280, 720 ) )
  {
    image.reset() ;
    return true ;
  }

  return false ;
}

bool arrayCopyTest()
{
  kgl::vkg::CommandBuffer     cmd      ;
  kgl::vkg::VkArray<unsigned> buffer_1 ;
  kgl::vkg::VkArray<unsigned> buffer_2 ; 
  
  test_array.resize( 500 ) ;
  std::fill( test_array.begin(), test_array.end(), 76006 ) ;

  buffer_1.initialize( device, 500, true  ) ;
  buffer_2.initialize( device, 500, false ) ;
  cmd     .initialize( graphics_queue, 1  ) ;
  if( cmd.size() != 1 ) return false ;
  
  // Record copy command.
  cmd.record() ;
  buffer_1.copyToDevice( test_array.data(), 500 ) ;
  buffer_2.copy( buffer_1, cmd.buffer( 0 ) ) ;
  cmd.stop() ;
  
  // Submit & Sync.
  graphics_queue.submit( cmd ) ;
  cmd.reset() ;
  
  return true ;
}

bool arrayCopyNonSyncedTest()
{
  kgl::vkg::CommandBuffer     cmd      ;
  kgl::vkg::VkArray<unsigned> buffer_1 ;
  kgl::vkg::VkArray<unsigned> buffer_2 ; 
  kgl::vkg::Synchronization   sync     ;
  
  test_array.resize( 500 ) ;
  std::fill( test_array.begin(), test_array.end(), 76006 ) ;

  buffer_1.initialize( device, 500, true  ) ;
  buffer_2.initialize( device, 500, false ) ;
  cmd     .initialize( graphics_queue, 1  ) ;
  sync    .initialize( device             ) ;
  if( cmd.size() != 1 ) return false ;
  
  // Record copy command.
  cmd.record() ;
  buffer_1.copyToDevice( test_array.data(), 500 ) ;
  buffer_2.copy( buffer_1, cmd.buffer( 0 ) ) ;
  cmd.stop() ;
  
  // Submit & Sync.
  graphics_queue.submit( cmd, sync ) ;
  return true ;
}

bool shaderTest()
{
  kgl::vkg::KgShader        shader   ;
  kgl::vkg::RenderPass      pass     ;
  kgl::vkg::Pipeline        pipeline ;
  kgl::vkg::CommandBuffer   buffer   ;
  kgl::vkg::Synchronization sync     ;
  
  // Add shaders.
  shader.addAttribute   ( 0, 0, kgl::vkg::KgShader::Format::vec4, 0                                    ) ;
  shader.addInputBinding( 0, sizeof( float ) * 3, vk::VertexInputRate::eVertex                         ) ;
  shader.addDescriptor  ( 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment ) ;
  shader.addShaderModule( vk::ShaderStageFlagBits::eVertex  , test_vert, sizeof( test_vert )           ) ;
  shader.addShaderModule( vk::ShaderStageFlagBits::eFragment, test_frag, sizeof( test_frag )           ) ;
  
  // Initialize vulkan objects.
  pass.setFinalLayout( vk::ImageLayout::ePresentSrcKHR ) ;
  pass    .initialize( swapchain                              ) ;
  shader  .initialize( device                                 ) ;
  pipeline.initialize( pass, shader                           ) ;
  buffer  .initialize( graphics_queue, 20                     ) ;
  sync    .initialize( device, 0                              ) ;

  for( unsigned index = 0; index < 20; index++ )
  {
    sync.waitOn( swapchain.acquire() ) ;
  
    buffer.record( pass, index ) ;
    buffer.stop( index ) ;
    graphics_queue.submit( buffer.buffer( index ), sync ) ;
  
    swapchain.submit( sync ) ;
    sync.clear() ;
  }
  
  device.wait() ;
  pipeline.reset() ;
  shader  .reset() ;
  pass    .reset() ;
  
  return true ;
}

int main()
{
  // Initialize Instance.
  instance.setApplicationName( "KGL-VKG Test App"                        ) ;
  instance.addExtension      ( Impl::platformSurfaceInstanceExtensions() ) ;
  instance.addExtension      ( "VK_KHR_surface"                          ) ;
  instance.addValidationLayer( "VK_LAYER_KHRONOS_validation"             ) ;
  instance.addValidationLayer( "VK_LAYER_LUNARG_standard_validation"     ) ;
  device  .addValidationLayer( "VK_LAYER_KHRONOS_validation"             ) ;
  device  .addValidationLayer( "VK_LAYER_LUNARG_standard_validation"     ) ;
  device  .addExtension      ( "VK_KHR_swapchain"                        ) ;
  
  instance.initialize() ;
  window.initialize( "Test", 1240, 1024 ) ;
  device.initialize( instance.device( 0 ), window.context() ) ;

  graphics_queue = device.graphicsQueue() ;
  
  swapchain.initialize( graphics_queue, window.context() ) ; 

  manager.add( "1) Array Test"                           , &simpleArrayTest                ) ;
  manager.add( "2) Array Copy & Synced Submit Test"      , &arrayCopyTest                  ) ;
  manager.add( "3) Array Copy & Non-Synced Submit Test"  , &arrayCopyNonSyncedTest         ) ;
  manager.add( "4) Buffer Creation & Allocation"         , &testBufferSingleAllocation     ) ;
  manager.add( "5) Preallocated Buffer Creation"         , &testBufferPreallocatedSingle   ) ;
  manager.add( "6) Multiple Preallocated Buffer Creation", &testBufferPreallocatedMultiple ) ;
  manager.add( "7) Memory Host-GPU Copy"                 , &testMemoryHostGPUCopy          ) ;
  manager.add( "8) Image Test"                           , &simpleImageTest                ) ;
  manager.add( "8) Pipeline Test"                        , &shaderTest                     ) ;
  
  std::cout << "\nTesting VKG Library" << std::endl ;
  
  return manager.test( karma::test::Output::Verbose ) ;
}
