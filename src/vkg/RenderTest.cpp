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
#include <library/Nyx.h>
#include <shaders/headers/draw_3d.h>
#include <vector>
#include <iostream>

using Framework = nyx::vkg::Vulkan ;

struct Vertex
{
  float x, y, z, w ;
};

struct Matrices
{
  float model[16] ;
  float view [16] ;
  float proj [16] ;
};

static nyx::RenderPass <Framework> render_pass   ;
static nyx::Renderer   <Framework> pipeline      ;
static nyx::Chain      <Framework> chain         ;
static Framework::Array<Vertex   > vertices      ;
static Framework::Array<Matrices > matrices      ;
static bool                        running       ;
std::vector<Vertex>                host_vertices = {
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
  std::cout << "Initializing Chain" << std::endl ;
  chain.initialize( render_pass, WINDOW_ID ) ;
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
  
  mat.model[ 0 ] = 1.0f ;
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
  
  while( running )
  {
    chain.push( pipeline, matrices.iterator() ) ;
    chain.draw( pipeline, vertices ) ;
    chain.submit() ;
    render_pass.present() ;
  }
  
  return 0 ;
}

