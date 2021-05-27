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
    
    UniformList   uniforms   ;
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
  struct NyxFileData
  {
    using AttributeList = std::vector<Attribute> ;

    AttributeList inputs            ;
    AttributeList outputs           ;
    std::string   include_directory ;
    ShaderMap     map               ;
    unsigned      version           ;

    /** Method to read a string from a file stream
     * @param stream The stream to read from
     * @return The string that has been read.
     */
    std::string readString( std::istream& stream ) const ;

    /** Method to read an unsigned integer from a file stream
     * @param stream The stream to read from.
     * @return The unsigned integer that has been read.
     */
    unsigned readUnsigned( std::istream& stream ) const ;

    /** Method to read a boolean from a file stream.
     * @param stream The stream to read from.
     * @return The boolean that has been read.
     */
    bool readBoolean( std::istream& stream ) const ;

    /** Method to read a magic number from a file stream.
     * @param stream The stream to read from.
     * @return The magic number read.
     */
    unsigned long long readMagic( std::istream& stream ) const ;

    /** Method to read SPIRV binary data from a file stream.
     * @param stream The stream to read from.
     * @param sz The side of the binary data that is in the stream.
     * @return Pointer to allocated memory of the loaded spirv binary.
     */
    unsigned* readSpirv( std::istream& stream, unsigned sz ) const ;
  };

  std::string NyxFileData::readString( std::istream& stream ) const
  {
    unsigned    sz  ;
    std::string out ;

    stream.read( (char*)&sz, sizeof( unsigned ) ) ;
    out.resize( sz ) ;
    stream.read( (char*)out.data(), sz ) ;

    return out ;
  }

  unsigned NyxFileData::readUnsigned( std::istream& stream ) const
  {
    unsigned val ;

    stream.read( (char*)&val, sizeof( unsigned ) ) ;
    return val ;
  }

  bool NyxFileData::readBoolean( std::istream& stream ) const
  {
    bool val ;

    stream.read( (char*)&val, sizeof( bool ) ) ;
    return val ;
  }

  unsigned long long NyxFileData::readMagic( std::istream& stream ) const
  {
    unsigned long long val ;

    stream.read( (char*)&val, sizeof( unsigned long long ) ) ;
    return val ;
  }

  unsigned* NyxFileData::readSpirv( std::istream& stream, unsigned sz ) const
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

  const ShaderIterator& ShaderIterator::operator*() const
  {
    return *this ;
  }
  
  ShaderStage ShaderIterator::stage() const
  {
    return data().it->first ;
  }

  unsigned ShaderIterator::numAttributes() const
  {
    return 0 ;
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

  bool ShaderIterator::operator!=( const ShaderIterator& input ) const
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

  NyxFile::NyxFile()
  {
    this->compiler_data = new NyxFileData() ;
  }

  NyxFile::~NyxFile()
  {
    delete this->compiler_data ;
  }
  
  NyxFile& NyxFile::operator =( const NyxFile& file )
  {
    *this->compiler_data = *file.compiler_data ;
    
    return *this ;
  }

  void NyxFile::load( const char* path )
  {
    std::ifstream              stream  ;
    std::vector<unsigned char> tmp     ;
    
    stream.open( path ) ;
    
    if( stream )
    {
      // Copy stream's contents into vector
      stream.seekg  ( 0, std::ios::end ) ;
      tmp   .reserve( stream.tellg()   ) ;
      stream.seekg  ( 0, std::ios::beg ) ;
        
      tmp.assign( ( std::istreambuf_iterator<char>( stream ) ), std::istreambuf_iterator<char>() ) ;
      
      // Call other load function to load.
      this->load( tmp.data(), tmp.size() ) ;
    }
    
    stream.close() ;
  }
  
  void NyxFile::load( const unsigned char* bytes, unsigned size )
  {
    std::stringstream          stream      ;
    unsigned                   num_shaders ;
    unsigned                   num_inputs  ;
    unsigned                   num_outputs ;
    unsigned long long         magic       ;
    nyx::Shader                shader      ;
    nyx::Uniform               uniform     ;
    nyx::Attribute             attr        ;

    data().map.clear() ;
    stream.write( reinterpret_cast<const char*>( bytes ), sizeof( unsigned char ) * size ) ;

    magic = data().readMagic( stream ) ;        
    if( magic != ::nyx::MAGIC ) /*TODO: LOG ERROR HERE */ return ;

    data().version = data().readUnsigned( stream ) ;
    num_shaders    = data().readUnsigned( stream ) ;
    num_inputs     = data().readUnsigned( stream ) ;
    num_outputs    = data().readUnsigned( stream ) ;
    
    for( unsigned index = 0; index < num_inputs; index++ )
    {
      const std::string name     = data().readString  ( stream ) ;
      const std::string type     = data().readString  ( stream ) ;
      const unsigned    size     = data().readUnsigned( stream ) ;
      const unsigned    location = data().readUnsigned( stream ) ;
      
      attr.name     = name     ;
      attr.type     = type     ;
      attr.size     = size     ;
      attr.location = location ;
      
      data().inputs.push_back( attr ) ;
    }
    
    for( unsigned index = 0; index < num_outputs; index++ )
    {
      const std::string name     = data().readString  ( stream ) ;
      const std::string type     = data().readString  ( stream ) ;
      const unsigned    size     = data().readUnsigned( stream ) ;
      const unsigned    location = data().readUnsigned( stream ) ;
      
      attr.name     = name     ;
      attr.type     = type     ;
      attr.size     = size     ;
      attr.location = location ;
      
      data().outputs.push_back( attr ) ;
    }
    
    for( unsigned it = 0; it < num_shaders; it++ )
    {
      const unsigned  spirv_size     = data().readUnsigned( stream             ) ;
      const unsigned* spirv          = data().readSpirv   ( stream, spirv_size ) ;
      const unsigned  stage          = data().readUnsigned( stream             ) ;
      const unsigned  num_uniforms   = data().readUnsigned( stream             ) ;

      shader.spirv   .clear() ;
      shader.uniforms.clear() ;

      shader.spirv     .assign( spirv, spirv + spirv_size ) ;
      shader.uniforms  .resize( num_uniforms              ) ;

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
      data().map.insert( { shader.stage, shader } ) ;
    }
  }

  ShaderIterator NyxFile::begin() const
  {
    ShaderIterator it ;
    it.data().it = data().map.begin() ;

    return it ;
  }

  ShaderIterator NyxFile::end() const 
  {
    ShaderIterator it ;
    it.data().it = data().map.end() ;

    return it ;
  }

  const char* NyxFile::inputName( unsigned index )
  {
    if( index < data().inputs.size() ) return data().inputs[ index ].name.c_str() ;
    return "" ;
  }

  unsigned NyxFile::inputLocation( unsigned index )
  {
    if( index < data().inputs.size() ) return data().inputs[ index ].location ;
    return 0 ;
  }

  unsigned NyxFile::inputByteSize( unsigned index )
  {
    if( index < data().inputs.size() ) return data().inputs[ index ].size ;
    return 0 ;
  }

  const char* NyxFile::inputType( unsigned index )
  {
    if( index < data().inputs.size() ) return data().inputs[ index ].type.c_str() ;
    return "" ;
  }

  const char* NyxFile::outputName( unsigned index )
  {
    if( index < data().outputs.size() ) return data().outputs[ index ].name.c_str() ;
    return "" ;
  }

  unsigned NyxFile::outputLocation( unsigned index )
  {
    if( index < data().outputs.size() ) return data().outputs[ index ].location ;
    return 0 ;
  }

  unsigned NyxFile::outputByteSize( unsigned index )
  {
    if( index < data().outputs.size() ) return data().outputs[ index ].size ;
    return 0 ;
  }

  const char* NyxFile::outputType( unsigned index )
  {
    if( index < data().outputs.size() ) return data().outputs[ index ].type.c_str() ;
    return "" ;
  }

  unsigned NyxFile::numInputs() const
  {
    return data().inputs.size() ;
  }

  unsigned NyxFile::numOutputs() const
  {
    return data().outputs.size() ;
  }

  unsigned NyxFile::size() const
  {
    return data().map.size() ;
  }

  NyxFileData& NyxFile::data()
  {
    return *this->compiler_data ;
  }

  const NyxFileData& NyxFile::data() const
  {
    return *this->compiler_data ;
  }
}