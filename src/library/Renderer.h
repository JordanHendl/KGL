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
 * File:   Renderer.h
 * Author: jhendl
 *
 * Created on February 6, 2021, 3:43 PM
 */

#pragma once

#include "Image.h"
#include "Array.h"

namespace nyx
{
  enum class PipelineStage : unsigned
  {
    Vertex,
    Fragment,
    Compute,
    TessC,
  };
  
  template<typename Impl>
  class Memory ;
  
  template<typename Impl, typename Type>
  class Array ;
  
  /** Class that describes a viewport.
   */
  class Viewport
  {
    public:
      
      /** Default constructor.
       */
      Viewport() ;
      
      /** Default copy constructor.
       * @param port The object to copy.
       */
      Viewport( const Viewport& port ) = default ;
      
      /** Default deconstructor.
       */
      ~Viewport() = default ;
      
      /** Defaulted assignment operator.
       */
      Viewport& operator=( const Viewport& port ) = default ;
      
      /** Method to set the X-position in screen space of this object.
       * @param value The x-position to set.
       */
      void setXPos( unsigned value ) ;
      
      /** Method to set the Y-position in screen space of this object.
       * @param value The y-position to set.
       */
      void setYPos( unsigned value ) ;
      
      /** Method to set the width of this object.
       * @param value The value to use for the width of this object.
       */
      void setWidth( unsigned value ) ;

      /** Method to set the heights of this object.
       * @param value The value to use for the height of this object.
       */
      void setHeight( unsigned value ) ;
      
      /** Method to set the max depth of this object.
       * @param depth The max depth value for this object.
       */
      void setMaxDepth( float depth ) ;
      
      /** Method to retrieve the X-position of this object.
       * @return The X-position of this object.
       */
      unsigned xpos() const ;
      
      /** Method to retrieve the Y-position of this object.
       * @return The Y-position of this object.
       */
      unsigned ypos() const ;
      
      /** Method to retrieve the width of this object.
       * @return The width of this object.
       */
      unsigned width() const ;
      
      /** Method to retrieve the height of this object.
       * @return The height of this object.
       */
      unsigned height() const ;
      
      /** Method to retrieve the max possible depth of this object.
       * @return The max possible depth of this object.
       */
      float maxDepth() const ;
    private:
      unsigned t_xpos      ;
      unsigned t_ypos      ;
      unsigned t_width     ;
      unsigned t_height    ;
      float    t_max_depth ;
  };
  
  template<typename Framework, nyx::ImageFormat ... Formats>
  class Renderer
  {
    public:
      /** Default constructor.
       */
      Renderer() = default ;
      
      /** Default deconstructor.
       */
      ~Renderer() = default ;
      
      /** Method to convert this object to the underlying implementation-specific version.
       * @return The implementation-specific version of this object.
       */
      operator const typename Framework::template Renderer<Formats...>&() const ;

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
      void initialize( unsigned device, const char* nyx_file_path, const typename Framework::Context& context ) ;
      
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
      void initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const typename Framework::Context& context ) ;
      
      /** Method to set the dimensions of this renderer's output attachments.
       * @param width The width in pixels of this object's output images.
       * @param height The height in pixels of this object's output images.
       */
      void setDimensions( unsigned width, unsigned height ) ;

      /** Method to add a viewport to this renderer.
       * @param viewport The viewport to add in the output of this renderer.
       */
      void addViewport( const nyx::Viewport& viewport ) ;
      
      /** Method to bind an array to one of this object's values on the GPU.
       * @param name The name associated with the value in the inputted pipeline.
       * @param array The GPU array to bind to the pipeline variable.
       */
      template<typename Type>
      void bind( const char* name, const nyx::Array<Framework, Type>& array ) ;

      /** Method to bind an image to one of this object's values on the GPU.
       * @param name The name associated with the value in the inputted pipeline.
       * @param image The GPU image to bind to the pipeline variable.
       */
      template<nyx::ImageFormat Format>
      void bind( const char* name, const nyx::Image<Framework, Format>& image ) ;

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
      const typename Framework::Device& device() const ;
      
