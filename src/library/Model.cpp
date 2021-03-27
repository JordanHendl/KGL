/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Model.cpp
 * Author: jhendl
 * 
 * Created on March 5, 2021, 7:39 PM
 */

#include "Model.h"
#include <common/Parser.h>
#include <map>
#include <vector>
#include <string>

namespace nyx
{
  namespace v1
  {
    struct ModelData
    {
      std::string                                      name       ;
      nyx::json::Parser                                parser     ;
      std::map<unsigned, std::string                 > mesh_names ;
      std::map<unsigned, std::vector<nyx::v1::Vertex>> vertices   ;
      std::map<unsigned, std::vector<unsigned       >> indices    ;
    };
  
    ModelImpl::ModelImpl()
    {
      this->model_data = new ModelData() ;
    }
  
    ModelImpl::~ModelImpl()
    {
      delete this->model_data ;
    }
  
    void ModelImpl::parse( const char* nyx_model_file )
    {
      std::string     name   ;
      nyx::v1::Vertex vertex ;
      unsigned        index  ;
      
      data().parser.initialize( nyx_model_file ) ;
      
      index = 0 ;
      for( auto token : data().parser )
      {
        data().mesh_names[ index ] = token.key() ;
        data().vertices  [ index ].reserve( token[ "num_vertices" ].number()     ) ;
        data().indices   [ index ].reserve( token[ "num_faces"    ].number() * 3 ) ;
        
        auto vertices = token[ "vertices" ] ;
        for( unsigned vert_index = 0; vert_index < vertices.size(); vert_index++ )
        {
//          vertices.; TODO figure this out.
          
          
          data().vertices[ index ].push_back( vertex ) ;
        }
      }
    }
  
    void ModelImpl::reset()
    {
      data().parser.clear() ;
      for( auto& vec : data().vertices ) vec.second.clear() ;
      for( auto& vec : data().indices  ) vec.second.clear() ;
    }
  
    unsigned ModelImpl::meshCount() const
    {
      return data().vertices.size() ;
    }
  
    const Vertex* ModelImpl::vertices( unsigned idx ) const
    {
      auto iter = data().vertices.find( idx ) ;
      if( iter != data().vertices.end() ) return iter->second.data() ;
      return nullptr ;
    }
  
    const unsigned* ModelImpl::indices( unsigned idx ) const
    {
      auto iter = data().indices.find( idx ) ;
      if( iter != data().indices.end() ) return iter->second.data() ;
      return nullptr ;
    }
  
    unsigned ModelImpl::vertexCount( unsigned idx ) const
    {
      auto iter = data().vertices.find( idx ) ;
      if( iter != data().vertices.end() )
      return iter->second.size() ;
      return 0 ;
    }
  
    unsigned ModelImpl::indexCount( unsigned idx ) const
    {
      auto iter = data().indices.find( idx ) ;
      if( iter != data().indices.end() )
      return iter->second.size() ;
      return 0 ;
    }
  
    ModelData& ModelImpl::data()
    {
      return *this->model_data ;
    }
  
    const ModelData& ModelImpl::data() const
    {
      return *this->model_data ;
    }
  }
}
