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
 * File:   RenderTest.cpp
 * Author: jhendl
 *
 * Created on April 7, 2021, 12:50 PM
 */

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Vulkan.h"
#include "Model.h"
#include "library/Window.h"
#include "library/RenderPass.h"
#include "library/Pipeline.h"
#include "library/Chain.h"
#include "event/Event.h"
#include <loaders/NgtFile.h>
#include <loaders/NggFile.h>
#include <library/Nyx.h>
#include <binary/draw_3d.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>

// Temporarily using glm for now. Prob use something else in the future.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using Framework = nyx::vkg::Vulkan ;

struct Matrices
{
  glm::mat4 model ;
  glm::mat4 view  ;
  glm::mat4 proj  ;
};

constexpr unsigned  DEVICE_ID = 0   ;
constexpr unsigned  WINDOW_ID = 0   ;
constexpr unsigned  WIDTH     = 720 ;
constexpr unsigned  HEIGHT    = 524 ;

static glm::vec3 POSITION  = glm::vec3( 0.0f, 0.0f, 0.0f ) ;  
static float     ROTATION  = 0.f                           ;

static nyx::EventManager                      manager       ;
static nyx::RenderPass <Framework>            render_pass   ;
static nyx::Pipeline   <Framework>            pipeline      ;
static nyx::Chain      <Framework>            chain         ;
static nyx::Chain      <Framework>            transfer      ;
static nyx::Image      <Framework>            image         ;
static mars::Model     <Framework>            model         ;
static Framework::Array<Matrices >            matrices      ;
static Matrices                               mat           ;
static bool                                   running       ;
static bool                                   paused        ;

static auto start_time = std::chrono::high_resolution_clock::now() ;

void setupRenderPass()
{
  nyx::Attachment attachment ;
  nyx::Subpass    subpass    ;
  
  attachment.setClearColor( 0, 0, 0, 1                    ) ;
  attachment.setFormat( nyx::ImageFormat::BGRA8           ) ;
  attachment.setLayout( nyx::ImageLayout::ColorAttachment ) ;
  
  subpass.addAttachment        ( attachment ) ;
  subpass.setDepthStencilEnable( true       ) ;
  subpass.setDepthClearValue   ( 1.0f       ) ;
  render_pass.addSubpass       ( subpass    ) ;
  
  std::cout << "Initializing RenderPass" << std::endl ;
  render_pass.initialize( DEVICE_ID, WINDOW_ID ) ;
}

void setupChain()
{
  std::cout << "Initializing Chains" << std::endl ;
  chain   .initialize( render_pass, WINDOW_ID                  ) ;
  transfer.initialize( DEVICE_ID     , nyx::ChainType::Compute ) ;
}

void setupVertices( const char* path )
{
  model.initialize( path, DEVICE_ID ) ;

  if( !model.initialized() )
  {
    std::cout << "Error loading model at " << path << ".\n" ;
    exit( 0 ) ;
  }
}

void setupMatrices()
{
  mat.view  = glm::lookAt( glm::vec3( 0.f, -2.0f, -1.f ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f )                ) ;
  mat.proj  = glm::perspective( glm::radians( 90.0f ), static_cast<float>( WIDTH ) / static_cast<float>( HEIGHT ), 0.1f, 5000.0f ) ;
  
  matrices.initialize( DEVICE_ID, 1 ) ;
  transfer.copy( &mat, matrices ) ;
  transfer.submit() ;
  transfer.synchronize() ;
}

void setupPipeline()
{
  nyx::Viewport viewport ;
  
  viewport.setWidth ( WIDTH  ) ;
  viewport.setHeight( HEIGHT ) ;
  
  pipeline.addViewport( viewport ) ;
  pipeline.setTestDepth( true ) ;
  std::cout << "Initializing Pipeline" << std::endl ;
  pipeline.initialize( render_pass, nyx::bytes::draw_3d, sizeof( nyx::bytes::draw_3d ) ) ;
  
  if( image.initialized() )
  {
    pipeline.bind( "mesh_texture", image ) ;
  }
}

