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

#include "Vulkan.h"
#include "library/Window.h"
#include "library/RenderPass.h"
#include "library/Renderer.h"
#include "library/Chain.h"
#include "event/Event.h"
#include <library/Nyx.h>
#include <shaders/headers/draw_3d.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using Framework = nyx::vkg::Vulkan ;

struct Matrices
{
  glm::mat4 model ;
  glm::mat4 view  ;
  glm::mat4 proj  ;
};

static nyx::EventManager           manager       ;
static nyx::RenderPass <Framework> render_pass   ;
static nyx::Renderer   <Framework> pipeline      ;
static nyx::Chain      <Framework> chain         ;
static nyx::Chain      <Framework> transfer      ;
static Framework::Array<glm::vec4> vertices      ;
static Framework::Array<Matrices > matrices      ;
static bool                        running       ;
std::vector<glm::vec4>                host_vertices = {
                                     {-0.5f, -0.5f, -0.5f, 1.0f}, 
                                     { 0.5f, -0.5f, -0.5f, 1.0f},  
                                     { 0.5f,  0.5f, -0.5f, 1.0f},  
                                     { 0.5f,  0.5f, -0.5f, 1.0f},  
                                     {-0.5f,  0.5f, -0.5f, 1.0f}, 
                                     {-0.5f, -0.5f, -0.5f, 1.0f}, 
                                
                                     {-0.5f, -0.5f,  0.5f, 1.0f},
                                     { 0.5f, -0.5f,  0.5f, 1.0f}, 
                                     { 0.5f,  0.5f,  0.5f, 1.0f}, 
                                     { 0.5f,  0.5f,  0.5f, 1.0f}, 
                                     {-0.5f,  0.5f,  0.5f, 1.0f},
                                     {-0.5f, -0.5f,  0.5f, 1.0f},
                                
                                     {-0.5f,  0.5f,  0.5f, 1.0f}, 
                                     {-0.5f,  0.5f, -0.5f, 1.0f}, 
                                     {-0.5f, -0.5f, -0.5f, 1.0f}, 
                                     {-0.5f, -0.5f, -0.5f, 1.0f}, 
                                     {-0.5f, -0.5f,  0.5f, 1.0f}, 
                                     {-0.5f,  0.5f,  0.5f, 1.0f}, 
                                
                                     { 0.5f,  0.5f,  0.5f,  1.0f},
                                     { 0.5f,  0.5f, -0.5f,  1.0f},
                                     { 0.5f, -0.5f, -0.5f,  1.0f},
                                     { 0.5f, -0.5f, -0.5f,  1.0f},
                                     { 0.5f, -0.5f,  0.5f,  1.0f},
                                     { 0.5f,  0.5f,  0.5f,  1.0f},
                                
                                     { -0.5f, -0.5f, -0.5f, 1.0f},
                                     {  0.5f, -0.5f, -0.5f, 1.0f}, 
                                     {  0.5f, -0.5f,  0.5f, 1.0f}, 
                                     {  0.5f, -0.5f,  0.5f, 1.0f}, 
                                     { -0.5f, -0.5f,  0.5f, 1.0f},
                                     { -0.5f, -0.5f, -0.5f, 1.0f},
                                
                                     { -0.5f,  0.5f, -0.5f, 1.0f},
                                     {  0.5f,  0.5f, -0.5f, 1.0f}, 
                                     {  0.5f,  0.5f,  0.5f, 1.0f}, 
                                     {  0.5f,  0.5f,  0.5f, 1.0f}, 
                                     { -0.5f,  0.5f,  0.5f, 1.0f},
                                     { -0.5f,  0.5f, -0.5f, 1.0f}
    };



constexpr unsigned DEVICE    = 0   ;
constexpr unsigned WIDTH     = 720 ;
constexpr unsigned HEIGHT    = 524 ;
constexpr unsigned WINDOW_ID = 0   ;

