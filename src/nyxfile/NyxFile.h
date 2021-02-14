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

namespace nyx
{
  class NyxFile ;
  
  /** The Shader stages possible for a KgFile to contain.
   */
  enum ShaderStage : unsigned
  {
    Vertex,
    Fragment,
    Geometry,
    Tess_C,
    Tess_E,
    Compute,
  };

  /** The enumeration of the types of Uniform variables possible in a KgShader file.
   */
  enum UniformType : unsigned
  {
    None,
    UBO,
    SAMPLER,
    IMAGE,
    SSBO
  };

  /** Iterator class to iterate over all shader data.
   */
  class ShaderIterator
  {
    public:
      /** Default constructor.
       */
      ShaderIterator() ;

      /** Copy constructor. Assigns this object to the input.
       * @param input The object to assign this one to.
       */
      ShaderIterator( const ShaderIterator& input ) ;

      /** Default Desconstructor.
       */
      ~ShaderIterator() ;
      
      /** Method to retrieve the number of uniforms in this shader stage.
       * @return unsigned The number of uniforms in this shader stage.
       */
      unsigned numUniforms() const ;

      /** Method to retrieve the number of attributes in this shader stage.
       * @return The number of attributes in this shader stage.
       */
      unsigned numAttributes() const ;

      /** Method to retrieve the C-String representation of the attribute name at the specified index.
       * @param index The index of attribute to retrieve the name for.
       * @return The C-String representation of the attribute name.
       */
      const char* attributeName( unsigned index ) ;

      /** Method to retrieve the attribute's location at the specified index.
       * @param index The index of attribute to search for.
       * @return The location of the attribute at the specified index.
       */
      unsigned attributeLocation( unsigned index ) ;

      /** Method to retrieve the byte size of the attribute at the specified index.
       * @param index The index of attribute to look up.
       * @return The size in bytes of the attribute.
       */
      unsigned attributeByteSize( unsigned index ) ;

      /** Method to retrieve whether or not the attribute at the specified index is an input.
       * @return Whether or not the attribute is classified as an input to the shader.
       */
      bool attributeIsInput( unsigned index ) ;

      /** Method to retrieve the type of attribute.
       * @param index The index of attribute to search for.
       * @return The attribute type in C-String format.
       */
      const char* attributeType( unsigned index ) ;

      /** Method to retrieve the full compiled spirv for this shader.
       * @return The compiled spirv representation of the current shader stage.
       */
      const unsigned* spirv() const ;

      /** Method to retrieve the size of the compiled spirv of this shader stage.
       * @return The size of the compiled spirv of this shader stage.
       */
      unsigned spirvSize() const ;
      
      /** Overloaded * operator to allow range-based for loops.
       * @return The shader stage of this iterator.
       */
      const ShaderIterator& operator*() const ;

      /** Method to retrieve the specific shader stage this iterator is representing.
       * @return The current shader stage.
       */
      ShaderStage stage() const ;

      /** Method to retrieve the type of uniform variable at the specified index.
       * @param id The index of uniform to look up.
       * @return The type of uniform that index represents.
       */
      UniformType uniformType( unsigned id ) const ;

      /** Method to retrieve the size in bytes of the uniform at the specified index.
       * @param id The index of uniform to look up.
       * @return The byte size of the specified uniform variable.
       */
      unsigned uniformSize( unsigned id ) const ;

      /** Method to retrieve the binding id of the specified uniform variable.
       * @param id The index to use for uniform look up.
       * @return The binding of the specified uniform.
       */
      unsigned uniformBinding( unsigned id ) const ;

      /** Method to retrieve the C-string representation of the uniform's name.
       * @param id The index to use for uniform look up.
       * @return C-string representation of the specified uniform's name.
       */
      const char* uniformName( unsigned id ) const ;

      /** ++ Operator to allow iteration of this object in a loop.
       */
      void operator++() ;

      /** Assignment operator. Assigns this object to the input.
       * @param input The object to assign this object to.
       * @return Reference to this object after assignment.
       */
      ShaderIterator& operator=( const ShaderIterator& input ) ;

      /** Inequality operator to use for comparison to allow iterating over this object.
       * @param input The object to test this object against.
       * @return Whether or not this object is equal to the input.
       */
      bool operator!=( const ShaderIterator& input ) const ;

    private:

      /** Forward declared structure containing this object's data.
       */
      struct ShaderIteratorData *shader_iterator_data ;

      /** Method to retrieve a reference to this object's internal data structure.
       * @return Reference to this object's internal data structure.
       */
      ShaderIteratorData& data() ;

      /** Method to retrieve a const-reference to this object's internal data structure.
       * @return Const-reference to this object's internal data structure.
       */
      const ShaderIteratorData& data() const  ;

      /** Forward declare friendship.
       */
      friend class NyxFile ;
  };

  /** Class to abstract a KgFile.
   */
  class NyxFile
  {
    public:

      /** Default constructor. 
       */
      NyxFile() ;

      /** Default deconstructor.
       */
      ~NyxFile() ;
      
      /** Assignment operator. Assigns the input to this object.
       * @param file The object to assign this one to.
       * @return Reference to this object after assignment.
       */
      NyxFile& operator=( const NyxFile& file ) ;

      /** Method to load the specified .nyx file at the input path.
       * @param The C-string path of the file on the filesystem to load.
       */
      void load( const char* path ) ;
      
      /** Method to load the specified .nyx file at the input path.
       * @param The array of bytes containing the .nyx file's data.
       */
      void load( const unsigned char* bytes, unsigned size ) ;

      /** Method to retrieve an iterator at the beginning of this object.
       * @return Iterator starting at the beginning of this object.
       */
      ShaderIterator begin() const ;

      /** Method to retrieve an iterator positioned at the end of this object.
       * @return Iterator starting at the end of this object.
       */
      ShaderIterator end() const ;

      /** Method to retrieve the number of shaders in this object.
       */
      unsigned size() const ;

    private:

      /** Forward declared structure containing this object's data.
       */
      struct NyxFileData* compiler_data ;

      /** Method to retrieve a reference to this object's internal data structure.
       * @return Reference to this object's internal data structure.
       */
      NyxFileData& data() ;

      /** Method to retrieve a const-reference to this object's internal data structure.
       * @return Const-reference to this object's internal data structure.
       */
      const NyxFileData& data() const ;
  };
}