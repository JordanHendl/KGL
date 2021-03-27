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
 * File:   Material.h
 * Author: jhendl
 *
 * Created on March 5, 2021, 8:55 PM
 */

#pragma once

#include "Image.h"

namespace nyx
{ 
  namespace v1
  {
    /** Template class to describe the physical material of a 3D Model.
     */
    template<typename Framework>
    class Material
    {
      public:
        
        /** Default constructor.
         */
        Material() ;
        
        /** Default deconstructor.
         */
        ~Material() ;
        
        /** Method to set the flat ambient color of this material.
         * @param x The r component.
         * @param y The g component.
         * @param z The a component.
         */
        void setAmbient( float x, float y, float z ) ;
        
        /** Method to set the flat diffuse color of this material.
         * @param x The r component.
         * @param y The g component.
         * @param z The a component.
         */
        void setDiffuse( float x, float y, float z ) ;
        
        /** Method to set the flat specular color of this material.
         * @param x The r component.
         * @param y The g component.
         * @param z The a component.
         */
        void setSpecular( float x, float y, float z ) ;
        
        /** Method to set how reflective this material is.
         * @param val The amount of reflectiveness to apply to this material.
         */
        void setReflectiveness( float val ) ;
        
        /** Method to set how shiny this material is.
         * @param val The amount of shininess this material is.
         */
        void setShininess( float val ) ;
        
        /** Method to set how transparent this material is.
         * @param val The amount of transparency this material contains.
         */
        void setTransparency( float val ) ;
        
        /** Method to set the refractiveness of this material.
         * @param val The refractiveness of this material.
         */
        void setRefractiveness( float val ) ;
        
        /** Method to retrieve the red component of the ambient color of this material.
         * @return The red component of the ambient color of this material.
         */
        float ambientR() ;
        
        /** Method to retrieve the green component of the ambient color of this material.
         * @return The green component of the ambient color of this material.
         */
        float ambientG() ;
        
        /** Method to retrieve the blue component of the ambient color of this material.
         * @return The blue component of the ambient color of this material.
         */
        float ambientB() ;
        
        /** Method to retrieve the red component of the diffuse color of this material.
         * @return The red component of the diffuse color of this material.
         */
        float diffuseR() ;
        
        /** Method to retrieve the green component of the diffuse color of this material.
         * @return The green component of the diffuse color of this material.
         */
        float diffuseG() ;
        
        /** Method to retrieve the blue component of the diffuse color of this material.
         * @return The blue component of the diffuse color of this material.
         */
        float diffuseB() ;
        
        /** Method to retrieve the red component of the specular color of this material.
         * @return The red component of the diffuse color of this material.
         */
        float specularR() ;
        
        /** Method to retrieve the green component of the specular color of this material.
         * @return The green component of the diffuse color of this material.
         */
        float specularG() ;
        
        /** Method to retrieve the blue component of the specular color of this material.
         * @return The blue component of the diffuse color of this material.
         */
        float specularB() ;
        
        /** Method to retrieve the reflectiveness of this material.
         * @return The reflectiveness of this material.
         */
        float reflectiveness() ;
        
        /** Method to retrieve the shininess of this material.
         * @return The shininess of this material.
         */
        float shininess() ;
        
        /** Method to retrieve the transparency of this material.
         * @return The transparency of this material.
         */
        float transparency() ;
        
        /** Method to retrieve the refractiveness of this material.
         * @return The refractiveness of this material.
         */
        float refractiveness() ;
        
        /** Method to retrieve the byte size of this object's flat parameters.
         * @return The size, in bytes, of this object's internal parameter structure.
         */
        unsigned byteSize() const ;
        
        /** Method to retrieve the pointer to this object's internal flat parameter structure.
         * @return Const pointer to the flat parameter structure used by this object.
         */
        const float* flat() const ;
        
      private:
        
        /** The flat parameters to use for this object.
         */
        struct Params
        {
          float ax, ay, az ;
          float dx, dy, dz ;
          float sx, sy, sz ;
          
          float reflectiveness ;
          float shininess      ;
          float roughness      ;
          float transparency   ;
          float refractivensss ;
        };
        
        /** The structure containing this object's textures to use in place of the flat parameters.
         */
        struct Textures
        {
          Image<Framework> img_ambient      ;
          Image<Framework> img_diffuse      ;
          Image<Framework> img_specular     ;
          Image<Framework> img_reflection   ;
          Image<Framework> img_shininess    ;
          Image<Framework> img_displacement ;
          Image<Framework> img_alpha        ;
          Image<Framework> img_bump         ;
          Image<Framework> img_highlight    ;
        };
  
        Params   parameters ;
        Textures textures   ; 
    };
  }
}

