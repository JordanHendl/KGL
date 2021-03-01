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
#include <library/Renderer.h>
#include <nyxfile/NyxFile.h>
#include <shaders/headers/draw.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <athena/Manager.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr unsigned device = 0 ;

using Impl = nyx::vkg::Vulkan ;

static Impl::Instance      instance       ;
static Impl::Queue         graphics_queue ;
static Impl::Swapchain     swapchain      ;
static nyx::Window<Impl>   window         ;
static athena::Manager     manager        ;

static std::vector<unsigned> test_array ;

athena::Result instance_initialization_test()
{
  // Initialize Instance.
  Impl::setApplicationName  ( "NYX-VKG Test App"                        ) ;
  Impl::addInstanceExtension( Impl::platformSurfaceInstanceExtensions() ) ;
  Impl::addInstanceExtension( "VK_KHR_surface"                          ) ;
  Impl::addValidationLayer  ( "VK_LAYER_KHRONOS_validation"             ) ;
  Impl::addValidationLayer  ( "VK_LAYER_LUNARG_standard_validation"     ) ;
  Impl::addDeviceExtension  ( "VK_KHR_swapchain"                        ) ;
  
  Impl::initialize() ;
  
  if( Impl::initialized() ) return true ;
  return false ;
}

athena::Result window_creation_test()
{
  window.initialize( "Test", 1280, 1024 ) ;
  
  if( !window.initialized() ) return athena::Result::Fail ;
  
  return true ;
}

athena::Result graphics_queue_get_test()
{
  graphics_queue = Impl::presentQueue( window.context(), device ) ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  if( !graphics_queue.valid() || !graphics_queue.graphics() ) return false ;
  
  return true ;
}

athena::Result swapchain_creation_test()
{
  swapchain.initialize( graphics_queue, window.context() ) ;
  if( !Impl::initialized()  ) return athena::Result::Skip ;
  if( swapchain.count() == 0 ) return false ;
  
  swapchain.reset() ;
  return true ;
}

athena::Result test_array_initialize()
{
  Impl::Array<float>         array ;
  nyx::Iterator<Impl, float> iter  ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  array.initialize( device, 500, true ) ;
  iter = array.iterator() ;
  
  array.reset() ;
  return true ;
}

athena::Result test_memory_initialize()
{
  nyx::Memory<Impl> memory ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
  
  if( !memory.initialized() ) return false ;
  memory.deallocate() ;
  return true ;
}

athena::Result test_memory_size()
{
  nyx::Memory<Impl> memory ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
  
  if( memory.size() != ( sizeof( unsigned ) * 200 ) ) return false ;
  
  memory.deallocate() ;
  return true ;
}

athena::Result test_memory_offset()
{
  nyx::Memory<Impl> memory1 ;
  nyx::Memory<Impl> memory2 ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  memory1.initialize( device, sizeof( unsigned ) * 200, true ) ;
  memory2 = memory1 + ( sizeof( unsigned ) * 100 ) ;
  
  if( !memory2.initialized() || ( memory2.size() - memory2.offset() ) != ( sizeof( unsigned ) * 100 ) ) return false ;
  
  memory1.deallocate() ;
  return true ;
}

athena::Result test_memory_device()
{
  nyx::Memory<Impl> memory; 
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  memory.initialize( device, sizeof( unsigned ) * 200, true ) ;
  
//  if( !memory.device().initialized() ) return false ;
  memory.deallocate() ;
  return true ;
}

