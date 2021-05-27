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
  enum class GPUStages : unsigned
  {
    AccelerationStructureBuild,
    AllGraphics,
    AllCommands,
    BottomOfPipe,
    ColorAttachmentOutput,
    ComputeShader,
    CommandPreprocess,
    DrawIndirect,
    EarlyFragmentTests,
    FragmentShader,
    GeometryShader,
    Host,
    LateFragmentTests,
    MeshShader,
    RayTracing,
    ShadingRateImage,
    Top,
    TaskShader,
    Transfer,
    TessellationControlShader,
    TessellationEvaluationShader,
    VertexInput,
    VertexShader,
  };
  
  enum class ChainType : unsigned
  {
    Graphics, ///< Graphics GPU queue's are used.
    Compute,  ///< Compute GPU queue's are used.
    Transfer  ///< Transfer GPU queue's are used.
  };
  
  enum class ChainMode : unsigned 
  {
    Sequential, ///< Chain operates all commands sequentially to all internal command records.
    All         ///< Chain operates all commands to all internal command buffers at once.
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
      Chain() { static_assert( sizeof( this ) == sizeof( typename Framework::Chain ) ) ; } ;
      
      /** Default deconstructor.
       */
      ~Chain() = default ;
      
      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      inline operator const typename Framework::Chain&() const ;

      /** Conversion operator for the implementation-specific version of this object, so this object can be used in the place of that as well.
       */
      inline operator typename Framework::Chain&() ;
      
      /** Method to force advance this chain to the next buffering.
       */
      inline void advance() ;
      
      /** Method to explicitly begin recording of this chain. Note: All operations on this object implicitly begin it's operation. There may be times though,
       *  where explicitly beginning it might be the preferred solution
       */
      inline void begin() ;
      
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
      
      /** Method to append a draw command to this object using indices.
       * @param indices The index array describing the render order of the vertex array.
       * @param vertices The array of vertices used for drawing.
       */
      template<typename Type, typename Type2>
      inline void drawInstanced( unsigned instance_count, const nyx::Renderer<Framework>& renderer, const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices ) ;

      /** Method to append a draw command to this object using indices.
       * @param indices The index array describing the render order of the vertex array.
       * @param vertices The array of vertices used for drawing.
       */
      template<typename Type>
      inline void drawInstanced( unsigned instance_count, const nyx::Renderer<Framework>& renderer, const Array<Framework, Type>& vertices ) ;

      /** Method to explicitly end recording of this object.
       * @note The submit method implicitly ends this chain's record as well.
       */
      inline void end() ;

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
      inline void initialize( const nyx::RenderPass<Framework>& render_pass, ChainType type, bool multi_pass = false ) ;
      
      /** Method to initialize this object with the given parameters.
       * @param pass The render pass to use for this object's commands.
       * @param window_id The id associated with the window to use for this object.
       */
      inline void initialize( const nyx::RenderPass<Framework>& render_pass, unsigned window_id, bool multi_pass = false ) ;
      
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
      
      /** Method to set the mode of this chain.
       * @param mode The mode of this chain to operate on.
       */
      inline void setMode( nyx::ChainMode mode ) ;

      /** Method to synchronize this object's operations with the device.
       */
      inline void synchronize() ;
      
      /** Method to submit this object's work to the device.
       */
      inline void submit() ;

      /** Method to generate a memory barrier between the two inputs.
       * @param read The array being read from.
       * @param write The array being written to.
       */
      template<typename Type>
      inline void memoryBarrier( const nyx::Array<Framework, Type>& read, const nyx::Array<Framework, Type>& write ) ;
      
      /** Method to generate a memory barrier between the two inputs.
       * @param read The array being read from.
       * @param write The image being written to.
       */
      template<typename Type>
      inline void memoryBarrier( const nyx::Array<Framework, Type>& read, const nyx::Image<Framework>& write ) ;
      
      /** Method to barrier the GPU pipeline.
       * @param src The pipeline stage to consume.
       * @param dst The pipeline stage to output.
       */
      inline void pipelineBarrier( GPUStages src, GPUStages dst ) ;
      
      /** Method to reset this object's data.
       */
      inline void reset() ;

    private:
      typename Framework::Chain impl ;
  };
  
  
  template<typename Framework>
  Chain<Framework>::operator const typename Framework::Chain&() const
  {
    return this->impl ;
  }

  template<typename Framework>
  Chain<Framework>::operator typename Framework::Chain&()
  {
    return this->impl ;
  }
  
  template<typename Framework>
  void Chain<Framework>::advance()
  {
    this->impl.advance() ;
  }
  
  template<typename Framework>
  void Chain<Framework>::begin()
  {
    this->impl.begin() ;
  }

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
  void Chain<Framework>::combine( const nyx::Chain<Framework>& child )
  {
    this->impl.combine( child ) ;
  }
  
  template<typename Framework>
  template<typename Type, typename Type2>
  void Chain<Framework>::drawIndexed( const nyx::Renderer<Framework>& renderer, const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices )
  {
    this->impl.drawIndexed( renderer, indices, vertices ) ;
  }
  
  template<typename Framework>
  template<typename Type, typename Type2>
  void Chain<Framework>::drawInstanced( unsigned instance_count, const nyx::Renderer<Framework>& renderer, const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices )
  {
    this->impl.drawInstanced( instance_count, renderer, indices, vertices ) ;
  }

  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::drawInstanced( unsigned instance_count, const nyx::Renderer<Framework>& renderer, const Array<Framework, Type>& vertices )
  {
    this->impl.drawInstanced( instance_count, renderer, vertices ) ;
  }
      
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::draw( const nyx::Renderer<Framework>& renderer, const Array<Framework,Type>& array, unsigned offset )
  {
    this->impl.draw( renderer, array, offset ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::end()
  {
    this->impl.end() ;
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
  void Chain<Framework>::setMode( nyx::ChainMode mode )
  {
    this->impl.setMode( mode ) ;
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
  void Chain<Framework>::initialize( const nyx::RenderPass<Framework>& render_pass, ChainType type, bool multi_pass )
  {
    this->impl.initialize( render_pass, type, multi_pass ) ;
  }
  
  template<typename Framework>
  void Chain<Framework>::initialize( const nyx::RenderPass<Framework>& render_pass, unsigned window_id, bool multi_pass )
  {
    this->impl.initialize( render_pass, window_id, multi_pass ) ;
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::memoryBarrier( const nyx::Array<Framework, Type>& read, const nyx::Array<Framework, Type>& write )
  {
    this->impl.memoryBarrier( read, write ) ; 
  }
  
  template<typename Framework>
  void Chain<Framework>::pipelineBarrier( nyx::GPUStages src, nyx::GPUStages dst )
  {
    this->impl.pipelineBarrier( src, dst ) ; 
  }
  
  template<typename Framework>
  template<typename Type>
  void Chain<Framework>::memoryBarrier( const nyx::Array<Framework, Type>& read, const nyx::Image<Framework>& write )
  {
    this->impl.memoryBarrier( read, write ) ; 
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