      /** Method to append a draw command to this renderer.
       * @param array The array of vertices to draw.
       * @param offset The offset into the vertex array to start drawing at.
       */
      template<typename Type>
      void draw( const nyx::Array<Framework, Type>& array, unsigned offset = 0 ) ;

      /** Method to append a draw command to this renderer using indices.
       * @param indices The index array describing the render order of the vertex array.
       * @param vertices The array of vertices used for drawing.
       */
      template<typename Type, typename Type2>
      void drawIndexed( const nyx::Array<Framework, Type2>& indices, const nyx::Array<Framework, Type>& vertices ) ;

      /**  Method to push a value straight onto the pipeline of this object, if it's available.
       * @param value The value to copy up to the GPU for this pipeline's operation.
       */
      template<typename Type>
      void push( const Type& value ) ;
      
      /** Method to finalize this renderer's operations. Actually performs the recorded operations.
       * @note If this renderer has the context when initialized, this presents to the input window as well.
       */
      void finalize() ;

      /** Method to retrieve a framebuffer from this object.
       * @param index The index of framebuffer to retrieve. See @count for the amount.
       * @return Const reference to this object's internal framebuffer.
       */
      template<unsigned index = 0>
      const auto framebuffer() const ;
      
      
    private:
      typename Framework::template Renderer<Formats...> impl ;
  };

  template<typename Framework, nyx::ImageFormat ... Formats>
  Renderer<Framework, Formats...>::operator const typename Framework::template Renderer<Formats...>&() const
  {
    return this->impl ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::initialize( unsigned device, const char* nyx_file_path )
  {
    this->impl.initialize( device, nyx_file_path ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::initialize( unsigned device, const char* nyx_file_path, const typename Framework::Context& context )
  {
    this->impl.initialize( device, nyx_file_path, context ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size )
  {
    this->impl.initialize( device, nyx_file_bytes, size ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::initialize( unsigned device, const unsigned char* nyx_file_bytes, unsigned size, const typename Framework::Context& context )
  {
    this->impl.initialize( device, nyx_file_bytes, size, context ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::addViewport( const nyx::Viewport& viewport )
  {
    this->impl.addViewport( viewport ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  template<typename Type>
  void Renderer<Framework, Formats...>::bind( const char* name, const nyx::Array<Framework, Type>& array )
  {
    this->impl.bind( name, array ) ;
  }
  
  template<typename Framework, nyx::ImageFormat ... Formats>
  template<nyx::ImageFormat Format>
  void Renderer<Framework, Formats...>::bind( const char* name, const nyx::Image<Framework, Format>& image )
  {
    this->impl.bind( name, image ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  unsigned Renderer<Framework, Formats...>::count() const
  {
    return this->impl.count() ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  unsigned Renderer<Framework, Formats...>::current() const
  {
    return this->impl.current() ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  const typename Framework::Device& Renderer<Framework, Formats...>::device() const
  {
    return this->impl.device() ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  template<typename Type>
  void Renderer<Framework, Formats...>::draw( const nyx::Array<Framework, Type>& array, unsigned offset )
  {
    this->impl.draw( array, offset ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  template<typename Type, typename Type2>
  void Renderer<Framework, Formats...>::drawIndexed( const nyx::Array<Framework, Type2>& indices, const nyx::Array<Framework, Type>& vertices )
  {
    this->impl.drawIndexed( indices, vertices ) ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  template<typename Type>
  void Renderer<Framework, Formats...>::push( const Type& value )
  {
    this->impl.push( value ) ;
  }
  
  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::finalize()
  {
    this->impl.finalize() ;
  }
  
  template<typename Framework, nyx::ImageFormat ... Formats>
  template<unsigned index>
  const auto Renderer<Framework, Formats...>::framebuffer() const
  {
    return this->impl.framebuffer() ;
  }

  template<typename Framework, nyx::ImageFormat ... Formats>
  void Renderer<Framework, Formats...>::setDimensions( unsigned width, unsigned height )
  {
    this->impl.setDimensions( width, height ) ;
  }
}