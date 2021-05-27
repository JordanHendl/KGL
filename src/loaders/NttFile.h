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
  struct NttFileData ;

  /** Class to manage loading Nyx Spooky Skeleton files.
   */
  class NttFile
  {
    public:
      
      struct ivec2
      {
        int x ;
        int y ;
      };
      
      struct Character
      {
        ivec2    size    ;
        ivec2    bearing ;
        unsigned advance ;
      };

      /** Constructor.
       */
      NttFile() ;
      
      /** Deconstructor.
       */
      ~NttFile() ;

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
      
      /** Method to keep the characters in memory, but deallocate the image buffers.
       */
      void deallocate() ;

      /** Method to reset this object and release all allocated data.
       */
      void reset() ;
      
      /** Method to retrieve the version of the loaded file.
       * @return 
       */
      unsigned version() const ;
      
      const Character* characters() const ;
      
      unsigned characterCount() const ;
      
      const Character& character( unsigned char character ) const ;
      
      const unsigned char* characterImage( unsigned char character ) const ;
      
      unsigned characterImageSize( unsigned char character ) const ;
    private:
      /** Forward declared structure containing this object's data.
       */
      NttFileData *ntt_file_data ;
      
      /** Method to retrieve a reference to this object's internal data structure.
       * @return Reference to this object's internal data structure.
       */
      NttFileData& data() ;
      
      /** Method to retrieve a const-reference to this object's internal data structure.
       * @return Const-reference to this object's internal data structure.
       */
      const NttFileData& data() const ;
  };
}