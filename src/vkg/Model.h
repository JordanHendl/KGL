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
 * File:   Model.h
 * Author: jhendl
 *
 * Created on April 16, 2021, 11:53 PM
 */

#pragma once
#include "loaders/NggFile.h"
#include "library/Array.h"
#include "library/Chain.h"
#include "library/Renderer.h"
#include <vector>
#include <string>
#include <iostream>

namespace mars
{
  template<typename Framework>
  class Model ;
  
  /** Mesh class. Defines what a mesh is.
   */
  template<typename Framework>
  class Mesh
  {
    template<typename Framework1>
    friend class Model ;

    std::string                                 name     ;
    nyx::Array<Framework, nyx::NggFile::Vertex> vertices ;
    nyx::Array<Framework, unsigned            > indices  ;
  };

  /** Template model class. Acts as a 3D model, and provides functionality for drawing.
   */
  template<typename Framework>
  class Model
  {
    public:
      
      /** Constructor.
       */
      Model() ;
      
      /** Method to initialize this object.
       * @param model_path The path to the .ngg file on disk to load.
       * @param gpu the gpu to allocate the model on.
       */
      inline void initialize( const char* ngg_path, unsigned gpu ) ;
      
      /** Method to initialize this object.
       * @param bytes The bytes containing the .ngg file.
       * @param size  The size of the input bytes.
       * @param gpu the gpu to allocate the model on.
       */
      inline void initialize( unsigned char* bytes, unsigned size, unsigned gpu ) ;
      
      /** Method to initialize this object.
       * @param file The file with the preloaded model on it.
       * @param gpu The gpu to allocate the model on.
       */
      inline void initialize( nyx::NggFile& file, unsigned gpu ) ;
      
      /** Method to check whether this object has been initialized.
       * @return Whether or not this object has been initialized.
       */
      inline bool initialized() const ;
      
      /** Method to draw this model to the input pipeline.
       * @param pipeline The pipeline to use for rendering.
       * @param chain The chain to record the draw command to.
       */
      inline void draw( const nyx::Renderer<Framework>& pipeline, nyx::Chain<Framework>& chain ) ;
      
      /** Method to reset and deallocate all data.
       */
      inline void reset() ;
      
    private:
      std::vector<Mesh<Framework>*> meshes ;
  };
  
  template<typename Framework>
  Model<Framework>::Model()
  {
    this->meshes = {} ;
  }
  
  template<typename Framework>
  void Model<Framework>::initialize( const char* model_path, unsigned gpu )
  {
    nyx::NggFile          file  ;
    nyx::Chain<Framework> chain ;

    file.load( model_path ) ;
    
    if( file.meshCount() != 0 )
    {
      this->meshes.reserve( file.meshCount() ) ;
      chain.initialize( gpu, nyx::ChainType::Compute ) ;
      for( unsigned index = 0; index < file.meshCount(); index++ )
      {
        Mesh<Framework>* mesh = new Mesh<Framework>() ;
        this->meshes.push_back( mesh ) ;
        
        mesh->vertices.initialize( gpu, file.mesh( index ).numVertices(), false, nyx::ArrayFlags::Vertex ) ;
        mesh->indices .initialize( gpu, file.mesh( index ).numIndices (), false, nyx::ArrayFlags::Index  ) ;
        
        chain.copy( file.mesh( index ).vertices(), mesh->vertices ) ;
        chain.submit     () ;
        chain.synchronize() ;
        chain.copy( file.mesh( index ).indices (), mesh->indices  ) ;
        chain.submit     () ;
        chain.synchronize() ;
      }
      
      chain.submit     () ;
      chain.synchronize() ;
      chain.reset      () ;
    }
    else
    {
    }
  }
  
  template<typename Framework>
  void Model<Framework>::initialize( unsigned char* bytes, unsigned size, unsigned gpu )
  {
    nyx::NggFile          file  ;
    nyx::Chain<Framework> chain ;

    file.load( bytes, size ) ;
    if( file.meshCount() != 0 )
    {
      this->meshes.reserve( file.meshCount() ) ;
      chain.initialize( gpu, nyx::ChainType::Compute ) ;
      for( unsigned index = 0; index < file.meshCount(); index++ )
      {
        Mesh<Framework>* mesh = new Mesh<Framework>() ;
        this->meshes.push_back( mesh ) ;
        
        mesh->vertices.initialize( gpu, file.mesh( index ).numVertices(), false, nyx::ArrayFlags::Vertex ) ;
        mesh->indices .initialize( gpu, file.mesh( index ).numIndices (), false, nyx::ArrayFlags::Index  ) ;
        
        chain.copy( file.mesh( index ).vertices(), mesh->vertices ) ;
        chain.submit     () ;
        chain.synchronize() ;
        chain.copy( file.mesh( index ).indices (), mesh->indices  ) ;
        chain.submit     () ;
        chain.synchronize() ;
      }
      
      chain.submit     () ;
      chain.synchronize() ;
      chain.reset      () ;
    }
    else
    {
    }
  }
  
  template<typename Framework>
  void Model<Framework>::initialize( nyx::NggFile& file, unsigned gpu )
  {
    nyx::Chain<Framework> chain ;

    this->meshes.reserve( file.meshCount() ) ;
    chain.initialize( gpu, nyx::ChainType::Compute ) ;
    for( unsigned index = 0; index < file.meshCount(); index++ )
    {
      Mesh<Framework>* mesh = new Mesh<Framework>() ;
      this->meshes.push_back( mesh ) ;
      
      mesh->vertices.initialize( gpu, file.mesh( index ).numVertices(), false, nyx::ArrayFlags::Vertex ) ;
      mesh->indices .initialize( gpu, file.mesh( index ).numIndices (), false, nyx::ArrayFlags::Index  ) ;
      
      chain.copy( file.mesh( index ).vertices(), mesh->vertices ) ;
      chain.submit     () ;
      chain.synchronize() ;
      chain.copy( file.mesh( index ).indices (), mesh->indices  ) ;
      chain.submit     () ;
      chain.synchronize() ;
    }
    
    chain.submit     () ;
    chain.synchronize() ;
    chain.reset      () ;
  }
  
  template<typename Framework>
  bool Model<Framework>::initialized() const
  {
    return !this->meshes.empty() ;
  }

  template<typename Framework>
  void Model<Framework>::draw( const nyx::Renderer<Framework>& pipeline, nyx::Chain<Framework>& draw )
  {
    for( const auto& mesh : this->meshes )
    {
      draw.drawIndexed( pipeline, mesh->indices, mesh->vertices ) ;
    }
  }
}