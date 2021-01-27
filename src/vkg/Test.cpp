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
#include <library/Array.h>
#include <library/Memory.h>
#include <library/Image.h>
#include <library/Window.h>
#include <template/List.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <Athena/Manager.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using Impl = nyx::vkg::Vulkan ;

static Impl::Instance      instance       ;
static Impl::Device        device         ;
static Impl::Queue         graphics_queue ;
static Impl::Swapchain     swapchain      ;
static nyx::Window<Impl>   window         ;
static athena::Manager     manager        ;

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

athena::Result instance_initialization_test()
{
  // Initialize Instance.
  instance.setApplicationName( "NYX-VKG Test App"                        ) ;
  instance.addExtension      ( Impl::platformSurfaceInstanceExtensions() ) ;
  instance.addExtension      ( "VK_KHR_surface"                          ) ;
  instance.addValidationLayer( "VK_LAYER_KHRONOS_validation"             ) ;
  instance.addValidationLayer( "VK_LAYER_LUNARG_standard_validation"     ) ;
  
  instance.initialize() ;
  
  if( instance.initialized() ) return true ;
  return false ;
}

athena::Result window_creation_test()
{
  window.initialize( "Test", 1240, 1024 ) ;
  
  if( !window.initialized() ) return athena::Result::Fail ;
  return true ;
}

athena::Result device_creation_test()
{
  if( instance.numDevices() == 0 ) return athena::Result::Skip ;
  
  device  .addValidationLayer( "VK_LAYER_KHRONOS_validation"         ) ;
  device  .addValidationLayer( "VK_LAYER_LUNARG_standard_validation" ) ;
  device  .addExtension      ( "VK_KHR_swapchain"                    ) ;
  device.initialize( instance.device( 0 ), window.context() ) ;
  
  if( !device.initialized() ) return athena::Result::Fail ;
  
  return athena::Result::Pass ;
}

athena::Result graphics_queue_get_test()
{
  graphics_queue = device.graphicsQueue() ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  if( !graphics_queue.valid() || !graphics_queue.graphics() ) return false ;
  
  return true ;
}

athena::Result swapchain_creation_test()
{
  swapchain.initialize( graphics_queue, window.context() ) ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  if( !swapchain.initialized() ) return false ;
  return true ;
}

athena::Result array_initialize_test()
{
  Impl::Array<float> array ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  array.initialize( device, 500, true ) ;
  array.reset() ;
  return true ;
}

athena::Result test_memory_initialize()
{
  nyx::Memory<Impl> memory ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
  
  if( !memory.initialized() ) return false ;
  return true ;
}

athena::Result test_memory_size()
{
  nyx::Memory<Impl> memory ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
  
  if( memory.size() != ( sizeof( unsigned ) * 200 ) ) return false ;
  return true ;
}

athena::Result test_memory_offset()
{
  nyx::Memory<Impl> memory1 ;
  nyx::Memory<Impl> memory2 ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  memory1.initialize( device, sizeof( unsigned ) * 200, true ) ;
  memory2 = memory1 + ( sizeof( unsigned ) * 100 ) ;
  
  if( !memory2.initialized() || ( memory2.size() - memory2.offset() ) != ( sizeof( unsigned ) * 100 ) ) return false ;
  return true ;
}

