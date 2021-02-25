/*
 * Copyright (C) 2021 jhendl
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
 * File:   Renderer.h
 * Author: jhendl
 *
 * Created on February 7, 2021, 11:28 AM
 */

#pragma once

#include "Image.h"


namespace vk
{
  class SurfaceKHR ;
}

namespace nyx
{
  /**
   */
  enum class ImageFormat : unsigned ;
  
  /**
   */
  enum class PipelineStage : unsigned ;

  /**
   */
  class Viewport ;
  
  template<typename Impl, nyx::ImageFormat Format>
  class Image ;
  
  /** Forward decleration so the template interface can access this object's functionality.
   */
  template<typename Impl, typename Type>
  class Array ;

  /**
   */
  template<typename Framework, nyx::ImageFormat ... Formats>
  class Renderer ;

  namespace vkg
  {
    using Surface = vk::SurfaceKHR ;

    class Vulkan ;
    
    class Buffer ;
    
    class Image ;
    
    template<nyx::ImageFormat ... Formats>
    class Renderer ;
    
    /* Forward declared implmentation.
     */
    class RendererImpl ;

    template<typename Format, typename ... Formats>
    constexpr nyx::ImageFormat format( unsigned desired, unsigned curr, Format format, Formats ... formats ) ;
    
    template<typename Format>
    constexpr nyx::ImageFormat format( unsigned desired, unsigned curr, Format format ) ;
    
    template<typename Format, typename ... Formats>
    void addFormat( RendererImpl& impl, Format format, Formats ... formats ) ;
    
    template<typename Format>
    void addFormat( RendererImpl& impl, Format format ) ;
    
    class RendererImpl
    {
      private:
        template<nyx::ImageFormat ... Formats>
        friend class Renderer ;
        
        template<typename Format, typename ... Formats>
        friend void addFormat( RendererImpl& impl, Format format, Formats ... formats ) ;
        
        template<typename Format>
        friend void addFormat( RendererImpl& impl, Format format ) ;

        /** Default constructor.
         */
        RendererImpl() ;
        
        /** Default deconstructor.
         */
        ~RendererImpl() ;

        /** Method to add an attachment to this renderer.
         * @param format The format of attachment to add.
         */
        void addAttachment( nyx::ImageFormat format ) ;

        /** Method to add a viewport to this renderer.
         * @param viewport The viewport to add in the output of this renderer.
         */
        void addViewport( const nyx::Viewport& viewport ) ;

        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_path The path to the .nyx file on the filesystem to use.
         */
        void initialize( unsigned device, const char* nyx_file_path ) ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_path The path to the .nyx file on the filesystem to use.
         * @param context The Framework context for this renderer to draw to.
         */
        void initialize( unsigned device, const char* nyx_file_path, const vkg::Surface& context ) ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_bytes The bytes of the .nyx file to use for this object.
         * @param size The size of the bytes array.
         */
        void initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size ) ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_bytes The bytes of the .nyx file to use for this object.
         * @param size The size of the bytes array.
         * @param context The Framework context for this renderer to draw to.
         */
        void initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const vkg::Surface& context ) ;

        /** Method to bind an array to one of this object's values on the GPU.
         * @param name The name associated with the value in the inputted pipeline.
         * @param buffer The vkg buffer to bind.
         * @param element_size The size of each element.
         * @param count The amount of elements.
         */
        void bind( const char* name, const nyx::vkg::Buffer& buffer ) ;
        
        /** Method to bind an image to one of this object's values on the GPU.
         * @param name The name associated with the value in the inputted pipeline.
         * @param image The vkg image to bind.
         */
        void bind( const char* name, const nyx::vkg::Image& image ) ;
        
        /** Method to retrieve the number of framebuffers generated by this renderer.
         * @return The number of framebuffers generated by this renderer.
         */
        unsigned count() const ;
        
        /** Method to retrieve the current framebuffer of this object that is to be drawn to next.
         * @return The index of the current framebuffer that is being drawn to next by this object.
         */
        unsigned current() const ;
  
        /** Method to retrieve the device used by this renderer.
         * @return Const reference to the device used for this renderer.
         */
        unsigned device() const ;
        
        /** Base method to use a buffer as vertices to draw.
         * @param buffer The buffer to use for vertices.
         * @param count The amount of vertices in the buffer.
         * @param offset The offset of the buffer to start at.
         */
        void drawBase( const nyx::vkg::Buffer& buffer, unsigned count, unsigned offset = 0 ) ;\
        
        /** Method to retrieve the format of the specified attachment index.
         * @param index The index of attachment to retrieve the format of.
         */
        nyx::ImageFormat format( unsigned index = 0 ) const ;

        /** Method to retrieve a framebuffer from this object.
         * @param index The index of framebuffer to retrieve. See @count for the amount.
         * @return Const reference to this object's internal framebuffer.
         */
        const vkg::Image& image( unsigned index = 0 ) const ;

