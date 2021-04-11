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
 * File:   RenderPass.cpp
 * Author: jhendl
 * 
 * Created on March 21, 2021, 6:13 PM
 */

#include "RenderPass.h"

namespace nyx
{
  Attachment::Attachment()
  {
    this->r             = 0.0f                                                       ;
    this->g             = 0.0f                                                       ;
    this->b             = 0.0f                                                       ;
    this->a             = 1.0f                                                       ;
    this->store_stencil = false                                                      ;
    this->test_stencil  = false                                                      ;
    this->clear_stencil = false                                                      ;
    this->img_format    = static_cast<unsigned>( nyx::ImageFormat::RGBA8           ) ;
    this->img_layout    = static_cast<unsigned>( nyx::ImageLayout::ColorAttachment ) ;
  }
  
  Attachment::~Attachment()
  {
    
  }
  
  void Attachment::setClearColor( float red, float green, float blue, float alpha )
  {
    this->r = red   ;
    this->g = green ;
    this->b = blue  ;
    this->a = alpha ;
  }

  void Attachment::setStencilTest( bool value )
  {
    this->test_stencil = value ;
  }

  void Attachment::setStencilClear( bool value )
  {
    this->clear_stencil = value ;
  }

  void Attachment::setStencilStore( bool value )
  {
    this->store_stencil = value ;
  }

  void Attachment::setFormat( nyx::ImageFormat format )
  {
    this->img_format = static_cast<unsigned>( format ) ;
  }

  void Attachment::setLayout( nyx::ImageLayout layout )
  {
    this->img_layout = static_cast<unsigned>( layout ) ;
  }

  bool Attachment::clearStencil() const
  {
    return this->clear_stencil ;
  }

  bool Attachment::testStencil() const
  {
    return this->test_stencil ;
  }

  bool Attachment::storeStencil() const
  {
    return this->store_stencil ;
  }

  nyx::ImageFormat Attachment::format() const
  {
    return static_cast<nyx::ImageFormat>( this->img_format ) ;
  }

  nyx::ImageLayout Attachment::layout() const
  {
    return static_cast<nyx::ImageLayout>( this->img_layout ) ;
  }

  float Attachment::red() const
  {
    return this->r ;
  }

  float Attachment::green() const
  {
    return this->g ;
  }

  float Attachment::blue() const
  {
    return this->b ;
  }

  float Attachment::alpha() const
  {
    return this->a ;
  }
}
      
