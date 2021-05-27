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

#include "NttFile.h"
#include <string>
#include <sstream>
#include <fstream>
#include <istream>
#include <istream>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>

namespace nyx
{
  static const unsigned long long MAGIC = 0x296f7d692e450a ;


  struct NttFileData
  {
    unsigned                                                     version           ;
    std::map<unsigned char, NttFile::Character>                  characters        ;
    std::map<unsigned char, std::pair<unsigned char*, unsigned>> character_buffers ;
    
    NttFileData() ;
    unsigned readUnsigned( std::istream& stream ) const ;
    float readFloat( std::istream& stream ) const ;
    std::string readString( std::istream& stream ) const ;
    unsigned long long readMagic( std::istream& stream ) const ;
    int readInteger( std::istream& stream ) const ;
    unsigned char readCharacter( std::istream& stream ) const ;
    unsigned char* readBytes( std::istream& stream, unsigned size ) const ;
    void readCharactersV1( std::istream& stream ) ;
  };
  
  unsigned NttFileData::readUnsigned( std::istream& stream ) const
  {
    unsigned val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned ) ) ;
    return val ;
  }
  
  int NttFileData::readInteger( std::istream& stream ) const
  {
    int val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( int ) ) ;
    return val ;
  }
  
  unsigned char NttFileData::readCharacter( std::istream& stream ) const
  {
    unsigned char val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned char ) ) ;
    return val ;
  }
    
  std::string NttFileData::readString( std::istream& stream ) const
  {
    unsigned    sz  ;
    std::string out ;

    stream.read( (char*)&sz, sizeof( unsigned ) ) ;
    out   .resize( sz ) ;
    stream.read( (char*)out.data(), sz ) ;

    return out ;
  }
  
  float NttFileData::readFloat( std::istream& stream ) const
  {
    return std::atof( this->readString( stream ).c_str() ) ;
  }
  
  unsigned long long NttFileData::readMagic( std::istream& stream ) const
  {
    unsigned long long val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned long long ) ) ;
    return val ;
  }
  
  unsigned char* NttFileData::readBytes( std::istream& stream, unsigned size ) const
  {
    unsigned char* data = new unsigned char[ size ] ;
    stream.read( reinterpret_cast<char*>( data ), size * sizeof( unsigned char ) ) ;

    return data ;
  }
  
  void NttFileData::readCharactersV1( std::istream& stream )
  {
    unsigned           character_count ;
    NttFile::Character character       ;
    unsigned char      ch              ;
    unsigned           buffer_size     ;
    unsigned char*     buffer          ;
    
    character_count     = this->readUnsigned ( stream ) ;
    
    for( unsigned index = 0; index < character_count; index++ )
    {
      ch                  = this->readCharacter( stream              ) ;
      buffer_size         = this->readUnsigned ( stream              ) ;
      character.bearing.x = this->readInteger  ( stream              ) ;
      character.bearing.y = this->readInteger  ( stream              ) ;
      character.size.x    = this->readInteger  ( stream              ) ;
      character.size.y    = this->readInteger  ( stream              ) ;
      character.advance   = this->readUnsigned ( stream              ) ;
      buffer              = this->readBytes    ( stream, buffer_size ) ;
      
      this->characters       [ ch ] = character               ;
      this->character_buffers[ ch ] = { buffer, buffer_size } ;
    }
  }
  
  NttFileData::NttFileData()
  {
    this->version = 0 ;
  }
  
  NttFile::NttFile()
  {
    this->ntt_file_data = new NttFileData() ;
  }

  NttFile::~NttFile()
  {
    this->reset() ;
    delete this->ntt_file_data ;
  }

  bool NttFile::load( const char* path )
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

  bool NttFile::load( const unsigned char* bytes, unsigned size )
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
          data().readCharactersV1( stream ) ;
          break ;
      }
    }
    else
    {
      return false ;
    }

    return true ;
  }
      
  unsigned NttFile::characterCount() const
  {
    return data().characters.size() ;
  }
  
  const NttFile::Character& NttFile::character( unsigned char character ) const
  {
    static NttFile::Character dummy ;
    auto iter = data().characters.find( character ) ;
    
    return iter != data().characters.end() ? iter->second : dummy  ;
  }
  
  const unsigned char* NttFile::characterImage( unsigned char character ) const
  {
    auto iter = data().character_buffers.find( character ) ;
    
    return iter != data().character_buffers.end() ? iter->second.first : nullptr ;
  }
  
  unsigned NttFile::characterImageSize( unsigned char character ) const
  {
    auto iter = data().character_buffers.find( character ) ;
    
    return iter != data().character_buffers.end() ? iter->second.second : 0 ;
  }

  void NttFile::deallocate()
  {
    for( auto& buff : data().character_buffers )
    {
      if( buff.second.first ) delete[] buff.second.first ;
      
      buff.second.first = nullptr ;
    }
    
    data().character_buffers.clear() ;
  }
  void NttFile::reset()
  {
    this->deallocate() ;
    data().characters.clear() ;
  }

  unsigned NttFile::version() const
  {
    return data().version ;
  }

  NttFileData& NttFile::data()
  {
    return *this->ntt_file_data ;
  }

  const NttFileData& NttFile::data() const
  {
    return *this->ntt_file_data ;
  }
}