        /** Base method to use a buffer as vertices to draw.
         * @param index The buffer to use for indices.
         * @param vert The buffer to use for vertices.
         * @param index_count The amount of indices in the buffer.
         * @param vert_count The amount of vertices in the buffer.
         * @param offset The offset of the buffer to start at.
         */
        void drawIndexedBase( const nyx::vkg::Buffer& index, const nyx::vkg::Buffer& vert, unsigned index_count, unsigned vert_count, unsigned offset = 0 ) ;

        /** Private method for pushing a value as a push-constant to this command buffer.
         * @param value The pointer value to push onto the Device.
         * @param byte_size The size in bytes of the object being pushed.
         * @param stage_flags The stage that the push constant is used on.
         */
        void pushConstantBase( const void* value, unsigned byte_size, nyx::PipelineStage stage_flags ) ;

        /** Method to reset this object and deallocate all allocated data.
         */
        void reset() ;
        
        /** Method to set the dimensions of this renderer.
         * @param width The width of this renderer's targets in pixels.
         * @param height The height of this renderer's targets in pixels.
         */
        void setDimensions( unsigned width, unsigned height ) ;

        /** Method to finalize this renderer's operations. Actually performs the recorded operations.
         * @note If this renderer has the context when initialized, this presents to the input window as well.
         */
        void finalize() ;

