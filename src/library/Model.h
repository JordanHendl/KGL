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
 * Created on March 5, 2021, 7:39 PM
 */

#pragma once

#include "Material.h"
#include "Array.h"
#include <map>
#include <string>

namespace nyx
{
  namespace v1
  {
    template<typename Framework>
    class Model ;
    
    template<typename Framework>
    class Mesh ;
    
    struct Vertex
    {
      /** The vertex position of this model.
       */
      float x ;
      float y ;
      float z ;
      
      /** The UV coordinates if this model is attached to any textures.
       */
      float u ;
      float v ;
      
      /** The normal vectors for this model
       */
      float nx ;
      float ny ;
      float nz ;
      
      /** The indices for animations.
       */
      unsigned skeleton_ids[ 4 ] ;
    };
    
    /** Object to contain the implmentation of a model.
     */
    class ModelImpl
    {
      private:
        
        /** Friend declaration so the template can access this object.
         */
        template<typename Framework>
        friend class Model ;
        
        /** Default constructor.
         */
        ModelImpl() ;
        
        /** Default deconstructor.
         */
        ~ModelImpl() ;
        
        /** Method to parse the input file data.
         * @param nyx_model_file The data to parse.
         */
        void parse( const char* nyx_model_file ) ;
        
        /** Method to free all allocated host data.
         */
        void reset() ;
        
        /** Method to retrieve the amount of meshes parsed.
         * @return The amount of meshes found from the parsed data.
         */
        unsigned meshCount() const ;
        
        /** Method to retrieve the name of a mesh at the specified index.
         * @param idx The index of mesh to retrieve the name of.
         * @return The C-string name of the mesh at the specified index.
         */
        const char* name( unsigned idx ) const ;
  
        /** Method to retrieve the const pointer to the start of the vertices parsed.
         * @param idx The mesh id to retrieve the vertices of.
         * @return Const pointer to the array of vertices for the specified mesh.
         */
        const Vertex* vertices( unsigned idx ) const ;
        
        /** Method to retrieve the const pointer to the start of the indices parsed.
         * @param idx The mesh id to retrieve the indices of.
         * @return Const pointer to the array of indices for the specified mesh.
         */
        const unsigned* indices( unsigned idx ) const ;
        
        /** Method to retrieve the amount of vertices for the specified mesh.
         * @param idx The mesh to retrieve the vertex count of.
         * @return The amount of vertices in the mesh.
         */
        unsigned vertexCount( unsigned idx ) const ;
        
        /** Method to retrieve the amount of indices for the specified mesh.
         * @param idx The mesh to retrieve the index count of.
         * @return The amount of indices in the mesh.
         */
        unsigned indexCount( unsigned idx ) const ;
        
        /** Pointer to the underlying object data.
         */
        struct ModelData *model_data ;
        
        /** Method to retrieve a reference to the underlying object data.
         * @return Reference to this object's underlying data.
         */
        ModelData& data() ;
        
        /** Method to retrieve a const-reference to the underlying object data.
         * @return Const-reference to this object's underlying data.
         */
        const ModelData& data() const ;
    };
    
    /** Template object to encapsulate a mesh.
     * @tparam The framwork to use for allocating this mesh on the GPU.
     */
    template<typename Framework>
    class Mesh
    {
      public:
        
        /** Default constructor.
         */
        Mesh() ;
        
        /** Default deconstructor.
         */
        ~Mesh() ;
        
        /** Method to retrieve the name of this mesh.
         * @return The C-string name of this mesh.
         */
        const char* name() const ;
        
        /** Method to apply the specified material to this mesh.
         * @param material The material to apply to this mesh.
         */
        void apply( const Material<Framework>& material ) ;
  
      private:
        
        /** Friend declaration.
         */
        template<typename API>
        friend class Model ;
        
        /** Friend declaration.
         */
        friend class ModelImpl ;
        
        /** The material used to draw this object.
         */
        Material<Framework> material ;
        
        /** The Array of vertices used to draw this object.
         */
        Array<Framework, Vertex> vertices ;
        
        /** The Array of indices used to draw this object.
         */
        Array<Framework, unsigned> indices ;
    };
    
    /** Template class to encapsulate a 3-dimensional model.
     */ 
    template<typename Framework>
    class Model
    {
      public:
        
        /** Default constructor.
         */
        Model() ;
        
        /** Default deconstructor.
         */
        ~Model() ;
        
        /** Method to initialize this object.
         * @param gpu The gpu ID to use for all GPU calls.
         * @param nyx_model_file The buffer containing the .NMP file data to parse.
         */
        void initialize( unsigned gpu, const char* nyx_model_file ) ;
        
        /** Method to retrieve whether or not this object is initialized.
         * @return Whether or not this object is initialized.
         */
        bool initialized() ;
        
        /** Method to reset this object and deallocate all allocated data.
         */
        void reset() ;
        
        /** Method to retrieve the amount of meshes that encompass this model.
         * @return The amount of meshes that encompass this model.
         */
        unsigned count() const ;
        
        /** Method to retrieve a reference to a mesh of this object.
         * @param idx The id of mesh to retrieve.
         * @return Reference to the mesh at the specified id.
         */
        Mesh<Framework>& mesh( unsigned idx = 0 ) ;
        
        /** Method to retrieve a const reference to a mesh of this object.
         * @param idx The id of mesh to retrieve.
         * @return Const reference to the mesh at the specified id.
         */
        const Mesh<Framework>& mesh( unsigned idx = 0 ) const ;
      private:
        
        /** The container for this object's' internal meshes.
         */
        std::map<std::string, Mesh<Framework>*> meshes ;
        
        /** The underlying implementation of this object.
         */
        ModelImpl impl ;
    };
    
    template<typename Framework>
    Model<Framework>::Model()
    {
      this->meshes.clear() ;
    }
    
    template<typename Framework>
    void Model<Framework>::reset()
    {
      
    }
    
    template<typename Framework>
    Model<Framework>::~Model()
    {
//      if( this->meshes ) handleError( Error::LooseMemory ) ;
    }
    
    template<typename Framework>
    void Model<Framework>::initialize( unsigned gpu, const char* nyx_model_file )
    {
      this->impl.parse( nyx_model_file ) ;
      
      for( unsigned index = 0; index < this->impl.meshCount(); index++ )
      {
        this->meshes[ index ] = new Mesh<Framework>() ;
        
        this->meshes[ index ]->vertices.initialize( gpu, this->impl.vertexCount( index ) ) ;
        this->meshes[ index ]->indices .initialize( gpu, this->impl.indexCount ( index ) ) ;
  
        this->meshes[ index ]->vertices.copyToDevice( this->impl.vertices( index ), this->impl.vertexCount( index ) ) ;
        this->meshes[ index ]->indices .copyToDevice( this->impl.indices ( index ), this->impl.indexCount ( index ) ) ;
      }
      
      this->impl.reset() ;
    }
  }
}
