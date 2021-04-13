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
  /** Class for loading a NgtFile.
   */
  class NgtFile
  {
    public:
      
      /** Constructor.
       */
      NgtFile() ;
      
      /** Deconstructor.
       */
      ~NgtFile() ;
      
      /** Method to load a Nyx Generated Texture from disk.
       * @param path The path to the file on the filesystem.
       * @return Whether the file was successful in being loaded or not.
       */
      bool load( const char* path ) ;
      
      /** Method to load a Nyx Generated Texture file from preloaded bytes.
       * @param bytes The bytes of the .ngt file.
       * @param size The size of the byte array.
       * @return Whether or not the file was successfully loaded.
       */
      bool load( const unsigned char* bytes, unsigned size ) ;
      
      /** Method to reset this object and release all allocated data.
       */
      void reset() ;
      
      /** Method to retrieve the channels of the loaded image.
       * @return The amount of channels in the loaded image.
       */
      unsigned channels() const ;
      
      /** Method to retrieve the width of the loaded image in pixels.
       * @return The width of the loaded image.
       */
      unsigned width() const ;
      
      /** Method to retrieve the height of the loaded image in pixels.
       * @return The height of the loaded image.
       */
      unsigned height() const ;
      
      /** Method to retrieve the version of the loaded file.
       * @return The version of the loaded file.
       */
      unsigned version() const ;
      
      /** Method to retrieve the pointer to the start of this object's image data.
       * @return The pointer to the start of this object's loaded image data.
       */
      const unsigned char* image() const ;
    private:
      
      /** Forward declared structure containing this object's data.
       */
      struct NgtFileData *ngt_file_data ;

      /** Method to retrieve a reference to this object's internal data structure.
       * @return Reference to this object's internal data structure.
       */
      NgtFileData& data() ;
      
      /** Method to retrieve a const-reference to this object's internal data structure.
       * @return Const-reference to this object's internal data structure.
       */
      const NgtFileData& data() const ;
  };
}