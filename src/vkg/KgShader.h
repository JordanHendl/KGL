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

#ifndef KGL_VKG_KGSHADER_H
#define KGL_VKG_KGSHADER_H

namespace vk
{
  class VertexInputAttributeDescription ;
  class VertexInputBindingDescription   ;
  class PipelineShaderStageCreateInfo   ;
}

namespace kgl
{
  namespace vkg
  {
    class Device ;
    
    /** Class for managing vulkan shaders & importing KgShaders.
     */
    class KgShader
    {
      public:
        
        /** Default constructor.
         */
        KgShader() ;
        
        /** Copy constructor. Assigns this object to the input.
         * @param shader The shader to assign this object to.
         */
        KgShader( const KgShader& shader ) ;
        
        /** Default deconstructor.
         */
        ~KgShader() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param shader The shader to assign this object to.
         * @return Reference to this object after assignment.
         */
        KgShader& operator=( const KgShader& shader ) ;
        
        /** Method to initialize this shader with the provided input.
         * @param device The library device to use for all vulkan operations.
         * @param kg_path The C-string path to the .kg file in the filesystem.
         */
        void initialize( const kgl::vkg::Device& device, const char* kg_path ) ;
        
        /** Method to initialize this shader with the provided input.
         * @param device The library device to use for all vulkan operations.
         * @param kg_data The binrary data of the .kg file to use for this shader.
         */
        void initializePreloaded( const kgl::vkg::Device& device, const char* kg_data ) ;
        
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
        struct KgShaderData *shader_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        KgShaderData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const KgShaderData& data() const ;
    };
  }
}
#endif /* KGSHADER_H */

