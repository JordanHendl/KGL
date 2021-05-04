/*
 * Copyright (C) 2021 Jordan Hendl
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

/* 
 * File:   Descriptor.h
 * Author: Jordan Hendl
 *
 * Created on January 31, 2021, 4:29 AM
 */

#pragma once

namespace vk
{
  // Forward declared vullkan objects.
  class DescriptorSet       ;
  class DescriptorSetLayout ;
}

namespace nyx
{
  enum class ImageFormat : unsigned ;
  enum class ImageUsage  : unsigned ;

  /** Forward declared array object.
   */
  template<typename Impl, typename Type>
  class Array ;
  
  /** Forward declared array flags.
   */
  class ArrayFlags ;
  
  /** Forward declared image.
   */
  template<typename Framework>
  class Image ;
  
  /** Forward declared nyx file.
   */
  class NyxFile ;
  namespace vkg
  {
    class Vulkan    ;
    class Buffer    ;
    class Image     ;
    class NyxShader ;

    class DescriptorPool ;

    /** Class to describe memory access in GPU shader programs.
     */
    class Descriptor
    {
      public:
        
        /** Default constructor.
         */
        Descriptor() ;

        /** Copy constructor.
         * @param desc The object to copy.
         */
        Descriptor( const Descriptor& desc ) ;

        /** Default deconstructor.
         */
        ~Descriptor() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param desc The descriptor to assign this object to.
         * @return Reference to this object after assignment.
         */
        Descriptor& operator=( const Descriptor& desc ) ;
        
        /** Alternative method to initialize this object if not grabbing from a pool.
         * @param pool The pool to initialize this object with.
         */
        void initialize( const vkg::DescriptorPool& pool ) ;
        
        /** Method to retrieve the vulkan descriptor for this object.
         * @return Const reference to the vulkan descriptor of this object.
         */
        const vk::DescriptorSet& set() const ;
        
        /** Method to reset this descriptor, if allowed.
         */
        void reset() ;

        /** Method to set an array into this object's descriptor.
         * @param name The name of variable to set.
         * @param array The array to bind to the data.
         */
        template<typename Type>
        void set( const char* name, const nyx::Array<vkg::Vulkan, Type>& array ) ;
        
        /** Base method to set a vulkan image to this object.
         * @param name The name to associate with the variable.
         * @param image The image to bind.
         */
        void set( const char* name, const nyx::vkg::Image& image ) ;
        
        /** Base method to set a vulkan image to this object.
         * @param name The name to associate with the variable.
         * @param image The image to bind.
         */
        void set( const char* name, const nyx::vkg::Image* const* images, unsigned count ) ;
        
      private:
        
        /** Base method to set a vulkan buffer to this object.
         * @param name The name to associate with the variable.
         * @param buffer The buffer to bind.
         */
        void set( const char* name, const nyx::vkg::Buffer& buffer ) ;
        
        
        /** Friend declaration.
         */
        friend class DescriptorPool ;
        friend class RendererImpl   ;
        
        /** Forward declared structure containing this object's data.
         */
        struct DescriptorData *desc_data ;

        /** Method to retrieve a reference to this object's internal data structure.
         * @return Reference to this object's internal data structure.
         */
        DescriptorData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data structure.
         * @return Const-reference to this object's internal data structure.
         */
        const DescriptorData& data() const ;
    };
    
    /** Class to manage descriptor creation.
     */
    class DescriptorPool
    {
      public:
        
        /** Default constructor.
         */
        DescriptorPool() ;
        
        /** Default deconstructor
         */
        ~DescriptorPool() ;
        
        /** Method to initialize this object.
         * @param shader The shader to use for reflection.
         */
        void initialize() ;
        
        /** Method to initialize this object.
         * @param shader The shader to use for reflection.
         * @param amount The amount of descriptors to create.
         */
        void initialize( const nyx::vkg::NyxShader& shader, unsigned amount = 1 ) ;
        
        /** Method to manually add an input to this pool.
         * @param name The name to associate with the input.
         * @param binding The binding to associate with the input.
         * @param type The type to associate with the input.
         */
        void addArrayInput( const char* name, unsigned binding, const nyx::ArrayFlags& type ) ;
        
        /** Method to manually add an input to this pool.
         * @param name The name to associate with the input.
         * @param binding The binding to associate with the input.
         * @param usage The type to associate with the input.
         */
        void addImageInput( const char* name, unsigned binding, nyx::ImageUsage usage ) ;
        
        /** Method to set the amount of descriptors to generate.
         * @param amount The amount of descriptors to be allowed to generate.
         */
        void setAmount( unsigned amount ) ;
        
        /** Method to manually set the descriptor set layout of this object.
         * @param layout The layout to set.
         */
        void setLayout( const vk::DescriptorSetLayout& layout ) ;
        
        /** Method to create a descriptor from this object.
         * @return A newly make descriptor of this object.
         */
        nyx::vkg::Descriptor make() ;

      private:
      
        /** Friend declaration.
         */
        friend class Descriptor ;

        /** Copying is disallowed.
         */
        DescriptorPool( const DescriptorPool& pool ) ;
        
        /** Assignment is disallowed.
         */
        const DescriptorPool& operator=( const DescriptorPool& pool ) ;
        
        /** Forward declared structure containing this object's data.
         */
        struct DescriptorPoolData* pool_data ;
        
        /** Method to retrieve a reference to this object's internal data structure.
         * @return Reference to this object's internal data structure.
         */
        DescriptorPoolData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data structure.
         * @return Const-reference to this object's internal data structure.
         */
        const DescriptorPoolData& data() const ;
    };
    
    template<typename Type>
    void Descriptor::set( const char* name, const nyx::Array<vkg::Vulkan, Type>& array )
    {
      this->set( name, array.buffer() ) ;
    }
  }
}

