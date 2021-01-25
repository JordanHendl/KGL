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


#include "NyxFile.h"
#include <string>
#include <sstream>
#include <fstream>
#include <ostream>
#include <istream>
#include <iostream>
#include <vector>
#include <cerrno>
#include <memory>
#include <algorithm>
#include <ctype.h>
#include <map>
#include <limits.h>
#include <stdlib.h>

namespace nyx
{
  struct Shader ;
  typedef std::map<nyx::ShaderStage, Shader> ShaderMap ;

  const unsigned long long MAGIC = 0x555755200d0a ;

  static inline unsigned sizeFromType( std::string type_name ) ;

  unsigned sizeFromType( std::string type_name )
  {
         if( type_name == "mat4"     ) return sizeof( float    ) * 16 ;
    else if( type_name == "mat3"     ) return sizeof( float    ) * 9  ;
    else if( type_name == "vec4"     ) return sizeof( float    ) * 4  ;
    else if( type_name == "ivec4"    ) return sizeof( unsigned ) * 4  ;
    else if( type_name == "vec3"     ) return sizeof( float    ) * 3  ;
    else if( type_name == "ivec3"    ) return sizeof( unsigned ) * 3  ;
    else if( type_name == "vec2"     ) return sizeof( float    ) * 2  ;
    else if( type_name == "ivec2"    ) return sizeof( unsigned ) * 2  ;
    else if( type_name == "unsigned" ) return sizeof( float    ) * 3  ;
    else if( type_name == "float"    ) return sizeof( float    ) * 3  ;
    else if( type_name == "bool"     ) return sizeof( float    ) * 3  ;
    else { std::cout << "Unknown type : " << type_name << std::endl ; exit( -1 ) ; } ;
  }

  /** Structure to encompass a shader uniform.
   */
  struct Uniform
  {
    unsigned    binding ; ///< TODO
    unsigned    size    ; ///< TODO
    UniformType type    ; ///< TODO
    std::string name    ; ///< TODO
  };

  /** Structure to encompass a shader attribute.
   */
  struct Attribute
  {
    bool        input    ; ///< TOOD
    std::string name     ; ///< TOOD
    std::string type     ; ///< TOOD
    unsigned    size     ; ///< TOOD
    unsigned    location ; ///< TOOD
  };

  /** Structure to encompass a shader.
   */
  struct Shader
  {
    typedef std::vector<unsigned>   SpirVData     ;
    typedef std::vector<Uniform>    UniformList   ;
    typedef std::vector<Attribute>  AttributeList ;

    UniformList   uniforms   ;
    AttributeList attributes ;
    SpirVData     spirv      ;
    ShaderStage   stage      ;
    std::string   name       ;
  };

  struct ShaderIteratorData
  {
    ShaderMap::const_iterator it ;
  };
  
  /** Container for a KgFile's data.
   */
  struct KgFileData
  {
      std::string include_directory ;
      ShaderMap   map               ;

    /** Method to read a string from a file stream
     * @param stream The stream to read from
     * @return The string that has been read.
     */
    std::string readString( std::ifstream& stream ) const ;

    /** Method to read an unsigned integer from a file stream
     * @param stream The stream to read from.
     * @return The unsigned integer that has been read.
     */
    unsigned readUnsigned( std::ifstream& stream ) const ;

    /** Method to read a boolean from a file stream.
     * @param stream The stream to read from.
     * @return The boolean that has been read.
     */
    bool readBoolean( std::ifstream& stream ) const ;

    /** Method to read a magic number from a file stream.
     * @param stream The stream to read from.
     * @return The magic number read.
     */
    unsigned long long readMagic( std::ifstream& stream ) const ;

    /** Method to read SPIRV binary data from a file stream.
     * @param stream The stream to read from.
     * @param sz The side of the binary data that is in the stream.
     * @return Pointer to allocated memory of the loaded spirv binary.
     */
    unsigned* readSpirv( std::ifstream& stream, unsigned sz ) const ;
  };

