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
  struct NssFileData ;

  /** Class to manage loading Nyx Spooky Skeleton files.
   */
  class NssFile
  {
    public:
      
      struct vec4
      {
        float x ; 
        float y ; 
        float z ; 
        float w ; 
      };
      
      struct mat4
      {
        vec4 x ;
        vec4 y ;
        vec4 z ;
        vec4 w ;
      };

      struct AnimationTransform
      {
        float time     = 0.0f                   ;
        vec4  position = { 0.f, 0.f, 0.f, 0.f } ;
        vec4  scale    = { 0.f, 0.f, 0.f, 0.f } ;
        vec4  rotation = { 0.f, 0.f, 0.f, 0.f } ;
      };
      
      class AnimationNode
      {
        public:
          AnimationNode() ;
          AnimationNode( const AnimationNode& node ) ;
          ~AnimationNode() ;
          AnimationNode& operator=( const AnimationNode& node ) ;
          const char* name() const ;
          unsigned transformCount() const ;
          AnimationTransform& transform( unsigned index ) ;
        private:
          friend struct NssFileData ;
          struct AnimationNodeData* anim_data ;
          AnimationNodeData& data() ;
          const AnimationNodeData& data() const ;
      };

      class Animation
      {
        public:
          Animation() ;
          Animation( const Animation& anim ) ;
          ~Animation() ;
          Animation& operator=( const Animation& animation ) ;
          const char* name() const ;
          AnimationNode& node( unsigned index = 0 ) ;
          unsigned nodeCount() const ;
          float fps() const ;
          float duration() const ;
        private:
          friend struct NssFileData ;
          struct AnimationData* anim_data ;
          AnimationData& data() ;
          const AnimationData& data() const ;
      };

      class Bone
      {
        public:
          Bone() ;
          ~Bone() ;
          Bone& operator=( const Bone& bone ) ;
          const char* name() const ;
          const mat4& transform() const ;
          unsigned numChildren() const ;
          const Bone& child( unsigned index ) const ;
        private:
          friend struct NssFileData ;
          struct BoneData* bone_data ;
          BoneData& data() ;
          const BoneData& data() const ;
      };

      /** Constructor.
       */
      NssFile() ;
      
      /** Deconstructor.
       */
      ~NssFile() ;

      /** Method to load a Nyx Spooky Skeleton from disk.
       * @param path The path to the file on the filesystem.
       * @return Whether the file was successful in being loaded or not.
       */
      bool load( const char* path ) ;
      
      /** Method to load a Nyx Spooky Skeleton file from preloaded bytes.
       * @param bytes The bytes of the .ngg file.
       * @param size The size of the byte array.
       * @return Whether or not the file was successfully loaded.
       */
      bool load( const unsigned char* bytes, unsigned size ) ;
      
      /** Method to reset this object and release all allocated data.
       */
      void reset() ;
      
      /** Method to retrieve the amount of animations loaded by this object.
       * @return The amount of animations loaded by this object.
       */
      unsigned animationCount() const ;
      
      /** Method to return the root bone of this file's loaded data.
       * @return The root bone of this file's loaded data.
       */
      const Bone& root() const ;

      /** Method to retrieve the reference to the specified animation.
       * @param index The index of animation to retrieve.
       * @return Const Reference to loaded animation at specified index.
       */
      const Animation& animation( unsigned index ) ;
      
      /** Method to retrieve the amount of bones loaded by this object.
       * @return The amount of bones loaded by this object.
       */
      unsigned boneCount() const ;
      
      /** Method to retrieve the reference to the specified bone.
       * @param index The index of bone to retrieve.
       * @return Const Reference to loaded bone at specified index.
       */
      const Bone& bone( unsigned index ) ;
      
      /**
       * @param name
       * @return 
       */
      bool has( const char* name ) const ;
      
      /**
       * @param name
       * @return 
       */
      const Bone& bone( const char* name ) ;

      /** Method to retrieve the version of the loaded file.
       * @return 
       */
      unsigned version() const ;
    private:
      /** Forward declared structure containing this object's data.
       */
      NssFileData *ngt_file_data ;
      
      /** Method to retrieve a reference to this object's internal data structure.
       * @return Reference to this object's internal data structure.
       */
      NssFileData& data() ;
      
      /** Method to retrieve a const-reference to this object's internal data structure.
       * @return Const-reference to this object's internal data structure.
       */
      const NssFileData& data() const ;
  };
}