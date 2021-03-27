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
 * File:   Material.cpp
 * Author: jhendl
 * 
 * Created on March 5, 2021, 8:55 PM
 */

#include "Material.h"

namespace nyx
{
  Material::Material()
  {
    
  }

  Material::~Material()
  {
  
  }

  void Material::setAmbient( float x, float y, float z )
  {
    this->parameters.ax = x ;
    this->parameters.ay = y ;
    this->parameters.az = z ;
  }

  void Material::setDiffuse( float x, float y, float z )
  {
    this->parameters.dx = x ;
    this->parameters.dy = y ;
    this->parameters.dz = z ;
  }

  void Material::setSpecular( float x, float y, float z )
  {
    this->parameters.sx = x ;
    this->parameters.sy = y ;
    this->parameters.sz = z ;
  }

  void Material::setReflectiveness( float val )
  {
    this->parameters.reflectiveness = val ;
  }

  void Material::setShininess( float val )
  {
    this->parameters.shininess = val ;
  }

  void Material::setTransparency( float val )
  {
    this->parameters.transparency = val ;
  }

  void Material::setRefractiveness( float val )
  {
    this->parameters.refractivensss = val ;
  }

  float Material::ambientR()
  {
    return this->parameters.ax ;
  }

  float Material::ambientG()
  {
    return this->parameters.ay ;
  }

  float Material::ambientB()
  {
    return this->parameters.az ;
  }

  float Material::diffuseR()
  {
    return this->parameters.dx ;
  }

  float Material::diffuseG()
  {
    return this->parameters.dy ;
  }

  float Material::diffuseB()
  {
    return this->parameters.dz ;
  }

  float Material::specularR()
  {
    return this->parameters.sx ;
  }

  float Material::specularG()
  {
    return this->parameters.sy ;
  }

  float Material::specularB()
  {
    return this->parameters.sz ;
  }

  float Material::reflectiveness()
  {
    return this->parameters.reflectiveness ;
  }

  float Material::shininess()
  {
    return this->parameters.shininess ;
  }

  float Material::transparency()
  {
    return this->parameters.transparency ;
  }

  float Material::refractiveness()
  {
    return this->parameters.refractivensss ;
  }
  
  unsigned Material::byteSize() const
  {
    return sizeof( Material::Params ) ;
  }

  const float* Material::flat() const
  {
    return reinterpret_cast<const float*>( &this->parameters ) ;
  }
}