  std::string KgFileData::readString( std::ifstream& stream ) const
  {
    unsigned    sz  ;
    std::string out ;

    stream.read( (char*)&sz, sizeof( unsigned ) ) ;
    out.resize( sz ) ;
    stream.read( (char*)out.data(), sz ) ;

    return out ;
  }

  unsigned KgFileData::readUnsigned( std::ifstream& stream ) const
  {
    unsigned val ;

    stream.read( (char*)&val, sizeof( unsigned ) ) ;
    return val ;
  }

  bool KgFileData::readBoolean( std::ifstream& stream ) const
  {
    bool val ;

    stream.read( (char*)&val, sizeof( bool ) ) ;
    return val ;
  }

  unsigned long long KgFileData::readMagic( std::ifstream& stream ) const
  {
    unsigned long long val ;

    stream.read( (char*)&val, sizeof( unsigned long long ) ) ;
    return val ;
  }

  unsigned* KgFileData::readSpirv( std::ifstream& stream, unsigned sz ) const
  {
    unsigned* data = new unsigned[ sz ] ;
    stream.read( (char*)data, sz * sizeof( unsigned ) ) ;

    return data ;
  }

  ShaderIterator::ShaderIterator()
  {
    this->shader_iterator_data = new ShaderIteratorData() ;
  }

  ShaderIterator::ShaderIterator( const ShaderIterator& input )
  {
    this->shader_iterator_data = new ShaderIteratorData() ;

    *this->shader_iterator_data = *input.shader_iterator_data ;
  }

  ShaderIterator::~ShaderIterator()
  {
    delete this->shader_iterator_data ;
  }

  unsigned ShaderIterator::spirvSize() const
  {
    return data().it->second.spirv.size() ;
  }

  unsigned ShaderIterator::numUniforms() const
  {
    return data().it->second.uniforms.size() ;
  }

  const unsigned* ShaderIterator::spirv() const
  {
    return data().it->second.spirv.data() ;
  }

  ShaderStage ShaderIterator::stage() const
  {
    return data().it->first ;
  }

  unsigned ShaderIterator::numAttributes() const
  {
    return data().it->second.attributes.size() ;
  }

  const char* ShaderIterator::attributeType( unsigned index )
  {
    return index < data().it->second.attributes.size() ? data().it->second.attributes[ index ].type.c_str() : "" ;
  }

  const char* ShaderIterator::attributeName( unsigned index )
  {
    return index < data().it->second.attributes.size() ? data().it->second.attributes[ index ].name.c_str() : "" ;
  }

  unsigned ShaderIterator::attributeByteSize( unsigned index )
  {
    return index < data().it->second.attributes.size() ? data().it->second.attributes[ index ].size : 0 ;
  }

  unsigned ShaderIterator::attributeLocation( unsigned index )
  {
    return index < data().it->second.attributes.size() ? data().it->second.attributes[ index ].location : 0 ;
  }

  bool ShaderIterator::attributeIsInput( unsigned index )
  {
    return index < data().it->second.attributes.size() ? data().it->second.attributes[ index ].input : 0 ;
  }

  UniformType ShaderIterator::uniformType( unsigned id ) const
  {
    return id < data().it->second.uniforms.size() ? data().it->second.uniforms[ id ].type : UniformType::None ;
  }

  unsigned ShaderIterator::uniformSize( unsigned id ) const
  {
    return id < data().it->second.uniforms.size() ? data().it->second.uniforms[ id ].size : 0 ;
  }

  unsigned ShaderIterator::uniformBinding( unsigned id ) const
  {
    return id < data().it->second.uniforms.size() ? data().it->second.uniforms[ id ].binding : UINT_MAX ;
  }

  const char* ShaderIterator::uniformName( unsigned id ) const
  {
    return id < data().it->second.uniforms.size() ? data().it->second.uniforms[ id ].name.c_str() : "" ;
  }

  void ShaderIterator::operator++()
  {
    ++data().it ;
  }

