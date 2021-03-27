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
#include "RenderPass.h"

namespace nyx
{
  template<typename Framework>
  class Memory ;
  
  template<typename Framework, typename Type>
  class Array ;
  
  template<typename Framework>
  class RenderPass ;
  
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
  
  template<typename Framework>
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
      operator const typename Framework::Renderer&() const ;

      /** Method to initialize this object.
       * @param device The device to use for all GPU operations.
       * @param nyx_file_path The path to the .nyx file on the filesystem to use.
       */
      void initialize( unsigned device, nyx::RenderPass<Framework>& pass, const char* nyx_file_path ) ;
      
      /** Method to initialize this object.
       * @param device The device to use for all GPU operations.
       * @param nyx_file_bytes The bytes of the .nyx file to use for this object.
       * @param size The size of the bytes array.
       */
      void initialize( unsigned device, nyx::RenderPass<Framework>& pass, const unsigned char* nyx_file_bytes, unsigned size ) ;
      
      bool initialized() const ;
      
      /** Method to set the dimensions of this renderer's output attachments.
       * @param width The width in pixels of this object's output images.
       * @param height The height in pixels of this object's output images.
       */
      void setDimensions( unsigned width, unsigned height ) ;

      /** Method to add a viewport to this renderer.
       * @param viewport The viewport to add in the output of this renderer.
       */
      void addViewport( const Viewport& viewport ) ;
      
      /** Method to bind an array to one of this object's values on the GPU.
       * @param name The name associated with the value in the inputted pipeline.
       * @param array The GPU array to bind to the pipeline variable.
       */
      template<typename Type>
      void bind( const char* name, const Array<Framework, Type>& array ) ;

      /** Method to bind an image to one of this object's values on the GPU.
       * @param name The name associated with the value in the inputted pipeline.
       * @param image The GPU image to bind to the pipeline variable.
       */
      
      void bind( const char* name, const Image<Framework>& image ) ;

      /** Method to retrieve the number of framebuffers generated by this renderer.
       * @return The number of framebuffers generated by this renderer.
       */
      unsigned count() const ;
      
      /** Method to retrieve the device used by this renderer.
       * @return Const reference to the device used for this renderer.
       */
      const typename Framework::Device& device() const ;

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
      
      
    private:
      typename Framework::Renderer impl ;
  };

  template<typename Framework>
  Renderer<Framework>::operator const typename Framework::Renderer&() const
  {
    return this->impl ;
  }

  template<typename Framework>
  void Renderer<Framework>::initialize( unsigned device, nyx::RenderPass<Framework>& pass, const char* nyx_file_path )
  {
    this->impl.initialize( device, pass, nyx_file_path ) ;
  }

  template<typename Framework>
  void Renderer<Framework>::initialize( unsigned device, nyx::RenderPass<Framework>& pass, const unsigned char* nyx_file_bytes, unsigned size )
  {
    this->impl.initialize( device, pass, nyx_file_bytes, size ) ;
  }

  template<typename Framework>
  bool Renderer<Framework>::initialized() const
  {
    return this->impl.initialized() ;
  }

  template<typename Framework>
  void Renderer<Framework>::addViewport( const Viewport& viewport )
  {
    this->impl.addViewport( viewport ) ;
  }

  template<typename Framework>
  template<typename Type>
  void Renderer<Framework>::bind( const char* name, const Array<Framework, Type>& array )
  {
    this->impl.bind( name, array ) ;
  }
  
  template<typename Framework>
  void Renderer<Framework>::bind( const char* name, const Image<Framework>& image )
  {
    this->impl.bind( name, image ) ;
  }

  template<typename Framework>
  unsigned Renderer<Framework>::count() const
  {
    return this->impl.count() ;
  }

  template<typename Framework>
  const typename Framework::Device& Renderer<Framework>::device() const
  {
    return this->impl.device() ;
  }

//  template<typename Framework>
//  template<typename Type>
//  void Renderer<Framework>::draw( const Array<Framework, Type>& array, unsigned offset )
//  {
//    this->impl.draw( array, offset ) ;
//  }
//
//  template<typename Framework>
//  template<typename Type, typename Type2>
//  void Renderer<Framework>::drawIndexed( const Array<Framework, Type2>& indices, const Array<Framework, Type>& vertices )
//  {
//    this->impl.drawIndexed( indices, vertices ) ;
//  }

  template<typename Framework>
  template<typename Type>
  void Renderer<Framework>::push( const Type& value )
  {
    this->impl.push( value ) ;
  }
  
  template<typename Framework>
  template<unsigned index>
  const auto Renderer<Framework>::framebuffer() const
  {
    return this->impl.framebuffer() ;
  }
}
