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

/* 
 * File:   KgShader.h
 * Author: Jordan Hendl
 *
 * Created on December 30, 2020, 2:26 PM
 */
#pragma once

/** Vulkan C typedefs for forward decleration.
 */
typedef unsigned VkFlags            ;
typedef VkFlags  VkShaderStageFlags ;

/** Vulkan C++ forward declerations.
 */
namespace vk
{
   template <typename BitType>
   class Flags ;

  class VertexInputAttributeDescription               ;
  class VertexInputBindingDescription                 ;
  class PipelineShaderStageCreateInfo                 ;
  class DescriptorSetLayout                           ;
  enum class Format                                   ;
  enum class VertexInputRate                          ;
  enum class DescriptorType                           ;
  enum class ShaderStageFlagBits : VkShaderStageFlags ;
  
  using ShaderStageFlags = ::vk::Flags<::vk::ShaderStageFlagBits> ; ///< Forward declared vulkan.hpp flags.
}

namespace nyx
{
  class ShaderIterator ;
  class NyxFile ;
  
  /** Forward declared array flags.
   */
  class ArrayFlags ;
  
  /** Forward declared shader stage enumeration.
   */
  enum ShaderStage : unsigned ;

  /** Forward declared image usage enumeration.
   */
  enum class ImageUsage : unsigned ;

  namespace vkg
  {
    class Device ;
    
    /** Class for managing vulkan shaders & importing KgShaders.
     */
    class NyxShader
    {
      public:
        
        /** The format of variable allowed.
         */
        enum class Format
        {
          mat4,
          vec4,
          ivec4,
          uvec4,
          mat3,
          vec3,
          ivec3,
          uvec3,
          mat2,
          vec2,
          ivec2,
          uvec2
        };
        
        
        /** The rate of vertex being input.
         */
        enum class InputRate
        {
          Vertex,
          Instanced,
        };

        /** Default constructor.
         */
        NyxShader() ;
        
        /** Copy constructor. Assigns this object to the input.
         * @param shader The shader to assign this object to.
         */
        NyxShader( const NyxShader& shader ) ;
        
        /** Default deconstructor.
         */
        ~NyxShader() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param shader The shader to assign this object to.
         * @return Reference to this object after assignment.
         */
        NyxShader& operator=( const NyxShader& shader ) ;
        
        /** Method to retrieve the end of this shader's data.
         * @return A const-reference to the container for this object's shader info.
         */
        const NyxFile& file() const ;
        
        /** Method to initialize this shader with the provided input.
         * @param device The library device to use for all vulkan operations.
         * @param nyx_path The C-string path to the .nyx file in the filesystem.
         */
        void initialize( unsigned device, const char* nyx_path ) ;
        
        /** Method to initialize this shader with the provided input.
         * @param device The library device to use for all vulkan operations.
         * @param nyx_path The C-string path to the .nyx file in the filesystem.
         * @param size The size of the byte array in elements.
         */
        void initialize( unsigned device, const unsigned char* nyx_bytes, unsigned size ) ;
        
        /** Method to initialize this shader with the provided input.
         * @note Initializes all data set manually.
         * @param device The library device to use for all vulkan operations.
         */
        void initialize( unsigned device ) ;
        
        /** Method to retrieve the number of shader stages this shader contains.
         * @return The number of shader stages this shader contains.
         */
        unsigned numStages() const ;
        
        /** Method to retrieve the number of vulkan vertex attributes created by this object.
         * @return The count of vertex attributes created by this object.
         */
        unsigned numVertexAttributes() const ;
        
        /** Method to retrieve the number of vulkan vertex bindings created by this object.
         * @return The count of vertex bindings created by this object.
         */
        unsigned numVertexBindings() const ;
        
        /** Method to manually add an attribute to this shader.
         * @param location The location of the attribute.
         * @param binding The binding of the attribute.
         * @param format The vulkan format of the attribute.
         * @param offset The offset of the attribute.
         */
        void addAttribute( unsigned location, unsigned binding, const NyxShader::Format& format, unsigned offset = 0 ) ;
        
        /** Method to manually add a descriptor to this shader.
         * @param binding The binding number of the entry, corresponds to a resource of the same binding in the shader stages.
         * @param type  Which type of resources are used for this binding.
         * @param count The number of descriptors contained in the binding ( e.g. an array in the shader ).
         * @param stage The bitmask of shader flags specifying which pipeline shader stage can access this resource.
         */
        void addDescriptor( unsigned binding, const nyx::ArrayFlags& type, unsigned count, nyx::ShaderStage stage ) ;
        
        /** Method to manually add a descriptor to this shader.
         * @param binding The binding number of the entry, corresponds to a resource of the same binding in the shader stages.
         * @param type  Which type of resources are used for this binding.
         * @param count The number of descriptors contained in the binding ( e.g. an array in the shader ).
         * @param stage The bitmask of shader flags specifying which pipeline shader stage can access this resource.
         */
        void addDescriptor( unsigned binding, nyx::ImageUsage type, unsigned count, nyx::ShaderStage stage ) ;

        /** Method to manually add an input binding to the shader.
         * @param binding The binding number of the input.
         * @param stride The stride distance in bytes between elements.
         * @param rate Whether it is verted index or instance index addressed.
         */
        void addInputBinding( unsigned binding, unsigned stride, InputRate rate ) ;
        
        /** Method to manually add a SPIRV shader module to this object.
         * @param stage The shader flags to associated with this shader.
         * @param spirv const pointer to the start of the SPIRV byte code.
         * @param size The size in of the SPIRV byte code in elements.
         */
        void addShaderModule( nyx::ShaderStage stage, const unsigned* spirv, unsigned size ) ;
        
        /** Method to retrieve the device used for this object's creation.
         * @return Const reference to the device used for this object's creation.
         */
        unsigned device() const ;

        /** Method to retrieve the descriptor set layout of this vulkan shader.
         * @return The descriptor set layout of this vulkan shader.
         */
        const vk::DescriptorSetLayout& layout() const ;

        /** Method to retrieve a const pointer to the start of this object's generated attribute data.
         * @return Const-pointer to the start of this object's generated attribute data.
         */
        const vk::VertexInputAttributeDescription* attributes() const ;
        
        /** Method to retrieve a const pointer to the start of this object's generated binding data.
         * @return Const-pointer to the start of this object's generated binding data.
         */
        const vk::VertexInputBindingDescription* bindings() const ;
        
        /** Method to retrieve a const pointer to the start of this object's generated pipeline stage data.
         * @return Const-pointer to the start of this object's generated pipeline stage data.
         */
        const vk::PipelineShaderStageCreateInfo* infos() const ;
        
        /** Method to release all allocation by this object.
         */
        void reset() ;
        
      private:
        
        /** Forward Declared structure to contain this object's internal data.
         */
        struct NyxShaderData *shader_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        NyxShaderData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const NyxShaderData& data() const ;
    };
  }
}

