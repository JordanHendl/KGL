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

#include "NgtFile.h"
#include <string>
#include <sstream>
#include <fstream>
#include <istream>
#include <istream>
#include <iostream>
#include <vector>

namespace nyx
{
  static const unsigned long long MAGIC = 0x7577755f6f776f0a ;

  struct NgtFileData
  {
    unsigned                   width        ;
    unsigned                   height       ;
    unsigned                   channels     ;
    unsigned                   element_size ;
    unsigned                   version      ;
    std::vector<unsigned char> bytes        ;
    
    NgtFileData() ;
    unsigned readUnsigned( std::istream& stream ) const ;
    unsigned long long readMagic( std::istream& stream ) const ;
    unsigned char* readBytes( std::istream& stream, unsigned size ) const ;
  };
  
  unsigned NgtFileData::readUnsigned( std::istream& stream ) const
  {
    unsigned val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned ) ) ;
    return val ;
  }
  
  unsigned long long NgtFileData::readMagic( std::istream& stream ) const
  {
    unsigned long long val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned long long ) ) ;
    return val ;
  }
  
  unsigned char* NgtFileData::readBytes( std::istream& stream, unsigned size ) const
  {
    unsigned char* data = new unsigned char[ size ] ;
    stream.read( reinterpret_cast<char*>( data ), size * sizeof( unsigned char ) ) ;

    return data ;
  }
  
  NgtFileData::NgtFileData()
  {
    this->width        = 0  ;
    this->height       = 0  ;
    this->channels     = 0  ;
    this->element_size = 1  ;
    this->bytes        = {} ;
  }
  
  NgtFile::NgtFile()
  {
    this->ngt_file_data = new NgtFileData() ;
  }

  NgtFile::~NgtFile()
  {
    delete this->ngt_file_data ;
  }

  bool NgtFile::load( const char* path )
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

  bool NgtFile::load( const unsigned char* bytes, unsigned size )
  {
    std::stringstream  stream    ;
    unsigned long long magic     ;
    unsigned char*     img_bytes ;
    
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
          data().width    = data().readUnsigned( stream                                                 ) ;
          data().height   = data().readUnsigned( stream                                                 ) ;
          data().channels = data().readUnsigned( stream                                                 ) ;
          img_bytes       = data().readBytes   ( stream, data().width * data().height * data().channels ) ;
          
          data().bytes.assign( img_bytes, img_bytes + data().width * data().height * data().channels ) ;
        break ;
      }
    }
    else
    {
      return false ;
    }

    return true ;
  }

  void NgtFile::reset()
  {
    data().bytes.clear() ;
    
    data().width        = 0  ;
    data().height       = 0  ;
    data().channels     = 0  ;
    data().element_size = 1  ;
    data().bytes        = {} ;
  }

  unsigned NgtFile::channels() const
  {
    return data().channels ;
  }

  unsigned NgtFile::width() const
  {
    return data().width ;
  }

  unsigned NgtFile::height() const
  {
    return data().height ;
  }

  unsigned NgtFile::version() const
  {
    return data().version ;
  }

  const unsigned char* NgtFile::image() const
  {
    return data().bytes.data() ;
  }

  NgtFileData& NgtFile::data()
  {
    return *this->ngt_file_data ;
  }

  const NgtFileData& NgtFile::data() const
  {
    return *this->ngt_file_data ;
  }
}