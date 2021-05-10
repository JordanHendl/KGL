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
#include <library/Renderer.h>
#include "library/RenderPass.h"
#include "library/Chain.h"
#include <loaders/NgtFile.h>
#include <loaders/NyxFile.h>
#include <binary/sheep.h>
#include <binary/draw.h>
#include <binary/buffer_reference.h>
#include <binary/color_depth.h>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <athena/Manager.h>

constexpr unsigned device = 0 ;

using Impl = nyx::vkg::Vulkan ;

constexpr unsigned WINDOW_ID = 8008135 ;

static Impl::Instance        instance       ;
static Impl::Queue           graphics_queue ;
static Impl::Swapchain       swapchain      ;
static nyx::RenderPass<Impl> render_pass    ;
static athena::Manager       manager        ;

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
  Impl::addWindow( WINDOW_ID, "Test", 1280, 1024 ) ;
  
  if( Impl::hasWindow( WINDOW_ID ) ) return true ;
  return false ;
}


athena::Result graphics_queue_get_test()
{
  graphics_queue = Impl::presentQueue( WINDOW_ID, device ) ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  if( !graphics_queue.valid() || !graphics_queue.graphics() ) return false ;
  
  return true ;
}

athena::Result swapchain_creation_test()
{
  swapchain.initialize( graphics_queue, WINDOW_ID ) ;
  if( !Impl::initialized()  ) return athena::Result::Skip ;
  if( swapchain.count() == 0 ) return false ;
  
  swapchain.reset() ;
  return true ;
}

athena::Result test_render_pass_creation()
{
  nyx::Attachment attachment ;
  nyx::Subpass    subpass    ;
  
  attachment.setClearColor( 0.5, 0.5, 0.5, 1.0                ) ;
  attachment.setFormat    ( nyx::ImageFormat::RGBA8           ) ;
  attachment.setLayout    ( nyx::ImageLayout::ColorAttachment ) ;
  
  subpass.addAttachment( attachment ) ;
  render_pass.addSubpass( subpass ) ;

  if( !Impl::initialized()  ) return athena::Result::Skip ;
  render_pass.initialize( device, WINDOW_ID ) ;
  if( !render_pass.initialized() ) return false ;
  
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
  std::vector<unsigned> test_array ;
  std::vector<unsigned> test_array_result ;
  
  Impl::Array<unsigned> buffer_1 ;
  Impl::Array<unsigned> buffer_2 ;
  
  nyx::Chain<Impl> chain ;
  test_array       .resize( 500 ) ;
  test_array_result.resize( 500 ) ;
  
  std::fill( test_array       .begin(), test_array       .end(), 78106 ) ;
  std::fill( test_array_result.begin(), test_array_result.end(), 0     ) ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  buffer_1.initialize( device, 500       ) ;
  buffer_2.initialize( device, 500       ) ;
  chain   .initialize( device, WINDOW_ID ) ;
  
  chain.copy( test_array.data(), buffer_1                 ) ;
  chain.copy( buffer_1         , buffer_2                 ) ;
  chain.copy( buffer_2         , test_array_result.data() ) ;
  
  chain.submit     () ;
  chain.synchronize() ;
  
  for( unsigned index = 0; index < 500; index++ )
  {
    if( test_array_result[ index ] != 78106 )
    {
      return false ;
    }
  }
  
  buffer_1.reset() ;
  buffer_2.reset() ;
  chain   .reset() ;
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
  Impl::Image image ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  if( image.initialize( nyx::ImageFormat::RGBA8, device, 1280, 1024 ) )
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
  
  Impl::Image      image ;

  if( !Impl::initialized() ) return athena::Result::Skip ; 
  if( !image.initialize( nyx::ImageFormat::RGBA8, device, width, height ) ) return false ;
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
  
  Impl::Image      image ;
  nyx::Chain<Impl> chain ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  chain.initialize( device, nyx::ChainType::Graphics ) ;
  if( !image.initialize( nyx::ImageFormat::RGBA8, device, width, height ) ) return false ;
  if( image.width()    != width                                           ) return false ;
  if( image.height()   != height                                          ) return false ;
  if( image.size()     != ( width * height )                              ) return false ;
  if( image.byteSize() < ( width * height * 4 )                           ) return false ;
  if( image.layout()   != nyx::ImageLayout::Undefined                     ) return false ;
  
  chain.transition( image, nyx::ImageLayout::General ) ;
  chain.submit() ;
  chain.synchronize() ;
  if( !image.resize( 2048, 1720 )                 ) return false ;
  if( image.width()    != 2048                    ) return false ;
  if( image.height()   != 1720                    ) return false ;
  if( image.size()     != 2048 * 1720             ) return false ;
  if( image.byteSize() < ( 2048 * 1720 * 4 )      ) return false ;
  if( image.layout() != nyx::ImageLayout::General ) return false ;
  image.reset() ;
  return true ;
}

