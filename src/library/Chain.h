/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Link.h
 * Author: jhendl
 *
 * Created on March 19, 2021, 7:14 PM
 */

#pragma once
#include "Array.h"
#include "Image.h"
#include "RenderPass.h"
#include "Renderer.h"

namespace nyx
{
  enum class ChainType : unsigned
  {
    Graphics,
    Compute,
    Transfer
  };
  
  enum class PipelineStage : unsigned ;
  
  template<typename Framework, typename Type>
  class Array ;
  
  template<typename Framework>
  class Image ;
  
  template<typename Framework>
  class RenderPass ;

  template<typename Framework>
  class Renderer ;

  template<typename Framework>
  class Chain
  {
    public:

      Chain()  = default ;

      ~Chain() = default ;
      
      template<typename Type>
      void copy( const nyx::Array<Framework, Type>& src, Array<Framework, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      template<typename Type>
      void copy( const nyx::Array<Framework, Type>& src, nyx::Image<Framework>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      template<typename Type>
      void copy( const nyx::Image<Framework>& src, nyx::Array<Framework, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      template<typename Type>
      void copy( const nyx::Array<Framework, Type>& src, Type* dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      template<typename Type>
      void copy( const Type* src, nyx::Array<Framework, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      void copy( const nyx::Image<Framework>& src, nyx::Image<Framework>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      /** Method to append a draw command to this renderer.
       * @param array The array of vertices to draw.
       * @param offset The offset into the vertex array to start drawing at.
       */
      template<typename Type>
      void draw( const nyx::Renderer<Framework>& renderer, const Array<Framework, Type>& array, unsigned offset = 0 ) ;

      /** Method to append a draw command to this renderer using indices.
       * @param indices The index array describing the render order of the vertex array.
       * @param vertices The array of vertices used for drawing.
       */
      template<typename Type, typename Type2>
      void drawIndexed( const nyx::Renderer<Framework>& renderer, const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices ) ;

      void transition( nyx::Image<Framework>& image, nyx::ImageLayout layout ) ;

      void initialize( unsigned gpu, ChainType type ) ;
      
      void initialize( unsigned gpu, unsigned window_id ) ;
      
      void initialize( const nyx::RenderPass<Framework>& render_pass, ChainType type ) ;
      void initialize( const nyx::RenderPass<Framework>& render_pass, unsigned window_id ) ;
      
      template<typename Type>
      void push( const nyx::Renderer<Framework>& pipeline, const Type& data, unsigned offset = 0 ) ;
      
      void synchronize() ;
      
      void submit() ;
      
      void reset() ;
    private:
      typename Framework::Chain impl ;
  };
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::copy( const nyx::Array<Framework, Type>& src, Array<Framework, Type>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
  {
    if( amt == 0 ) amt = src.size() ;
    this->impl.copy( src, dst, amt, src_offset, dst_offset ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::copy( const nyx::Array<Framework, Type>& src, nyx::Image<Framework>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
  {
    if( amt == 0 ) amt = src.size() ;
    this->impl.copy( src, dst, amt, src_offset, dst_offset ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::copy( const nyx::Image<Framework>& src, nyx::Array<Framework, Type>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
  {
    if( amt == 0 ) amt = src.byteSize() ;
    this->impl.copy( src, dst, amt, src_offset, dst_offset ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::copy( const nyx::Array<Framework, Type>& src, Type* dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
  {
    if( amt == 0 ) amt = src.size() ;
    this->impl.copy( src, dst, amt, src_offset, dst_offset ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::copy( const Type* src, nyx::Array<Framework, Type>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
  {
    if( amt == 0 ) amt = dst.size() ;
    this->impl.copy( src, dst, amt, src_offset, dst_offset ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::copy( const nyx::Image<Framework>& src, nyx::Image<Framework>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
  {
    if( amt == 0 ) amt = src.byteSize() ;
    this->impl.copy( src, dst, amt, src_offset, dst_offset ) ;
  }

  template<typename Framework>
  template<typename Type, typename Type2>
  void Chain<Framework>::drawIndexed( const nyx::Renderer<Framework>& renderer, const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices )
  {
    this->impl.drawIndexed( renderer, indices, vertices ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::draw( const nyx::Renderer<Framework>& renderer, const Array<Framework,Type>& array, unsigned offset )
  {
    this->impl.draw( renderer, array, offset ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::submit()
  {
    this->impl.submit() ;
  }
  
  template<typename Framework>
  void Chain<Framework>::transition( nyx::Image<Framework>& image, nyx::ImageLayout layout )
  {
    this->impl.transition( image, layout ) ;
  }

  template<typename Framework>
  void Chain<Framework>::synchronize()
  {
    this->impl.synchronize() ;
  }
 
  template<typename Framework>
  void Chain<Framework>::initialize( unsigned gpu, ChainType type )
  {
    this->impl.initialize( gpu, type ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::initialize( unsigned gpu, unsigned window_id )
  {
    this->impl.initialize( gpu, window_id ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::initialize( const nyx::RenderPass<Framework>& render_pass, ChainType type )
  {
    this->impl.initialize( render_pass, type ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::initialize( const nyx::RenderPass<Framework>& render_pass, unsigned window_id )
  {
    this->impl.initialize( render_pass, window_id ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::push( const nyx::Renderer<Framework>& pipeline, const Type& data, unsigned offset )
  {
    this->impl.push( pipeline, data, offset ) ;
  }

  template<typename Framework>
  void Chain<Framework>::reset()
  {
    this->impl.reset() ;
  }
}