  ShaderIterator& ShaderIterator::operator=( const ShaderIterator& input )
  {
    data().it = input.data().it ;

    return *this ;
  }

  bool ShaderIterator::operator!=( const ShaderIterator& input )
  {
    return data().it != input.data().it ;
  }

  ShaderIteratorData& ShaderIterator::data()
  {
    return *this->shader_iterator_data ;
  }

  const ShaderIteratorData& ShaderIterator::data() const
  {
    return *this->shader_iterator_data ;
  }

  KgFile::KgFile()
  {
    this->compiler_data = new KgFileData() ;
  }

  KgFile::~KgFile()
  {
    delete this->compiler_data ;
  }
  
  KgFile& KgFile::operator =( const KgFile& file )
  {
    *this->compiler_data = *file.compiler_data ;
    
    return *this ;
  }

  void KgFile::load( const char* path )
  {
    std::ifstream              stream  ;
    std::string                str     ;
    unsigned                   sz      ;
    unsigned long long         magic   ;
    ::nyx::Shader      shader  ;
    ::nyx::Uniform     uniform ;
    ::nyx::Attribute   attr    ;

    data().map.clear() ;
    stream.open( path, std::ios::binary ) ;

    if( stream )
    {
      magic = data().readMagic( stream ) ;        
      if( magic != ::nyx::MAGIC ) /*TODO: LOG ERROR HERE */ return ;

      sz = data().readUnsigned( stream ) ;
      for( unsigned it = 0; it < sz; it++ )
      {
        const unsigned  spirv_size     = data().readUnsigned( stream             ) ;
        const unsigned* spirv          = data().readSpirv   ( stream, spirv_size ) ;
        const unsigned  stage          = data().readUnsigned( stream             ) ;
        const unsigned  num_uniforms   = data().readUnsigned( stream             ) ;
        const unsigned  num_attributes = data().readUnsigned( stream             ) ;

        shader.spirv   .clear() ;
        shader.uniforms.clear() ;

        shader.spirv     .assign( spirv, spirv + spirv_size ) ;
        shader.uniforms  .resize( num_uniforms              ) ;
        shader.attributes.resize( num_attributes            ) ;
        shader.stage = static_cast<::nyx::ShaderStage>( stage ) ;
        for( unsigned index = 0; index < num_uniforms; index++ )
        {
           const std::string name         = data().readString  ( stream ) ;
           const unsigned uniform_type    = data().readUnsigned( stream ) ;
           const unsigned uniform_binding = data().readUnsigned( stream ) ;
           const unsigned uniform_size    = data().readUnsigned( stream ) ;

           uniform.name    = name                                            ;
           uniform.type    = static_cast<::nyx::UniformType>( uniform_type ) ;
           uniform.binding = uniform_binding                                 ;
           uniform.size    = uniform_size                                    ;

           shader.uniforms[ index ] = uniform ;
        }
        for( unsigned index = 0; index < num_attributes; index++ )
        {
          const std::string name  = data().readString  ( stream ) ;
          const std::string type  = data().readString  ( stream ) ;
          const unsigned    size  = data().readUnsigned( stream ) ;
          const unsigned location = data().readUnsigned( stream ) ;
          const bool     input    = data().readBoolean ( stream ) ;

          attr.name     = name     ;
          attr.location = location ;
          attr.size     = size     ;
          attr.type     = type     ;
          attr.input    = input    ;

          shader.attributes[ index ] = attr ;
        }

        data().map.insert( { shader.stage, shader } ) ;
      }
    }
  }

  ShaderIterator KgFile::begin() const
  {
    ShaderIterator it ;
    it.data().it = data().map.begin() ;

    return it ;
  }

  ShaderIterator KgFile::end() const 
  {
    ShaderIterator it ;
    it.data().it = data().map.end() ;

    return it ;
  }

  unsigned KgFile::size() const
  {
    return data().map.size() ;
  }

  KgFileData& KgFile::data()
  {
    return *this->compiler_data ;
  }

  const KgFileData& KgFile::data() const
  {
    return *this->compiler_data ;
  }
}