void setupTexture( const char* file_path )
{
  Framework::Array<unsigned char> staging_buffer ;
  nyx::NgtFile                    loader         ;
  unsigned                        width          ;
  unsigned                        height         ;
  const unsigned char*            bytes          ;
  
  if( !loader.load( file_path ) ) 
  {
    std::cout << "Problem loading image " << file_path << std::endl ;
    exit( 0 ) ;
  }

  width  = loader.width   () ;
  height = loader.height  () ;
  bytes  = loader.image   () ;
  
  if( width != 0 && height != 0 )
  {
    staging_buffer.initialize( DEVICE_ID, width * height * 4, true ) ;
    image         .initialize( nyx::ImageFormat::RGBA8, DEVICE_ID, width, height, 1 ) ;
    
    transfer.copy( bytes, staging_buffer ) ;
    transfer.transition( image, nyx::ImageLayout::TransferDst ) ;
    transfer.copy( staging_buffer, image ) ;
    transfer.transition( image, nyx::ImageLayout::ShaderRead ) ;
    
    transfer.submit()      ;
    transfer.synchronize() ;
    
    staging_buffer.reset() ;
  }
  else
  {
    std::cout << "Problem loading image " << file_path << std::endl ;
    exit( 0 ) ;
  }
  
  loader.reset() ;
}

void respond( const nyx::Event& event )
{
  if( event.type() == nyx::Event::Type::WindowExit )
  {
    running = false ;
  }

  if( event.type() == nyx::Event::Type::KeyDown )
  {  
    switch( event.key() )
    {
      case nyx::Key::Space :
        paused = !paused ;
        
        if( paused )
        {
          start_time = std::chrono::high_resolution_clock::now() ;
        }
      break ;
      
      case nyx::Key::Z :
        ROTATION = ROTATION < glm::radians( 180.f ) ? glm::radians( 180.f ) : 0.f ;
        break ;
      case nyx::Key::ESC :
        running = false ;
      break ;
      case nyx::Key::Up :
        POSITION.y += 0.5f ;
      break ;
      case nyx::Key::Down :
        POSITION.y -= 0.5f ;
      break ;
      case nyx::Key::Left :
        POSITION.z -= 0.5f ;
      break ;
      case nyx::Key::Right :
        POSITION.z += 0.5f ;
      break ;
      default : break ;
    }
  }
}

int main( int argc, char** argv )
{
  glm::mat4 pos  ;
  float     time ;

  start_time = std::chrono::high_resolution_clock::now() ;
  
  // Set the necessary required extensions/debug layers.
  Framework::setApplicationName  ( "NYX-VKG Window Test App"                      ) ;
  Framework::addInstanceExtension( Framework::platformSurfaceInstanceExtensions() ) ;
  Framework::addInstanceExtension( "VK_KHR_surface"                               ) ;
  Framework::addValidationLayer  ( "VK_LAYER_KHRONOS_validation"                  ) ;
  Framework::addValidationLayer  ( "VK_LAYER_LUNARG_standard_validation"          ) ;
  Framework::addDeviceExtension  ( "VK_KHR_swapchain"                             ) ;

  Framework::addWindow( WINDOW_ID, "Nyx Render Test", WIDTH, HEIGHT ) ;
  
  running = true  ;
  paused  = false ;

  if( argc < 3 ) 
  {
    std::cout << "Usage: nyx_vkg_window_text <model>.ngg <texture>.ngt\n" << std::endl ;
    return 0 ;
  }
  // Setup all necessary parts for rendering.
  setupRenderPass(           ) ;
  setupChain     (           ) ;
  setupVertices  ( argv[ 1 ] ) ;
  setupTexture   ( argv[ 2 ] ) ;
  setupMatrices  (           ) ;
  setupPipeline  (           ) ;
  
  // Enroll a function so we can respond to inputs.
  manager.enroll( &respond, "Keyboard Response" ) ;
  
  // Render loop.
  while( running )
  {
    auto current_time = std::chrono::high_resolution_clock::now();
    
    // Push the iterator to the matrices & draw.
    chain.push( pipeline, matrices.iterator() ) ;
    model.draw( pipeline, chain               ) ;
    chain.submit() ;
    
    if( !paused )
    {
      time     += std::chrono::duration<float, std::chrono::seconds::period>( current_time - start_time ).count() ;
      pos       = glm::translate( glm::mat4( 1.0f ), POSITION ) ;
      mat.model = glm::rotate( pos, time * glm::radians( 90.0f ), glm::vec3(0.0f, 0.0f, 1.0f ) ) ;
      mat.model = glm::rotate( mat.model, ROTATION, glm::vec3( 0.f, 1.0f, 0.0f ) ) ;
      
      // Update the matrix buffer
      transfer.copy( &mat, matrices ) ;
      transfer.submit() ;
    }
    
    // Present to screen and publish window events.
    render_pass.present() ;
    Framework::handleWindowEvents( WINDOW_ID ) ;
    
    start_time = current_time ;
  }
  
  return 0 ;
}