void setupRenderPass()
{
  nyx::Attachment attachment ;
  nyx::Subpass    subpass    ;
  
  attachment.setClearColor( 0, 0, 0, 1 ) ;
  attachment.setFormat( nyx::ImageFormat::RGBA8           ) ;
  attachment.setLayout( nyx::ImageLayout::ColorAttachment ) ;
  
  subpass.addAttachment ( attachment ) ;
  render_pass.addSubpass( subpass    ) ;
  
  std::cout << "Initializing RenderPass" << std::endl ;
  render_pass.initialize( DEVICE, WINDOW_ID ) ;
}

void setupChain()
{
  std::cout << "Initializing Chains" << std::endl ;
  chain   .initialize( render_pass, WINDOW_ID               ) ;
  transfer.initialize( DEVICE     , nyx::ChainType::Compute ) ;
}

void setupVertices()
{
  vertices.initialize( DEVICE, host_vertices.size(), false, nyx::ArrayFlags::Vertex ) ;
  
  std::cout << "Copying Vertices to Device" << std::endl ;
  chain.copy( host_vertices.data(), vertices ) ;
  chain.submit()      ;
  chain.synchronize() ;
}

void setupMatrices()
{
  Matrices mat ;
  
  mat.model = glm::mat4( 1 ) ;
  mat.view  = glm::lookAtRH( glm::vec3( 0 ), glm::vec3( 0 ), glm::vec3( 0, 1.f, 0.f ) ) ;
  mat.proj  = glm::perspective( glm::radians( 90.0f ), static_cast<float>( WIDTH ) / static_cast<float>( HEIGHT ), 0.0f, 100.0f ) ;
  
  matrices.initialize( DEVICE, 1 ) ;
  chain.copy( &mat, matrices ) ;
  chain.submit() ;
  chain.synchronize() ;
}

void setupPipeline()
{
  nyx::Viewport viewport ;
  
  viewport.setWidth ( WIDTH  ) ;
  viewport.setHeight( HEIGHT ) ;
  
  pipeline.addViewport( viewport ) ;
  std::cout << "Initializing Pipeline" << std::endl ;
  pipeline.initialize( DEVICE, render_pass, nyx::bytes::draw_3d, sizeof( nyx::bytes::draw_3d ) ) ;
}

void respond( const nyx::Event& event )
{
  static bool toggle = false ;
  
  if( event.type() == nyx::Event::Type::KeyDown )
  {  
    if( event.key() == nyx::Key::Return )
    {
      Matrices mat ;
      
      if( toggle )
      {
        toggle = false ;
        mat.model[ 0 ][ 0 ] = 1.0f ;
      }
      else
      {
        mat.model[ 0 ][ 0 ] = 0.0f ;
        toggle = true ;
      }
      
      transfer.copy( &mat, matrices ) ;
      transfer.submit() ;
    }
    
    if( event.key() == nyx::Key::ESC )
    {
      running = false ;
    }
  }
}

int main()
{
  Framework::setApplicationName  ( "NYX-VKG Window Test App"                      ) ;
  Framework::addInstanceExtension( Framework::platformSurfaceInstanceExtensions() ) ;
  Framework::addInstanceExtension( "VK_KHR_surface"                               ) ;
  Framework::addValidationLayer  ( "VK_LAYER_KHRONOS_validation"                  ) ;
  Framework::addValidationLayer  ( "VK_LAYER_LUNARG_standard_validation"          ) ;
  Framework::addDeviceExtension  ( "VK_KHR_swapchain"                             ) ;
  Framework::addDeviceExtension  ( "VK_KHR_shader_non_semantic_info"              ) ;

  Framework::addWindow( WINDOW_ID, "Nyx Render Test", WIDTH, HEIGHT ) ;
  
  running = true ;
  
  setupRenderPass() ;
  setupChain     () ;
  setupVertices  () ;
  setupMatrices  () ;
  setupPipeline  () ;
  
  manager.enroll( &respond, "Keyboard Response" ) ;
  while( running )
  {
    chain.push( pipeline, matrices.iterator() ) ;
    chain.draw( pipeline, vertices ) ;
    chain.submit() ;
    render_pass.present() ;
    Framework::handleWindowEvents( WINDOW_ID ) ;
  }
  
  return 0 ;
}

