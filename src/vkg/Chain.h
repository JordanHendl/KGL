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

/* Declarations for the Vulkan GPU implementation of a Chain, an object to handle recording GPU operations, submitting GPU operation, and synchronizing them.
 * File:   Chain.h
 * Author: jhendl
 *
 * Created on March 19, 2021, 9:42 PM
 */

#pragma once

namespace nyx
{
  enum class ChainType : unsigned ;
  template<typename Framework, typename Type>
  class Array ;
  
  template<typename Framework>
  class Image ;
  
  template<typename Framework>
  class Renderer ;
  
  enum class PipelineStage : unsigned ;
  enum class ImageLayout : unsigned ;

  namespace vkg
  {
    class Buffer     ;
    class Image      ;
    class Vulkan     ;
    class Renderer   ;
    class RenderPass ;
    
    /** Class to handle recording operations to perform on the GPU.
     */
    class Chain
    {
      public:
        /** Default constructor.
         */
        Chain() ;
        
        /** Default deconstructor.
         */
        ~Chain() ;
        
        /** Method to initialize this object with the given parameters.
         * @param gpu The ID of gpu to use.
         * @param type The type of queue to use for this chain.
         */
        void initialize( unsigned gpu, ChainType type ) ;
        
        /** Method to initialize this object with the given parameters.
         * @param gpu The ID of gpu to use.
         * @param window_id The id associated with the window to use for this object.
         */
        void initialize( unsigned gpu, unsigned window_id ) ;

        /** Method to initialize this object with the given parameters.
         * @param pass The render pass to use for this object's commands.
         * @param type The type of queue to use for this chain.
         */
        void initialize( const RenderPass& pass, ChainType type ) ;
        
        /** Method to initialize this object with the given parameters.
         * @param pass The render pass to use for this object's commands.
         * @param window_id The id associated with the window to use for this object.
         */
        void initialize( const RenderPass& pass, unsigned window_id ) ;
        