athena::Result test_memory_sync_to_host_copy()
{
  nyx::Memory<Impl>     memory   ;
  std::vector<unsigned> host_mem ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
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

athena::Result test_array_size()
{
  Impl::Array<float> array ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  array.initialize( device, 500, true ) ;
  if( array.size() != 500 ) return athena::Result::Fail ;

  array.reset() ;  
  return true ;
}

athena::Result test_array_host_copy()
{
  Impl::Array<unsigned> buffer_1 ;
  Impl::Array<unsigned> buffer_2 ; 
  
  test_array.resize( 500 ) ;
  std::fill( test_array.begin(), test_array.end(), 76006 ) ;

  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  buffer_1.initialize( device, 500, true ) ;
  buffer_2.initialize( device, 500, true ) ;
  buffer_1.copyToDevice( test_array.data(), 500 ) ;
  buffer_2.copy( buffer_1 ) ;
  buffer_2.syncToHost() ;
  
  for( unsigned index = 0; index < 500; index++ )
  {
    if( buffer_2[ index ] != 76006 )
    {
      std::cout << "index: " << index << " | " << buffer_2[ index ] << std::endl ;
//      return false ;
    }
  }

  buffer_1.reset() ;
  buffer_2.reset() ;
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
  if( !Impl::initialized() ) return athena::Result::Skip ;
  buffer_1.initialize( device, 500, true  ) ;
  buffer_2.initialize( device, 500, false ) ;
  cmd     .initialize( graphics_queue, 1  ) ;
  sync    .initialize( device             ) ;
  
  sync.resetFence() ;

  if( cmd.size() != 1 ) return false ;
  
  // Record copy command.
  cmd.record() ;
  buffer_1.copyToDevice( test_array.data(), 500 ) ;
  buffer_2.copy( buffer_1, graphics_queue ) ;
  cmd.stop() ;
  
  // Submit & Sync.
  graphics_queue.submit( cmd, sync ) ;
  return true ;
}

athena::Result test_array_prealloc_init()
{
  Impl::Array<unsigned> array_1 ;
  Impl::Array<unsigned> array_2 ; 
  nyx::Memory<Impl>     memory  ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  memory.initialize( device, sizeof( unsigned ) * 1000 ) ;
  
  if( !array_1.initialize( memory, 200 ) ) return false ;
  if( !array_2.initialize( memory, 200 ) ) return false ;
          
  array_1.reset() ;
  array_2.reset() ;
  return true ;
}

athena::Result test_image_initialization()
{
  Impl::Image<nyx::ImageFormat::RGBA8> image ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  if( image.initialize( device, 1280, 1024 ) )
  {
    image.reset() ;
    return true ;
  }

  return false ;
}

athena::Result test_image_size()
{
  const unsigned width  = 1280 ;
  const unsigned height = 1024 ;
  
  Impl::Image<nyx::ImageFormat::RGBA8> image ;
  if( !Impl::initialized() ) return athena::Result::Skip ;
  if( !image.initialize( device, width, height ) ) return false ;
  if( image.width()    != width                  ) return false ;
  if( image.height()   != height                 ) return false ;
  if( image.size()     != ( width * height )     ) return false ;
  if( image.byteSize() < ( width * height * 4 )  ) return false ;
  
  image.reset() ;
  return true ;
}

athena::Result test_image_resize()
{
  const unsigned width  = 1280 ;
  const unsigned height = 1024 ;
  
  Impl::Image<nyx::ImageFormat::RGBA8> image ;
  if( !Impl::initialized() ) return athena::Result::Skip ;
  if( !image.initialize( device, width, height )      ) return false ;
  if( image.width()    != width                       ) return false ;
  if( image.height()   != height                      ) return false ;
  if( image.size()     != ( width * height )          ) return false ;
  if( image.byteSize() < ( width * height * 4 )       ) return false ;
  if( image.layout()   != nyx::ImageLayout::Undefined ) return false ;
  
  image.transition( nyx::ImageLayout::General, graphics_queue ) ;
  
  if( !image.resize( 2048, 1720 )                 ) return false ;
  if( image.width()    != 2048                    ) return false ;
  if( image.height()   != 1720                    ) return false ;
  if( image.size()     != 2048 * 1720             ) return false ;
  if( image.byteSize() < ( 2048 * 1720 * 4 )      ) return false ;
  if( image.layout() != nyx::ImageLayout::General ) return false ;
  image.reset() ;
  return true ;
}

athena::Result test_renderer_init()
{
  nyx::Renderer<Impl, nyx::ImageFormat::RGBA8> renderer ;
  nyx::RGBAImage<Impl>                         image    ;

  renderer.setDimensions( 1920, 1080 ) ;
  renderer.initialize( device, nyx::bytes::draw, sizeof( nyx::bytes::draw ) ) ;
  image = renderer.framebuffer<0>() ;
  
  if( image.width() != 1920 || image.height() != 1080 ) return false ;
  return true ;
}

athena::Result test_renderer_draw()
{
  nyx::Renderer<Impl, nyx::ImageFormat::RGBA8> renderer ;
  nyx::Array<Impl, float>                      vertices ;
  nyx::RGBAImage<Impl>                         image    ;
  nyx::Viewport                                viewport ;
  
  viewport.setWidth ( 1280 ) ;
  viewport.setHeight( 1024 ) ;
  renderer.setDimensions( 1920, 1080 ) ;
  renderer.addViewport  ( viewport   ) ;
  vertices.initialize( device, 9, false, nyx::ArrayFlags::Vertex ) ;
  renderer.initialize( device, nyx::bytes::draw, sizeof( nyx::bytes::draw ), window.context() ) ;
  for( unsigned i = 0; i < 200 ; i++ )
  {
    renderer.draw( vertices ) ;
    renderer.finalize() ;
  }
  return true ;
}

athena::Result test_image_copy()
{
  Impl::Array<unsigned char> staging ; 
  nyx::RGBAImage<Impl>       image   ;
  int width  ;
  int height ;
  int chan   ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  auto bytes = stbi_load( "test_image.jpeg", &width, &height, &chan, STBI_rgb_alpha ) ;
  if( !bytes ) return false ;
  
  // Initialize objects.
  image  .initialize( device, static_cast<unsigned>( width ), static_cast<unsigned>( height ), 1                                             ) ;
  staging.initialize( device, static_cast<unsigned>( width * height * 4 ), true, nyx::ArrayFlags::TransferSrc | nyx::ArrayFlags::TransferDst ) ;
  
  // Copy to host-visible staging buffer first.
  staging.copyToDevice( bytes, static_cast<unsigned>( width * height * 4 ) ) ;

  // Now, record a GPU to GPU copy on the record.  
  image.copy( staging.buffer(), graphics_queue ) ;
  
  return true ;
}

int main()
{
  manager.initialize( "Nyx VULKAN Library" ) ;
  manager.add( "01) Instance Creation Test"              , &instance_initialization_test  ) ;
  manager.add( "02) Window Creation Test"                , &window_creation_test          ) ;
  manager.add( "03) Graphics Queue Grab Test"            , &graphics_queue_get_test       ) ;
  manager.add( "04) Swapchain Creation Test"             , &swapchain_creation_test       ) ;
  manager.add( "05) Memory::initialize Test"             , &test_memory_initialize        ) ;
  manager.add( "06) Memory::size Test"                   , &test_memory_size              ) ;
  manager.add( "07) Memory::offset Test"                 , &test_memory_offset            ) ;
  manager.add( "08) Memory::device Test"                 , &test_memory_device            ) ;
  manager.add( "09) Memory::syncToHost Test"             , &test_memory_sync_to_host_copy ) ;
  manager.add( "10) Array::initialize Test"              , &test_array_initialize         ) ;
  manager.add( "11) Array::initialize Preallocated Test" , &test_array_prealloc_init      ) ;
  manager.add( "12) Array::size Test"                    , &test_array_size               ) ;
  manager.add( "13) Array::copy Test"                    , &test_array_host_copy          ) ;
  manager.add( "14) Array::copy Test ( No waiting )"     , &test_array_copy_non_wait      ) ;
  manager.add( "15) Image::initialize Test"              , &test_image_initialization     ) ;
  manager.add( "16) Image::size Test"                    , &test_image_size               ) ;
  manager.add( "17) Image::resize Test"                  , &test_image_resize             ) ;
  manager.add( "18) Image::copy Test"                    , &test_image_copy               ) ;
  manager.add( "19) Renderer::initialize Test"           , &test_renderer_init            ) ;
  manager.add( "20) Renderer::draw Test"                 , &test_renderer_draw            ) ;
  
  return manager.test( athena::Output::Verbose ) ;
}
