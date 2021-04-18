/*
 * Copyright (C) 2021 Jordan Hendl
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

  /** Template class for performing and managing GPU operations.
   */
  template<typename Framework>
  class Chain
  {
    public:
      
      /** Default constructor.
       */
      Chain()  = default ;
      
      /** Default deconstructor.
       */
      ~Chain() = default ;
      
      /** Method to record a copy two library arrays to eachother.
       * @param src The array to copy from.
       * @param dst The array to copy to.
       * @param amt The amount of elements to copy. Defaults to entire size.
       * @param src_offset The offset of the first array to copy from.
       * @param dst_offset The offset of the second array to copy to.
       */
      template<typename Type>
      inline void copy( const nyx::Array<Framework, Type>& src, Array<Framework, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      /** Method to record a copy between a library array and a library image.
       * @param src The array to copy from.
       * @param dst The image to copy to.
       * @param amt The amount of data from the array to copy. Defaults to entire size.
       * @param src_offset The offset of the array to copy from.
       * @param dst_offset The offset of the image to copy to.
       */
      template<typename Type>
      inline void copy( const nyx::Array<Framework, Type>& src, nyx::Image<Framework>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;

      /** Method to record a copy from a library image to a library array.
       * @param src The image to copy from.
       * @param dst The array to copy to.
       * @param amt The amount of data from the image to copy from. Defaults to entire image.
       * @param src_offset The offset of the image to copy from.
       * @param dst_offset The offset of the array to copy to.
       */
      template<typename Type>
      inline void copy( const nyx::Image<Framework>& src, nyx::Array<Framework, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      /** Method to record a copy from a library array to a host data pointer.
       * @param src The array to copy from.
       * @param dst The host data pointer to copy data to.
       * @param amt The amount of data to copy. Defaults to the entire array size.
       * @param src_offset The offset of the source to copy from.
       * @param dst_offset The offset of the destination to copy to.
       */
      template<typename Type>
      inline void copy( const nyx::Array<Framework, Type>& src, Type* dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      /** Method to record a copy from a host data pointer to a library array.
       * @param src The host data pointer to copy from.
       * @param dst The library array to copy to.
       * @param amt The amount of data to copy. Defaults to entire size.
       * @param src_offset The offset of the source to copy from.
       * @param dst_offset The offset of the destination to copy to.
       */
      template<typename Type>
      inline void copy( const Type* src, nyx::Array<Framework, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      /** Method to record a copy from library image to library image.
       * @param src The source image to copy from.
       * @param dst The destination image to copy to.
       * @param amt The amount of data to copy. Defaults to entire size.
       * @param src_offset The offset of the source to copy from.
       * @param dst_offset The offset of the destination to copy to.
       */
      inline void copy( const nyx::Image<Framework>& src, nyx::Image<Framework>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
      
      /** Method to retrieve the device used by this object.
       * @return The device id used by this object.
       */
      inline unsigned device() const ;
      
      /** Method to combine the child chain's recorded data to this object's. 
       * If this object is part of rendering, this command causes the next 
       * @param child
       */
      inline void combine( const nyx::Chain<Framework>& child ) ;
      /** Method to append a draw command to this renderer.
       * @param array The array of vertices to draw.
       * @param offset The offset into the vertex array to start drawing at.
       */
      template<typename Type>
      inline void draw( const nyx::Renderer<Framework>& renderer, const Array<Framework, Type>& array, unsigned offset = 0 ) ;

      /** Method to append a draw command to this object using indices.
       * @param indices The index array describing the render order of the vertex array.
       * @param vertices The array of vertices used for drawing.
       */
      template<typename Type, typename Type2>
      inline void drawIndexed( const nyx::Renderer<Framework>& renderer, const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices ) ;

      /** Method to record an image transition operation.
       * @param image The image to transition.
       * @param layout The layout of the image to transition to.
       */
      inline void transition( nyx::Image<Framework>& image, nyx::ImageLayout layout ) ;
      
      /** Method to initialize this chain with a parent, making this chain a child of the input.
       * @param parent The chain to use as a parent.
       * @param subpass_id If the parent is part of a render pass, which id of subpass this child is recording for.
       */
      inline void initialize( const nyx::Chain<Framework>& parent, unsigned subpass_id = 0 ) ;

      /** Method to initialize this object with the given parameters.
       * @param gpu The ID of gpu to use.
       * @param type The type of queue to use for this chain.
       */
      inline void initialize( unsigned gpu, ChainType type ) ;
      
      /** Method to initialize this object with the given parameters.
       * @param gpu The ID of gpu to use.
       * @param window_id The id associated with the window to use for this object.
       */
      inline void initialize( unsigned gpu, unsigned window_id ) ;
      
      /** Method to initialize this object with the given parameters.
       * @param pass The render pass to use for this object's commands.
       * @param type The type of queue to use for this chain.
       */
      inline void initialize( const nyx::RenderPass<Framework>& render_pass, ChainType type ) ;
      
      /** Method to initialize this object with the given parameters.
       * @param pass The render pass to use for this object's commands.
       * @param window_id The id associated with the window to use for this object.
       */
      inline void initialize( const nyx::RenderPass<Framework>& render_pass, unsigned window_id ) ;
      
      /** Method to check whether or not this object is initialized.
       * @return Whether or not this object is initialized.
       */
      inline bool initialized() const ;

      /** Method to push a variable onto the input pipeline.
       * @param pipeline The pipeline to push the data to.
       * @param data The data to push.
       * @param offset The offset, in bytes, to update the data in the pipeline. Defaults to 0.
       */
      template<typename Type>
      inline void push( const nyx::Renderer<Framework>& pipeline, const Type& data, unsigned offset = 0 ) ;
      
      /** Method to synchronize this object's operations with the device.
       */
      inline void synchronize() ;
      
      /** Method to submit this object's work to the device.
       */
      inline void submit() ;

      /** Method to reset this object's data.
       */
      inline void reset() ;

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
  bool Chain<Framework>::initialized() const
  {
    return this->impl.initialized() ;
  }

  template<typename Framework>
  void Chain<Framework>::initialize( const nyx::Chain<Framework>& parent, unsigned subpass_id )
  {
    this->impl.initialize( parent, subpass_id ) ;
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