athena::Result test_image_copy()
{
  Impl::Array<unsigned char> staging ; 
  nyx::Image<Impl>           image   ;
  nyx::Chain<Impl>           chain   ;
  nyx::NgtFile               loader  ;
  unsigned                   width   ;
  unsigned                   height  ;
  unsigned                   channel ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  chain.initialize( device, nyx::ChainType::Graphics ) ;
  
  // Initialize objects.
  loader.load( nyx::bytes::sheep, sizeof( nyx::bytes::sheep ) ) ;
  
  width   = loader.width   () ;
  height  = loader.height  () ;
  channel = loader.channels() ;

  image  .initialize( nyx::ImageFormat::RGBA8, device, static_cast<unsigned>( width ), static_cast<unsigned>( height ) ) ;
  staging.initialize( device, static_cast<unsigned>( width * height * channel ) ) ;
  
  chain.copy( loader.image(), staging ) ;
  chain.copy( staging       , image   ) ;

  chain.submit() ;
  chain.synchronize() ;
  return true ;
}

athena::Result test_renderer_init()
{
  nyx::Renderer<Impl> renderer ;
  nyx::Viewport       viewport ;
  
  viewport.setWidth ( 1280 ) ;
  viewport.setHeight( 1024 ) ;
  if( !Impl::initialized() ) return athena::Result::Skip ;
  renderer.addViewport( viewport ) ;
  renderer.initialize( device, render_pass, nyx::bytes::draw, sizeof( nyx::bytes::draw ) ) ;
  
  if( !renderer.initialized() ) return false ;
  return true ;
}

athena::Result test_renderer_draw()
{
  nyx::Renderer<Impl> renderer ;
  Impl::Array<float>  vertices ;
  Impl::Image         image  ;
  nyx::Viewport       viewport ;
  nyx::Chain<Impl>    chain    ;
  
  viewport.setWidth ( 1280 ) ;
  viewport.setHeight( 1024 ) ;
  if( !Impl::initialized() ) return athena::Result::Skip ;
  chain   .initialize ( device, nyx::ChainType::Graphics               ) ;
  image   .initialize ( nyx::ImageFormat::RGBA8, device, 1280, 1024, 1 ) ;
  renderer.addViewport( viewport                                       ) ;
  
  chain.transition( image, nyx::ImageLayout::ShaderRead      ) ;
  chain.submit() ;
  chain.synchronize() ;

  chain.reset() ;
  chain.initialize( render_pass, WINDOW_ID ) ;
  vertices.initialize( device, 9, false, nyx::ArrayFlags::Vertex ) ;
  renderer.initialize( device, render_pass, nyx::bytes::draw, sizeof( nyx::bytes::draw ) ) ;
  renderer.bind( "framebuffer", image ) ;
  
  for( unsigned i = 0; i < 20 ; i++ )
  {
    chain.draw( renderer, vertices ) ;
  }
  
  chain.submit     () ;
  chain.synchronize() ;
  renderer.reset() ;
  return true ;
}

