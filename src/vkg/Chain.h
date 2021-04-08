/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
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
    
    class Chain
    {
      public:
        Chain() ;
        ~Chain() ;
        
        void initialize( unsigned gpu, ChainType type ) ;
        
        void initialize( unsigned gpu, unsigned window_id ) ;

        void initialize( const RenderPass& pass, ChainType type ) ;
        
        void initialize( const RenderPass& pass, unsigned window_id ) ;

        template<typename Type>
        void copy( const nyx::Array<Vulkan, Type>& src, Array<Vulkan, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        template<typename Type>
        void copy( const nyx::Array<Vulkan, Type>& src, nyx::Image<Vulkan>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        template<typename Type>
        void copy( const nyx::Image<Vulkan>& src, nyx::Array<Vulkan, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        template<typename Type>
        void copy( const nyx::Array<Vulkan, Type>& src, Type* dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        template<typename Type>
        void copy( const Type* src, nyx::Array<Vulkan, Type>& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        void copy( const vkg::Image& src, vkg::Image& dst, unsigned amt = 0, unsigned src_offset = 0, unsigned dst_offset = 0 ) ;
        
        /** Method to append a draw command to this renderer.
         * @param array The array of vertices to draw.
         * @param offset The offset into the vertex array to start drawing at.
         */
        template<typename Type>
        void draw( const vkg::Renderer& renderer, const Array<Vulkan, Type>& array, unsigned offset = 0 ) ;

        /** Method to append a draw command to this renderer using indices.
         * @param indices The index array describing the render order of the vertex array.
         * @param vertices The array of vertices used for drawing.
         */
        template<typename Type, typename Type2>
        void drawIndexed( const vkg::Renderer& renderer, const Array<Vulkan, Type2>& indices, const Array<Vulkan, Type>& vertices ) ;

        template<typename Type>
        void push( const Renderer& pipeline, const Type& data, unsigned offset ) ;

        void transition( vkg::Image& image, nyx::ImageLayout layout ) ;
        
        unsigned device() const ;
        
        void synchronize() ;
        
        void submit() ;
        
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
