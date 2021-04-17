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

#pragma once
#include <string>

namespace nyx
{
  /** Class to manage loading Nyx Generated Geometry files.
   */
  class NggFile
  {
    public:
      
      /** The max file size possible for a mesh's name.
       */
      static constexpr unsigned MAX_NAME_SIZE = 50 ;
      
      /** Structure containing the format of this file's vertex.
       */
      struct Vertex
      {
        float    position[ 4 ] ;
        float    normals [ 4 ] ;
        float    weights [ 4 ] ;
        unsigned ids     [ 4 ] ;
        float    uvs     [ 2 ] ;
      };

      struct Material
      {
        public:
          inline unsigned diffuseCount() const ;
          inline unsigned specularCount() const ;
          inline unsigned normalCount() const ;
          inline unsigned heightCount() const ;
          inline std::string diffuse ( unsigned index ) ;
          inline std::string specular( unsigned index ) ;
          inline std::string normal( unsigned index ) ;
          inline std::string height( unsigned index ) ;
        private:
          friend class NggFile     ;
          friend class NggFileData ;
          
          unsigned num_diffuse  = 0 ;
          unsigned num_specular = 0 ;
          unsigned num_normal   = 0 ;
          unsigned num_height   = 0 ;
          
          char** m_diffuse  = nullptr ;
          char** m_specular = nullptr ;
          char** m_normal   = nullptr ;
          char** m_height   = nullptr ;
      };

      /** Helper struct to contain a mesh's contents.
       */
      struct Mesh
      {
        public:
          inline std::string name() const ;
          const inline Vertex& vertex( unsigned index ) const ;
          inline unsigned index( unsigned index ) ;
          inline const Vertex* vertices() const ;
          inline const unsigned* indices() const ;
          inline unsigned numVertices() const ;
          inline unsigned numIndices() const ;
        private:
          friend class NggFile     ;
          friend class NggFileData ;
          
          char      m_name[ MAX_NAME_SIZE ]           ;
          Vertex  * m_vertices              = nullptr ;
          unsigned* m_indices               = nullptr ;
          unsigned num_vertices             = 0       ;
          unsigned num_indices              = 0       ;
          Material material                           ;
      };
      
      /** Constructor.
       */
      NggFile() ;
      
      /** Deconstructor.
       */
      ~NggFile() ;

      /** Method to load a Nyx Generated Geometry from disk.
       * @param path The path to the file on the filesystem.
       * @return Whether the file was successful in being loaded or not.
       */
      bool load( const char* path ) ;
      
      /** Method to load a Nyx Generated Geometry file from preloaded bytes.
       * @param bytes The bytes of the .ngg file.
       * @param size The size of the byte array.
       * @return Whether or not the file was successfully loaded.
       */
      bool load( const unsigned char* bytes, unsigned size ) ;
      
      /** Method to reset this object and release all allocated data.
       */
      void reset() ;
      
      /** Method to retrieve the amount of meshes loaded by this object.
       * @return The amount of meshes loaded by this object.
       */
      unsigned meshCount() const ;
      
      /** Method to retrieve the reference to the specified mesh.
       * @param index The index of mesh to retrieve.
       * @return Const Reference to loaded mesh at specified index.
       */
      const Mesh& mesh( unsigned index ) ;
      
      /** Method to retrieve the version of the loaded file.
       * @return 
       */
      unsigned version() const ;
    private:
      /** Forward declared structure containing this object's data.
       */
      struct NggFileData *ngt_file_data ;
      
      /** Method to retrieve a reference to this object's internal data structure.
       * @return Reference to this object's internal data structure.
       */
      NggFileData& data() ;
      
      /** Method to retrieve a const-reference to this object's internal data structure.
       * @return Const-reference to this object's internal data structure.
       */
      const NggFileData& data() const ;
  };
  
  std::string NggFile::Mesh::name() const
  {
    std::string str ;

    str = this->m_name ;
    return str ;
  }

  const NggFile::Vertex& NggFile::Mesh::vertex( unsigned index ) const
  {
    static NggFile::Vertex dummy ;
    
    return index < this->num_vertices ? this->m_vertices[ index ] : dummy ;
  }

  unsigned NggFile::Mesh::index( unsigned index )
  {
    return index < this->num_indices ? this->m_indices[ index ] : 0 ;
  }

  const NggFile::Vertex* NggFile::Mesh::vertices() const
  {
    return this->m_vertices ;
  }

  const unsigned* NggFile::Mesh::indices() const
  {
    return this->m_indices ;
  }

  unsigned NggFile::Mesh::numVertices() const
  {
    return this->num_vertices ;
  }

  unsigned NggFile::Mesh::numIndices() const
  {
    return this->num_indices ;
  }

  unsigned NggFile::Material::diffuseCount() const
  {
    return this->num_diffuse ;
  }
  
  unsigned NggFile::Material::specularCount() const
  {
    return this->num_specular ;
  }
  
  unsigned NggFile::Material::normalCount() const
  {
    return this->num_normal ;
  }
  
  unsigned NggFile::Material::heightCount() const
  {
    return this->num_height ;
  }
  
  std::string NggFile::Material::diffuse ( unsigned index )
  {
    std::string str ;
    
    str = "" ;
    if( index < this->num_diffuse ) str = this->m_diffuse[ index ] ;
    
    return str ;
  }
  
  std::string NggFile::Material::specular( unsigned index )
  {
    std::string str ;
    
    str = "" ;
    if( index < this->num_specular ) str = this->m_specular[ index ] ;
    
    return str ; 
  }
  
  std::string NggFile::Material::normal( unsigned index )
  {
    std::string str ;
    
    str = "" ;
    if( index < this->num_normal ) str = this->m_normal[ index ] ;
    
    return str ;
  }
  
  std::string NggFile::Material::height( unsigned index )
  {
    std::string str ;
    
    str = "" ;
    if( index < this->num_height ) str = this->m_height[ index ] ;
    
    return str ;
  }
}