athena::Result test_buffer_reference_iterator()
{
  struct vec4
  {
    float x, y, z, w;
  };
  
  nyx::Renderer<Impl> pipeline ;
  Impl::Array<vec4>   vertices ;
  Impl::Array<vec4>   array    ;
  Impl::Array<bool>   uniform  ;
  nyx::Chain<Impl>    chain    ;
  nyx::Viewport       viewport ;
  
  viewport.setWidth ( 1280 ) ;
  viewport.setHeight( 1024 ) ;
  
  if( !Impl::initialized() ) return athena::Result::Skip ;
  
  pipeline.addViewport( viewport ) ;
  
  // Initialize all the gpu objects needed.
  chain   .initialize( render_pass, WINDOW_ID                                                                    ) ;
  array   .initialize( device, 1                                                                                 ) ;
  uniform .initialize( device, 1, false, nyx::ArrayFlags::UniformBuffer                                          ) ;
  vertices.initialize( device, 9, false, nyx::ArrayFlags::Vertex                                                 ) ;
  pipeline.initialize( device, render_pass, nyx::bytes::buffer_reference, sizeof( nyx::bytes::buffer_reference ) ) ;
  
  // Bind an array to a uniform location. This array is now what is being used for the uniform.
  pipeline.bind( "ColorInfo", uniform ) ;
  
  chain.push( pipeline, array.iterator() ) ;
  chain.draw( pipeline, vertices ) ;
  
  for( unsigned i = 0; i < 1 ; i++ )
  {
    chain.draw( pipeline, vertices ) ;
  }
  
  chain.submit() ;
  return true ;
}

athena::Result test_rendering_with_subpasses()
{
  struct vec4
  {
    float x, y, z, w ;
  };

  nyx::RenderPass<Impl> pass       ;
  nyx::Renderer  <Impl> pipeline   ;
  Impl::Array    <vec4> vertices   ;
  nyx::Subpass          subpass    ;
  nyx::Chain<Impl>      chain      ;
  nyx::Attachment       attachment ;
  nyx::Viewport         viewport   ;
  
  viewport.setWidth ( 1280 ) ;
  viewport.setHeight( 1024 ) ;
  
  pipeline.addViewport  ( viewport   ) ;
  
  attachment.setClearColor( 0.0f, 0.f, 0.f, 1.0f              ) ;
  attachment.setLayout    ( nyx::ImageLayout::ColorAttachment ) ;
  attachment.setFormat    ( nyx::ImageFormat::RGBA8           ) ;
  subpass.addAttachment( attachment ) ;
  
  pass.addSubpass( subpass ) ;
  pass    .initialize( device                                                                   ) ;
  pipeline.initialize( device, pass, nyx::bytes::color_depth, sizeof( nyx::bytes::color_depth ) ) ;
  chain   .initialize( pass, nyx::ChainType::Graphics                                           ) ;
  vertices.initialize( device, 9, false, nyx::ArrayFlags::Vertex                                ) ;
  
  chain.draw( pipeline, vertices ) ;
  
  chain.submit() ;
  return true ;
}

int main()
{
  manager.initialize( "Nyx VULKAN Library" ) ;

  manager.add( "01) Instance Creation"                 , &instance_initialization_test   ) ;
  manager.add( "02) Window Creation"                   , &window_creation_test           ) ;
  manager.add( "03) Graphics Queue Grab"               , &graphics_queue_get_test        ) ;
  manager.add( "04) Swapchain Creation"                , &swapchain_creation_test        ) ;
  manager.add( "05) Memory::initialize"                , &test_memory_initialize         ) ;
  manager.add( "06) Memory::size"                      , &test_memory_size               ) ;
  manager.add( "07) Memory::offset"                    , &test_memory_offset             ) ;
  manager.add( "08) Memory::device"                    , &test_memory_device             ) ;
  manager.add( "09) Memory::syncToHost"                , &test_memory_sync_to_host_copy  ) ;
  manager.add( "10) Array::initialize"                 , &test_array_initialize          ) ;
  manager.add( "11) Array::initialize Preallocated"    , &test_array_prealloc_init       ) ;
  manager.add( "12) Array::size"                       , &test_array_size                ) ;
  manager.add( "13) Array::copy"                       , &test_array_host_copy           ) ;
  manager.add( "14) Image::initialize"                 , &test_image_initialization      ) ;
  manager.add( "15) Image::size"                       , &test_image_size                ) ;
  manager.add( "17) Image::resize"                     , &test_image_resize              ) ;
  manager.add( "18) Image::copy"                       , &test_image_copy                ) ;
  manager.add( "19) RenderPass::initialize"            , &test_render_pass_creation      ) ;
  manager.add( "20) Renderer::initialize"              , &test_renderer_init             ) ;
  manager.add( "21) Renderer::draw"                    , &test_renderer_draw             ) ;
  manager.add( "22) Renderer::draw with array iterator", &test_buffer_reference_iterator ) ;
  manager.add( "23) Renderer::draw with subpasses"     , &test_rendering_with_subpasses  ) ;
  
  return manager.test( athena::Output::Verbose ) ;
}