athena::Result test_memory_device()
{
  nyx::Memory<Impl> memory; 
  
  if( !device.initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
  
  if( !memory.device().initialized() ) return false ;
  return true ;
}

athena::Result test_memory_sync_to_host_copy()
{
  nyx::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true, nyx::MemoryFlags::HostVisible | nyx::MemoryFlags::HostCoherent ) ;
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

athena::Result array_size_test()
{
  Impl::Array<float> array ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  array.initialize( device, 500, true ) ;
  if( array.size() != 500 ) return athena::Result::Fail ;
  
  return true ;
}
athena::Result array_host_copy_test()
{
  Impl::CommandRecord   cmd      ;
  Impl::Array<unsigned> buffer_1 ;
  Impl::Array<unsigned> buffer_2 ; 
  
  test_array.resize( 500 ) ;
  std::fill( test_array.begin(), test_array.end(), 76006 ) ;

  if( !device.initialized() ) return athena::Result::Skip ;
  buffer_1.initialize( device, 500, true  ) ;
  buffer_2.initialize( device, 500, false ) ;
  cmd     .initialize( graphics_queue, 1  ) ;
  if( cmd.size() != 1 ) return false ;
  
  // Record copy command.
  cmd.record() ;
  buffer_1.copyToDevice( test_array.data(), 500 ) ;
  buffer_2.copy( buffer_1, cmd ) ;
  cmd.stop() ;
  
  // Submit & Sync.
  graphics_queue.submit( cmd ) ;
  cmd.reset() ;
  
  return true ;
}

athena::Result test_array_copy_non_wait()
{
  Impl::CommandRecord   cmd      ;
  Impl::Array<unsigned> buffer_1 ;
  Impl::Array<unsigned> buffer_2 ; 
  Impl::Synchronization sync     ;
  
  test_array.resize( 500 ) ;
  std::fill( test_array.begin(), test_array.end(), 76006 ) ;
  if( !device.initialized() ) return athena::Result::Skip ;
  buffer_1.initialize( device, 500, true  ) ;
  buffer_2.initialize( device, 500, false ) ;
  cmd     .initialize( graphics_queue, 1  ) ;
  sync    .initialize( device             ) ;
  if( cmd.size() != 1 ) return false ;
  
  // Record copy command.
  cmd.record() ;
  buffer_1.copyToDevice( test_array.data(), 500 ) ;
  buffer_2.copy( buffer_1, cmd ) ;
  cmd.stop() ;
  
  // Submit & Sync.
  graphics_queue.submit( cmd, sync ) ;
  return true ;
}

athena::Result test_buffer_initialize()
{
  Impl::Buffer buffer ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  buffer.initialize( device, 200, false ) ;

  if( buffer.buffer() && buffer.memory().memory() ) 
  {
    buffer.reset() ;
    return true ;
  }

  buffer.reset() ;
  return false ;
}

athena::Result test_buffer_preallocated_initialize()
{
  const unsigned buffer_size = sizeof( unsigned ) * 200 ;
  Impl::Buffer      buffer ;
  nyx::Memory<Impl> memory ;
  unsigned          binded ;
  
  binded = 0 ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
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

athena::Result test_multiple_buffer_preallocated_initialize()
{
  const unsigned buffer_size = sizeof( unsigned ) * 200 ;
  Impl::Buffer      buffer_one ;
  Impl::Buffer      buffer_two ;
  nyx::Memory<Impl> memory_one ;
  nyx::Memory<Impl> memory_two ;
  unsigned          binded     ;
  
  binded = 0 ;
  if( !device.initialized() ) return athena::Result::Skip ;
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

athena::Result test_buffer_size()
{
  Impl::Buffer buffer ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  buffer.initialize( device, 500 * sizeof( unsigned ) ) ;
  
  if( buffer.size() < 500 * sizeof( unsigned ) ) return false ;
  return true ;
}

athena::Result simpleImageTest()
{
  Impl::Image<nyx::ImageFormat::RGBA8> image ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  if( image.initialize( device, 1280, 720 ) )
  {
    image.reset() ;
    return true ;
  }

  return false ;
}

athena::Result shaderTest()
{
  Impl::Shader        shader   ;
  Impl::RenderPass    pass     ;
  Impl::Pipeline      pipeline ;
  Impl::CommandRecord buffer   ;
  
  nyx::List<Impl::Synchronization> syncs ;
  
  if( !device.initialized() ) return athena::Result::Skip ;
  // Add shaders.
  shader.addAttribute   ( 0, 0, Impl::Shader::Format::vec4, 0                                          ) ;
  shader.addInputBinding( 0, sizeof( float ) * 3, vk::VertexInputRate::eVertex                         ) ;
  shader.addDescriptor  ( 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment ) ;
  shader.addShaderModule( vk::ShaderStageFlagBits::eVertex  , test_vert, sizeof( test_vert )           ) ;
  shader.addShaderModule( vk::ShaderStageFlagBits::eFragment, test_frag, sizeof( test_frag )           ) ;
  
  // Initialize vulkan objects.
  pass.setFinalLayout( vk::ImageLayout::ePresentSrcKHR ) ;
  pass    .initialize( swapchain                       ) ;
  shader  .initialize( device                          ) ;
  pipeline.initialize( pass, shader                    ) ;
  buffer  .initialize( graphics_queue, 20              ) ;
  syncs   .initialize( swapchain.count(), device, 0    ) ;

  for( unsigned i = 0; i < 50; i++ )
  {
    syncs.current().waitOn( swapchain.acquire() ) ;
  
    buffer.record( pass, swapchain.current() ) ;
    buffer.stop( swapchain.current() ) ;
    graphics_queue.submit( buffer.buffer( swapchain.current() ), syncs ) ;
  
    swapchain.submit( syncs ) ;
    syncs.current().clear() ;
    syncs.current().waitOnFences() ;
    syncs.advance() ;
  }
  
  device  .wait()  ;
  syncs   .reset() ;
  pipeline.reset() ;
  shader  .reset() ;
  pass    .reset() ;
  
  return true ;
}

athena::Result imageLoadTest()
{
  Impl::Array<unsigned char> staging ; 
  nyx::RGBAImage<Impl>       image   ;
  Impl::CommandRecord        cmd     ;
  int width  ;
  int height ;
  int chan   ;
  
  auto bytes = stbi_load( "test_image.jpeg", &width, &height, &chan, STBI_rgb_alpha ) ;
  if( !device.initialized() ) return athena::Result::Skip ;
  cmd.initialize      ( graphics_queue, 1 ) ;
  staging.initialize  ( device, static_cast<unsigned>( width * height * 4 ), true, nyx::ArrayFlags::TransferSrc | nyx::ArrayFlags::TransferDst ) ;
  staging.copyToDevice( bytes, static_cast<unsigned>( width * height * 4 ) ) ;
  
  if( !image.initialize( device, static_cast<unsigned>( width ), static_cast<unsigned>( height ), 1 ) ) return false ;
  
  cmd.record() ;
  image.copy( staging.buffer(), cmd ) ;
  cmd.stop() ;
  
  graphics_queue.submit( cmd ) ;
  
  return true ;
}

int main()
{
  manager.initialize( "Nyx VULKAN Library" ) ;
  manager.add( "01) Instance Creation Test"                 , &instance_initialization_test                 ) ;
  manager.add( "02) Window Creation Test"                   , &window_creation_test                         ) ;
  manager.add( "03) Device Creation Test"                   , &device_creation_test                         ) ;
  manager.add( "04) Graphics Queue Grab Test"               , &graphics_queue_get_test                      ) ;
  manager.add( "05) Swapchain Creation Test"                , &swapchain_creation_test                      ) ;
  manager.add( "06) Memory::initialize Test"                , &test_memory_initialize                       ) ;
  manager.add( "07) Memory::size Test"                      , &test_memory_size                             ) ;
  manager.add( "08) Memory::offset Test"                    , &test_memory_offset                           ) ;
  manager.add( "09) Memory::device Test"                    , &test_memory_device                           ) ;
  manager.add( "10) Memory::syncToHost Test"                , &test_memory_sync_to_host_copy                ) ;
  manager.add( "11) Buffer::initialize Test"                , &test_buffer_initialize                       ) ;
  manager.add( "12) Buffer::preallocated init Test"         , &test_buffer_preallocated_initialize          ) ;
  manager.add( "13) Buffer::multiple preallocated init Test", &test_multiple_buffer_preallocated_initialize ) ;
  manager.add( "14) Buffer::size Test"                      , &test_buffer_size                             ) ;
  manager.add( "15) Array::initialize Test"                 , &array_initialize_test                        ) ;
  manager.add( "16) Array::size Test"                       , &array_size_test                              ) ;
  manager.add( "17) Array::copy Test"                       , &array_host_copy_test                         ) ;
  manager.add( "18) Array::copy Test ( No waiting )"        , &test_array_copy_non_wait                     ) ;
  manager.add( "19) Image Test"                             , &simpleImageTest                              ) ;
  manager.add( "20) Pipeline Test"                          , &shaderTest                                   ) ;
  manager.add( "21) Image Copy Test"                        , &imageLoadTest                                ) ;
  
  return manager.test( athena::Output::Verbose ) ;
}
