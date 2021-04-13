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

#include "NggFile.h"
#include <string>
#include <sstream>
#include <fstream>
#include <istream>
#include <istream>
#include <iostream>
#include <vector>
#include <cstring>

namespace nyx
{
  static const unsigned long long MAGIC = 0x26656d696b750a ;
  using Mesh   = NggFile::Mesh   ;
  using Vertex = NggFile::Vertex ;

  struct NggFileData
  {
    unsigned                   version ;
    std::vector<NggFile::Mesh> meshes  ;

    NggFileData() ;
    unsigned readUnsigned( std::istream& stream ) const ;
    Mesh readMeshV1( std::istream& stream ) const ;
    std::string readString( std::istream& stream ) const ;
    unsigned long long readMagic( std::istream& stream ) const ;
    unsigned char* readBytes( std::istream& stream, unsigned size ) const ;
  };
  
  unsigned NggFileData::readUnsigned( std::istream& stream ) const
  {
    unsigned val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned ) ) ;
    return val ;
  }
  
  std::string NggFileData::readString( std::istream& stream ) const
  {
    unsigned    sz  ;
    std::string out ;

    stream.read( (char*)&sz, sizeof( unsigned ) ) ;
    out.resize( sz ) ;
    stream.read( (char*)out.data(), sz ) ;

    return out ;
  }
  
  Mesh NggFileData::readMeshV1( std::istream& stream ) const
  {
    Mesh        mesh ;
    std::string tmp  ;
    
    const std::string str = this->readString  ( stream ) ;
    std::strcpy( mesh.m_name, str.c_str() ) ;
    mesh.m_name[ NggFile::MAX_NAME_SIZE - 1 ] = '\0' ;
    mesh.num_vertices          = this->readUnsigned( stream ) ;
    mesh.num_indices           = this->readUnsigned( stream ) ;
    mesh.material.num_diffuse  = this->readUnsigned( stream ) ;
    mesh.material.num_specular = this->readUnsigned( stream ) ;
    mesh.material.num_normal   = this->readUnsigned( stream ) ;
    mesh.material.num_height   = this->readUnsigned( stream ) ;
    
    mesh.m_vertices   = reinterpret_cast<Vertex*>  ( this->readBytes( stream, mesh.num_vertices * sizeof( Vertex ) ) ) ;
    mesh.m_indices    = reinterpret_cast<unsigned*>( this->readBytes( stream, mesh.num_indices * sizeof( unsigned ) ) ) ;
    
    mesh.material.m_diffuse  = new char*[ mesh.material.num_diffuse  ] ;
    mesh.material.m_specular = new char*[ mesh.material.num_specular ] ;
    mesh.material.m_normal   = new char*[ mesh.material.num_normal   ] ;
    mesh.material.m_height   = new char*[ mesh.material.num_height   ] ;
    
    for( unsigned index = 0; index < mesh.material.num_diffuse; index++ )
    {
      tmp = this->readString( stream ) ;
      
      mesh.material.m_diffuse[ index ] = new char[ tmp.size() + 1 ] ;
      std::strcpy( mesh.material.m_diffuse[ index ], tmp.c_str() ) ;
    }
    
    for( unsigned index = 0; index < mesh.material.num_specular; index++ )
    {
      tmp = this->readString( stream ) ;
      
      mesh.material.m_specular[ index ] = new char[ tmp.size() + 1 ] ;
      std::strcpy( mesh.material.m_specular[ index ], tmp.c_str() ) ;
    }
    
    for( unsigned index = 0; index < mesh.material.num_normal; index++ )
    {
      tmp = this->readString( stream ) ;
      
      mesh.material.m_normal[ index ] = new char[ tmp.size() + 1 ] ;
      std::strcpy( mesh.material.m_normal[ index ], tmp.c_str() ) ;
    }
    
    for( unsigned index = 0; index < mesh.material.num_height; index++ )
    {
      tmp = this->readString( stream ) ;
      
      mesh.material.m_height[ index ] = new char[ tmp.size() + 1 ] ;
      std::strcpy( mesh.material.m_height[ index ], tmp.c_str() ) ;
    }
    
     return mesh ;
  }

  unsigned long long NggFileData::readMagic( std::istream& stream ) const
  {
    unsigned long long val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned long long ) ) ;
    return val ;
  }
  
  unsigned char* NggFileData::readBytes( std::istream& stream, unsigned size ) const
  {
    unsigned char* data = new unsigned char[ size ] ;
    stream.read( reinterpret_cast<char*>( data ), size * sizeof( unsigned char ) ) ;

    return data ;
  }
  
  NggFileData::NggFileData()
  {
    this->version = 0 ;
  }
  
  NggFile::NggFile()
  {
    this->ngt_file_data = new NggFileData() ;
  }

  NggFile::~NggFile()
  {
    this->reset() ;
    delete this->ngt_file_data ;
  }

  bool NggFile::load( const char* path )
  {
    std::ifstream              stream  ;
    std::vector<unsigned char> tmp     ;
    bool                       success ;
    
    stream.open( path ) ;
    success = false ;
    
    if( stream )
    {
      // Copy stream's contents into vector
      stream.seekg  ( 0, std::ios::end ) ;
      tmp   .reserve( stream.tellg()   ) ;
      stream.seekg  ( 0, std::ios::beg ) ;
        
      tmp.assign( ( std::istreambuf_iterator<char>( stream ) ), std::istreambuf_iterator<char>() ) ;
      
      // Call other load function to load.
      success = this->load( tmp.data(), tmp.size() ) ;
    }
    
    stream.close() ;
    
    return success ;
  }

  bool NggFile::load( const unsigned char* bytes, unsigned size )
  {
    std::stringstream  stream    ;
    unsigned long long magic     ;
    
    this->reset() ;
    stream.write( reinterpret_cast<const char*>( bytes ), sizeof( unsigned char ) * size ) ;
    
    magic = data().readMagic( stream ) ;
    
    // Check for magic number before going any further.
    if( magic == nyx::MAGIC )
    {
      data().version = data().readUnsigned( stream ) ;
      
      // Versions may change functionality, so check here.
      switch( data().version )
      {
        case 1:
          data().meshes.resize( data().readUnsigned( stream ) ) ;
          
          for( unsigned index = 0; index < data().meshes.size(); index++ )
          {
            data().meshes[ index ] = data().readMeshV1( stream ) ;
          }
        break ;
      }
    }
    else
    {
      return false ;
    }

    return true ;
  }

  unsigned NggFile::meshCount() const
  {
    return data().meshes.size() ;
  }
  
  const Mesh& NggFile::mesh( unsigned index )
  {
    static Mesh dummy ;
    return index < this->meshCount() ? data().meshes[ index ] : dummy ; 
  }
      
  void NggFile::reset()
  {
    for( auto& mesh : data().meshes )
    {
      delete[] mesh.m_vertices ;
      delete[] mesh.m_indices  ;
      
      for( unsigned index = 0; index < mesh.material.num_diffuse ; index++ ) delete[] mesh.material.m_diffuse [ index ] ;
      for( unsigned index = 0; index < mesh.material.num_specular; index++ ) delete[] mesh.material.m_specular[ index ] ;
      for( unsigned index = 0; index < mesh.material.num_normal  ; index++ ) delete[] mesh.material.m_normal  [ index ] ;
      for( unsigned index = 0; index < mesh.material.num_height  ; index++ ) delete[] mesh.material.m_height  [ index ] ;
      
      delete[] mesh.material.m_diffuse  ;
      delete[] mesh.material.m_specular ;
      delete[] mesh.material.m_normal   ;
      delete[] mesh.material.m_height   ;
    }
    
    data().meshes.clear() ;
  }

  unsigned NggFile::version() const
  {
    return data().version ;
  }

  NggFileData& NggFile::data()
  {
    return *this->ngt_file_data ;
  }

  const NggFileData& NggFile::data() const
  {
    return *this->ngt_file_data ;
  }
}