        /** Forward-declared structure to contain this object's internal data.
         */
        struct RendererData* renderer_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        RendererData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const RendererData& data() const ;
    };

    template<nyx::ImageFormat ... Formats>
    class Renderer
    {
      private:
        
        /** Friend decleration so the template interface can access this object's functionality.
         */
        template<typename Impl, nyx::ImageFormat ... Formats2>
        friend class nyx::Renderer ;

        /** Default constructor.
         */
        Renderer() ;
  
        /** Default deconstructor.
         */
        ~Renderer() ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_path The path to the .nyx file on the filesystem to use.
         */
        void initialize( unsigned device, const char* nyx_file_path ) ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_path The path to the .nyx file on the filesystem to use.
         * @param context The Framework context for this renderer to draw to.
         */
        void initialize( unsigned device, const char* nyx_file_path, const vkg::Surface& context ) ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_bytes The bytes of the .nyx file to use for this object.
         * @param size The size of the byte array.
         */
        void initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size ) ;
  
        /** Method to initialize this object.
         * @param device The device to use for all GPU operations.
         * @param nyx_file_bytes The bytes of the .nyx file to use for this object.
         * @param size The size of the byte array.
         * @param context The Framework context for this renderer to draw to.
         */
        void initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const vkg::Surface& context ) ;
  
        /** Method to add a viewport to this renderer.
         * @param viewport The viewport to add in the output of this renderer.
         */
        void addViewport( const nyx::Viewport& viewport ) ;
        
        /** Method to bind an array to one of this object's values on the GPU.
         * @param name The name associated with the value in the inputted pipeline.
         * @param array The GPU array to bind to the pipeline variable.
         */
        template<typename Type>
        void bind( const char* name, const nyx::Array<vkg::Vulkan, Type>& array ) ;
        
        /** Method to bind an image to one of this object's values on the GPU.
         * @param name The name associated with the value in the inputted pipeline.
         * @param image The GPU image to bind to the pipeline variable.
         */
        template<nyx::ImageFormat Format>
        void bind( const char* name, const nyx::Image<vkg::Vulkan, Format>& image ) ;
        
        /** Method to retrieve the number of framebuffers generated by this renderer.
         * @return The number of framebuffers generated by this renderer.
         */
        unsigned count() const ;
  
        /** Method to retrieve the current framebuffer of this object that is to be drawn to next.
         * @return The index of the current framebuffer that is being drawn to next by this object.
         */
        unsigned current() const ;
  
        /** Method to retrieve the device used by this renderer.
         * @return Const reference to the device used for this renderer.
         */
        unsigned device() const ;
  
        /** Method to append a draw command to this renderer.
         * @param array The array of vertices to draw.
         * @param offset The offset into the vertex array to start drawing at.
         */
        template<typename Type>
        void draw( const nyx::Array<vkg::Vulkan, Type>& array, unsigned offset = 0 ) ;
  
        /** Method to append a draw command to this renderer using indices.
         * @param indices The index array describing the render order of the vertex array.
         * @param vertices The array of vertices used for drawing.
         */
        template<typename Type, typename Type2>
        void drawIndexed( const nyx::Array<vkg::Vulkan, Type2>& indices, const nyx::Array<vkg::Vulkan, Type>& vertices ) ;
        
        /**  Method to push a value straight onto the pipeline of this object, if it's available.
         * @param value The value to copy up to the GPU for this pipeline's operation.
         */
        template<typename Type>
        void push( const Type& value ) ;
  
        /** Method to retrieve a framebuffer from this object.
         * @param index The index of framebuffer to retrieve. See @count for the amount.
         * @return Const reference to this object's internal framebuffer.
         */
        template<unsigned index = 0>
        const auto framebuffer() const ;
        
        /** Method to finalize this renderer's operations. Actually performs the recorded operations.
         * @note If this renderer has the context when initialized, this presents to the input window as well.
         */
        void finalize() ;

        /** Method to set the dimensions of this renderer.
         * @param width The width of this renderer's targets in pixels.
         * @param height The height of this renderer's targets in pixels.
         */
        void setDimensions( unsigned width, unsigned height ) ;

        /** The underlying implementation of this object.
         */
        RendererImpl impl ;
    };
    
    template<typename Format, typename ... Formats>
    void addFormat( RendererImpl& impl, Format format, Formats ... formats )
    {
      impl.addAttachment( format ) ;
      
      addFormat( impl, formats... ) ;
    }
    
    template<typename Format>
    void addFormat( RendererImpl& impl, Format format )
    {
      impl.addAttachment( format ) ;
    }

    template<typename Format, typename ... Formats>
    constexpr nyx::ImageFormat format( unsigned desired, unsigned curr, Format format, Formats ... formats )
    {
      if( desired != curr ) return vkg::format( desired, curr++, formats... ) ;
      
      return static_cast<nyx::ImageFormat>( format ) ;
    }
    
    template<typename Format>
    constexpr nyx::ImageFormat format( unsigned desired, unsigned curr, Format format )
    {
      if( desired == curr )
      return static_cast<nyx::ImageFormat>( format ) ;
      
      return static_cast<nyx::ImageFormat>( 0 ) ;
    }

    template<nyx::ImageFormat ... Formats>
    Renderer<Formats...>::Renderer()
    {
      nyx::vkg::addFormat( this->impl, Formats... ) ;
    }

    template<nyx::ImageFormat ... Formats>
    Renderer<Formats...>::~Renderer()
    {
      this->impl.reset() ;
    }

    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::initialize( unsigned device, const char* nyx_file_path )
    {
      this->impl.initialize( device, nyx_file_path ) ;
    }

    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::initialize( unsigned device, const char* nyx_file_path, const vkg::Surface& context )
    {
      this->impl.initialize( device, nyx_file_path, context ) ;
    }

    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size )
    {
      this->impl.initialize( device, nyx_file_bytes, size ) ;
    }

    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const vkg::Surface& context )
    {
      this->impl.initialize( device, nyx_file_bytes, size, context ) ;
    }

    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::addViewport( const nyx::Viewport& viewport )
    {
      this->impl.addViewport( viewport ) ;
    }

    template<nyx::ImageFormat ... Formats>
    template<typename Type>
    void Renderer<Formats...>::bind( const char* name, const nyx::Array<vkg::Vulkan, Type>& array )
    {
      this->impl.bind( name, array ) ;
    }
    
    template<nyx::ImageFormat ... Formats>
    template<nyx::ImageFormat Format>
    void Renderer<Formats...>::bind( const char* name, const nyx::Image<vkg::Vulkan, Format>& image )
    {
      this->impl.bind( name, image ) ;
    }

    template<nyx::ImageFormat ... Formats>
    unsigned Renderer<Formats...>::count() const
    {
      return this->impl.count() ;
    }

    template<nyx::ImageFormat ... Formats>
    unsigned Renderer<Formats...>::current() const
    {
      return this->impl.current() ;
    }

    template<nyx::ImageFormat ... Formats>
    unsigned Renderer<Formats...>::device() const
    {
      return this->impl.device() ;
    }

    template<nyx::ImageFormat ... Formats>
    template<typename Type>
    void Renderer<Formats...>::draw( const nyx::Array<vkg::Vulkan, Type>& array, unsigned offset )
    {
      this->impl.drawBase( array, array.size(), offset ) ;
    }

    template<nyx::ImageFormat ... Formats>
    template<typename Type, typename Type2>
    void Renderer<Formats...>::drawIndexed( const nyx::Array<vkg::Vulkan, Type2>& indices, const nyx::Array<vkg::Vulkan, Type>& vertices )
    {
      this->impl.drawIndexedBase( indices, vertices, indices.size(), vertices.size() ) ; ;
    }

    template<nyx::ImageFormat ... Formats>
    template<typename Type>
    void Renderer<Formats...>::push( const Type& value )
    {
      this->impl.pushConstantBase( static_cast<const void*>( &value ), sizeof( Type ), static_cast<nyx::PipelineStage>( 0 ) ) ;
    }

    template<nyx::ImageFormat ... Formats>
    template<unsigned index>
    const auto Renderer<Formats...>::framebuffer() const
    {
      constexpr auto format = vkg::format( index, 0, Formats... ) ;
      return static_cast< nyx::Image<vkg::Vulkan, format> >( impl.image( index ) ) ;
    }
    
    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::finalize()
    {
      this->impl.finalize() ;
    }
    
    template<nyx::ImageFormat ... Formats>
    void Renderer<Formats...>::setDimensions( unsigned width, unsigned height )
    {
      this->impl.setDimensions( width, height ) ;
    }
  }
}


