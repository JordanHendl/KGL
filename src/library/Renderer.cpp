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
 * File:   Renderer.cpp
 * Author: jhendl
 * 
 * Created on February 6, 2021, 3:43 PM
 */

#include "Renderer.h"

namespace nyx
{
  Viewport::Viewport()
  {
    this->t_xpos      = 0    ;
    this->t_ypos      = 0    ;
    this->t_width     = 1280 ;
    this->t_height    = 1024 ;
    this->t_max_depth = 1.0f ;
  }
 
  void Viewport::setXPos( unsigned value )
  {
    this->t_xpos = value ;
  }
 
  void Viewport::setYPos( unsigned value )
  {
    this->t_ypos = value ;
  }
 
  void Viewport::setWidth( unsigned value )
  {
    this->t_width = value ;
  }
 
  void Viewport::setHeight( unsigned value )
  {
    this->t_height = value ;
  }
 
  void Viewport::setMaxDepth( float depth )
  {
    this->t_max_depth = depth ;
  }
 
  unsigned Viewport::xpos() const
  {
    return this->t_xpos ;
  }
 
  unsigned Viewport::ypos() const
  {
    return this->t_ypos ;
  }
 
  unsigned Viewport::width() const
  {
    return this->t_width ;
  }
 
  unsigned Viewport::height() const
  {
    return this->t_height ;
  }
 
  float Viewport::maxDepth() const
  {
    return this->t_max_depth ;
  }
}
