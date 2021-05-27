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

#include "NssFile.h"
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
  static const unsigned long long MAGIC = 0x29657d692b450a ;

  using vec4               = NssFile::vec4               ;
  using mat4               = NssFile::mat4               ;
  using AnimationTransform = NssFile::AnimationTransform ;
  using AnimationNode      = NssFile::AnimationNode      ;
  using Animation          = NssFile::Animation          ;
  using Bone               = NssFile::Bone               ;

  struct NssFileData
  {
    unsigned               version    ;
    unsigned               root_index ;
    std::vector<Bone     > bones      ;
    std::vector<Animation> animations ;
    
    NssFileData() ;
    unsigned readUnsigned( std::istream& stream ) const ;
    Bone readBoneV1( std::istream& stream ) const ;
    Animation readAnimationV1( std::istream& stream ) const ;
    AnimationNode readAnimNodeV1( std::istream& stream ) const ;
    float readFloat( std::istream& stream ) const ;
    vec4 readVec4( std::istream& stream ) const ;
    mat4 readMat4( std::istream& stream ) const ;
    std::string readString( std::istream& stream ) const ;
    unsigned long long readMagic( std::istream& stream ) const ;
    unsigned char* readBytes( std::istream& stream, unsigned size ) const ;
    void findBones() ;
  };
  
  struct AnimationData
  {
    std::string                name     ;
    float                      duration ;
    float                      fps      ;
    std::vector<AnimationNode> nodes    ;
    
    ~AnimationData()
    {
      this->nodes.clear() ;
    }
    
    AnimationData& operator=( const AnimationData& data )
    {
      this->name     = data.name     ;
      this->duration = data.duration ;
      this->fps      = data.fps      ;
      this->nodes    = data.nodes    ;
      
      return *this ;
    }
  };
  
  struct AnimationNodeData
  {
    std::string                     name       ;
    std::vector<AnimationTransform> transforms ;
    
    ~AnimationNodeData()
    {
      this->transforms.clear() ;
    }
    
    AnimationNodeData& operator=( const AnimationNodeData& data )
    {
      this->name       = data.name       ;
      this->transforms = data.transforms ;
      
      return *this ;
    }
  };

  struct BoneData
  {
    std::string                  name         ;
    mat4                         transform    ;
    std::vector<Bone*>           children     ;
    std::vector<std::string>     tmp_children ;
  };

  unsigned NssFileData::readUnsigned( std::istream& stream ) const
  {
    unsigned val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned ) ) ;
    return val ;
  }
  
  std::string NssFileData::readString( std::istream& stream ) const
  {
    unsigned    sz  ;
    std::string out ;

    stream.read( (char*)&sz, sizeof( unsigned ) ) ;
    out   .resize( sz ) ;
    stream.read( (char*)out.data(), sz ) ;

    return out ;
  }
  
  float NssFileData::readFloat( std::istream& stream ) const
  {
    return std::atof( this->readString( stream ).c_str() ) ;
  }
  
  vec4 NssFileData::readVec4( std::istream& stream ) const
  {
    vec4 vec ;
    
    vec.x = this->readFloat( stream ) ;
    vec.y = this->readFloat( stream ) ;
    vec.z = this->readFloat( stream ) ;
    vec.w = this->readFloat( stream ) ;
    
    return vec ;
  }
  
  mat4 NssFileData::readMat4( std::istream& stream ) const
  {
    mat4 mat ;
    
    mat.x.x = this->readFloat( stream ) ;
    mat.x.y = this->readFloat( stream ) ;
    mat.x.z = this->readFloat( stream ) ;
    mat.x.w = this->readFloat( stream ) ;
    mat.y.x = this->readFloat( stream ) ;
    mat.y.y = this->readFloat( stream ) ;
    mat.y.z = this->readFloat( stream ) ;
    mat.y.w = this->readFloat( stream ) ;
    mat.z.x = this->readFloat( stream ) ;
    mat.z.y = this->readFloat( stream ) ;
    mat.z.z = this->readFloat( stream ) ;
    mat.z.w = this->readFloat( stream ) ;
    mat.w.x = this->readFloat( stream ) ;
    mat.w.y = this->readFloat( stream ) ;
    mat.w.z = this->readFloat( stream ) ;
    mat.w.w = this->readFloat( stream ) ;
    
    return mat ;
  }
  
  unsigned long long NssFileData::readMagic( std::istream& stream ) const
  {
    unsigned long long val ;

    stream.read( reinterpret_cast<char*>( &val ), sizeof( unsigned long long ) ) ;
    return val ;
  }
  
  unsigned char* NssFileData::readBytes( std::istream& stream, unsigned size ) const
  {
    unsigned char* data = new unsigned char[ size ] ;
    stream.read( reinterpret_cast<char*>( data ), size * sizeof( unsigned char ) ) ;

    return data ;
  }
  
  Bone NssFileData::readBoneV1( std::istream& stream ) const
  {
    Bone bone ;
    
    bone.data().name      = this->readString( stream ) ;
    bone.data().transform = this->readMat4  ( stream ) ;
    
    bone.data().tmp_children.resize( this->readUnsigned( stream ) ) ;
    
    for( auto& str : bone.data().tmp_children )
    {
      str = this->readString( stream ) ;
    }
    
    return bone ;
  }
  
  Animation NssFileData::readAnimationV1( std::istream& stream ) const
  {
    Animation animation ;
    
    animation.data().name     = this->readString( stream ) ;
    animation.data().duration = this->readFloat ( stream ) ;
    animation.data().fps      = this->readFloat ( stream ) ;
    
    animation.data().nodes.resize( this->readUnsigned( stream ) ) ;
    
    for( auto& node : animation.data().nodes )
    {
      node = this->readAnimNodeV1( stream ) ;
    }
    
    return animation ;
  }
  
  AnimationNode NssFileData::readAnimNodeV1( std::istream& stream ) const
  {
    AnimationNode node ;
    
    node.data().name = this->readString( stream ) ;
    
    node.data().transforms.resize( this->readUnsigned( stream ) ) ;
    
    for( auto& transform : node.data().transforms )
    {
      transform.time     = this->readFloat( stream ) ;
      transform.position = this->readVec4 ( stream ) ;
      transform.scale    = this->readVec4 ( stream ) ;
      transform.rotation = this->readVec4 ( stream ) ;
    }
    
    return node ;
  }
  
  void NssFileData::findBones()
  {
    for( auto& bone : this->bones )
    {
      for( auto& child : bone.data().tmp_children )
      {
        for( auto& bone_two : this->bones )
        {
          if( bone_two.data().name == child )
          {
            bone.data().children.push_back( &bone_two ) ;
          }
        }
      }
      bone.data().tmp_children.clear() ;
    }
  }

  NssFileData::NssFileData()
  {
    this->version = 0 ;
  }
  
  AnimationNode::AnimationNode()
  {
    this->anim_data = new AnimationNodeData() ;
  }

  AnimationNode::AnimationNode( const AnimationNode& node ) 
  {
    this->anim_data = new AnimationNodeData() ;
    *this = node ;
  }

  AnimationNode::~AnimationNode()
  {
    delete this->anim_data ;
  }

  AnimationNode& AnimationNode::operator=( const AnimationNode& node )
  {
    *this->anim_data = *node.anim_data ;
    
    return *this ;
  }

  const char* AnimationNode::name() const
  {
    return data().name.c_str() ;
  }

  unsigned AnimationNode::transformCount() const
  {
    return data().transforms.size() ;
  }

  AnimationTransform& AnimationNode::transform( unsigned index )
  {
    static AnimationTransform dummy ;
    return index < data().transforms.size() ? data().transforms[ index ] : dummy ;
  }

  AnimationNodeData& AnimationNode::data()
  {
    return *this->anim_data ;
  }

  const AnimationNodeData& AnimationNode::data() const
  {
    return *this->anim_data ;
  }
          
  Animation::Animation()
  {
    this->anim_data = new AnimationData() ;
  }

  Animation::Animation( const Animation& anim )
  {
    this->anim_data = new AnimationData() ;
    *this = anim ;
  }
  
  Animation::~Animation()
  {
    delete this->anim_data ;
  }

  Animation& Animation::operator=( const Animation& animation )
  {
    *this->anim_data = *animation.anim_data ;
    
    return *this ;
  }

  const char* Animation::name() const
  {
    return data().name.c_str() ;
  }

  AnimationNode& Animation::node( unsigned index )
  {
    static AnimationNode dummy ;
    return index < data().nodes.size() ? data().nodes[ index ] : dummy ;
  }

  unsigned Animation::nodeCount() const
  {
    return data().nodes.size() ;
  }

  float Animation::fps() const
  {
    return data().fps ;
  }

  float Animation::duration() const
  {
    return data().duration ;
  }

  AnimationData& Animation::data()
  {
    return *this->anim_data ;
  }

  const AnimationData& Animation::data() const
  {
    return *this->anim_data ;
  }

  
  Bone::Bone()
  {
    this->bone_data = new BoneData() ;
  }

  Bone::~Bone()
  {
    delete this->bone_data ;
  }

  Bone& Bone::operator=( const Bone& bone )
  {
    *this->bone_data = *bone.bone_data ;
    
    return *this ;
  }

  const char* Bone::name() const
  {
    return data().name.c_str() ;
  }

  const mat4& Bone::transform() const
  {
    return data().transform ;
  }

  unsigned Bone::numChildren() const
  {
    return data().children.size() ;
  }
  
  const Bone& Bone::child( unsigned index ) const
  {
    static Bone dummy ;
    return index < data().children.size() ? *data().children[ index ] : dummy ;
  }
          
  BoneData& Bone::data()
  {
    return *this->bone_data ;
  }

  const BoneData& Bone::data() const
  {
    return *this->bone_data ;
  }
  
  NssFile::NssFile()
  {
    this->ngt_file_data = new NssFileData() ;
  }

  NssFile::~NssFile()
  {
    this->reset() ;
    delete this->ngt_file_data ;
  }

  bool NssFile::load( const char* path )
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

  bool NssFile::load( const unsigned char* bytes, unsigned size )
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
          data().root_index = data().readUnsigned( stream ) ;

          data().bones     .resize( data().readUnsigned( stream ) ) ;
          data().animations.resize( data().readUnsigned( stream ) ) ;
          
          for( auto& bone : data().bones )
          {
            bone = data().readBoneV1( stream ) ;
          }
          
          for( auto& anim : data().animations )
          {
            anim = data().readAnimationV1( stream ) ;
          }
          
          data().findBones() ;
          break ;
      }
    }
    else
    {
      return false ;
    }

    return true ;
  }

  const Bone& NssFile::root() const
  {
    static Bone dummy ;
    return data().root_index < data().bones.size() ? data().bones[ data().root_index ] : dummy ;
  }

  unsigned NssFile::animationCount() const
  {
    return data().animations.size() ;
  }
  
  const Animation& NssFile::animation( unsigned index )
  {
    static Animation dummy ;
    return index < this->animationCount() ? data().animations[ index ] : dummy ; 
  }
  
  unsigned NssFile::boneCount() const
  {
    return data().bones.size() ;
  }
  
  const Bone& NssFile::bone( unsigned index )
  {
    static Bone dummy ;
    return index < this->boneCount() ? data().bones[ index ] : dummy ; 
  }
      
  bool NssFile::has( const char* name ) const
  {
    for( auto& bone : data().bones )
    {
      if( bone.name() == std::string( name ) ) return true ;
    }
    
    return false ;
  }
      
  const Bone& NssFile::bone( const char* name )
  {
    static Bone dummy ;
    for( auto& bone : data().bones )
    {
      if( bone.name() == std::string( name ) ) return bone ;
    }
    return dummy ;
  }
      
  void NssFile::reset()
  {
    data().bones     .clear() ;
    data().animations.clear() ;
  }

  unsigned NssFile::version() const
  {
    return data().version ;
  }

  NssFileData& NssFile::data()
  {
    return *this->ngt_file_data ;
  }

  const NssFileData& NssFile::data() const
  {
    return *this->ngt_file_data ;
  }
}