        /** Method to record a copy two library arrays to eachother.
         * @param src The array to copy from.
         * @param dst The array to copy to.
         * @param amt The amount of elements to copy. Defaults to entire size.
         * @param src_offset The offset of the first array to copy from.
         * @param dst_offset The offset of the second array to copy to.
         */
        template<typename Type>
        void copy( const nyx::Array<Vulkan, Type>& src, Array<Vulkan, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to record a copy between a library array and a library image.
         * @param src The array to copy from.
         * @param dst The image to copy to.
         * @param amt The amount of data from the array to copy. Defaults to entire size.
         * @param src_offset The offset of the array to copy from.
         * @param dst_offset The offset of the image to copy to.
         */
        template<typename Type>
        void copy( const nyx::Array<Vulkan, Type>& src, nyx::Image<Vulkan>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to record a copy from a library image to a library array.
         * @param src The image to copy from.
         * @param dst The array to copy to.
         * @param amt The amount of data from the image to copy from. Defaults to entire image.
         * @param src_offset The offset of the image to copy from.
         * @param dst_offset The offset of the array to copy to.
         */
        template<typename Type>
        void copy( const nyx::Image<Vulkan>& src, nyx::Array<Vulkan, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to record a copy from a library array to a host data pointer.
         * @param src The array to copy from.
         * @param dst The host data pointer to copy data to.
         * @param amt The amount of data to copy. Defaults to the entire array size.
         * @param src_offset The offset of the source to copy from.
         * @param dst_offset The offset of the destination to copy to.
         */
        template<typename Type>
        void copy( const nyx::Array<Vulkan, Type>& src, Type* dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to record a copy from a host data pointer to a library array.
         * @param src The host data pointer to copy from.
         * @param dst The library array to copy to.
         * @param amt The amount of data to copy. Defaults to entire size.
         * @param src_offset The offset of the source to copy from.
         * @param dst_offset The offset of the destination to copy to.
         */
        template<typename Type>
        void copy( const Type* src, nyx::Array<Vulkan, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to record a copy from library image to library image.
         * @param src The source image to copy from.
         * @param dst The destination image to copy to.
         * @param amt The amount of data to copy. Defaults to entire size.
         * @param src_offset The offset of the source to copy from.
         * @param dst_offset The offset of the destination to copy to.
         */
        void copy( const vkg::Image& src, vkg::Image& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to append a draw command to this object.
         * @param array The array of vertices to draw.
         * @param offset The offset into the vertex array to start drawing at.
         */
        template<typename Type>
        void draw( const vkg::Renderer& renderer, const Array<Vulkan, Type>& array, unsigned offset = 0 ) ;

        /** Method to append a draw command to this object using indices.
         * @param indices The index array describing the render order of the vertex array.
         * @param vertices The array of vertices used for drawing.
         */
        template<typename Type, typename Type2>
        void drawIndexed( const vkg::Renderer& renderer, const Array<Vulkan, Type2>& indices, const Array<Vulkan, Type>& vertices ) ;

        /** Method to push a variable onto the input pipeline.
         * @param pipeline The pipeline to push the data to.
         * @param data The data to push.
         * @param offset The offset, in bytes, to update the data in the pipeline. Defaults to 0.
         */
        template<typename Type>
        void push( const Renderer& pipeline, const Type& data, unsigned offset ) ;
        
        /** Method to record an image transition operation.
         * @param image The image to transition.
         * @param layout The layout of the image to transition to.
         */
        void transition( vkg::Image& image, nyx::ImageLayout layout ) ;
        
        /** Method to retrieve the device used by this object.
         * @return 
         */
        unsigned device() const ;
        
        /** Method to synchronize this object's operations with the device.
         */
        void synchronize() ;
        
        /** Method to submit this object's work to the device.
         */
        void submit() ;
        
        /** Method to reset this object's data.
         */
        void reset() ;
        
      private:
        
        void drawBase( const vkg::Renderer& renderer, const vkg::Buffer& vertices, unsigned count, unsigned offset ) ;
        void drawIndexedBase( const vkg::Renderer& renderer, const vkg::Buffer& indices, unsigned index_count, const vkg::Buffer& vertices, unsigned vertex_count ) ;
        
        void copy( const vkg::Buffer& src, vkg::Buffer& dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset ) ;

        void copy( const void* src, vkg::Buffer& dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset ) ;
        
        void copy( const void* src, vkg::Image& dst ) ;
        
        void copy( const vkg::Buffer& src, void* dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset ) ;

        void copy( const vkg::Image& src, void* dst ) ;
        
        void copy( const vkg::Image& src, vkg::Buffer& dst, unsigned amt, unsigned src_offset, unsigned dst_offset ) ;
        
        void copy( const vkg::Buffer& src, vkg::Image& dst, unsigned copy_amt, unsigned element_size, unsigned src_offset, unsigned dst_offset ) ;
        
        void pushBase( const Renderer& pipeline, const void* value, unsigned byte_size, unsigned offset ) ;
        
        struct ChainData* chain_data ;
        
        const ChainData& data() const ;

        ChainData& data() ;
    };
    
    template<typename Type>
    void Chain::copy( const nyx::Array<Vulkan, Type>& src, Array<Vulkan, Type>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
    {
      this->copy( src, dst, amt, sizeof( Type ), src_offset, dst_offset ) ;
    }
    
    template<typename Type>
    void Chain::copy( const nyx::Array<Vulkan, Type>& src, nyx::Image<Vulkan>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
    {
      this->copy( src, dst, amt, sizeof( Type ), src_offset, dst_offset ) ;
    }
    
    template<typename Type>
    void Chain::copy( const nyx::Image<Vulkan>& src, nyx::Array<Vulkan, Type>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
    {
      this->copy( src, dst, amt, src_offset, dst_offset ) ;
    }
    
    template<typename Type>
    void Chain::copy( const nyx::Array<Vulkan, Type>& src, Type* dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
    {
      this->copy( src, dst, amt, sizeof( Type ), src_offset, dst_offset ) ;
    }
    
    template<typename Type>
    void Chain::copy( const Type* src, nyx::Array<Vulkan, Type>& dst, unsigned amt, unsigned src_offset, unsigned dst_offset )
    {
      this->copy( src, dst, amt, sizeof( Type ), src_offset, dst_offset ) ;
    }
    
    template<typename Type>
    void Chain::draw( const vkg::Renderer& renderer, const Array<Vulkan, Type>& array, unsigned offset )
    {
      this->drawBase( renderer, array, array.size(), offset ) ;
    }
    
    template<typename Type, typename Type2>
    void Chain::drawIndexed( const vkg::Renderer& renderer, const Array<Vulkan, Type2>& indices, const Array<Vulkan, Type>& vertices )
    {
      this->drawIndexedBase( renderer, indices, indices.size(), vertices, vertices.size() ) ;
    }
    
    template<typename Type>
    void Chain::push( const Renderer& pipeline, const Type& data, unsigned offset )
    {
      this->pushBase( pipeline, static_cast<const void*>( &data ), sizeof( Type ), offset ) ;
    }
  }
}
