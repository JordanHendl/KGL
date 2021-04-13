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
 * File:   Pipeline.h
 * Author: Jordan Hendl
 *
 * Created on December 30, 2020, 1:21 AM
 */

#pragma once

namespace vk
{
  class PipelineLayout ;
  class Pipeline       ;
}

namespace nyx
{ 
  class Viewport ;
  
  namespace vkg
  {
    class Device        ;
    class RenderPass    ;
    class CommandBuffer ;
    class NyxShader     ;
    
    class Pipeline
    {
      public:
        
        /** Default constructor. Initializes this object's data.
         */
        Pipeline() ;
        
        /** Copy constructor. Assigns this object to the input.
         * @param pipeline The pipeline to assign this object to.
         */
        Pipeline( const Pipeline& pipeline ) ;
        
        /** Default deconstructor.
         */
        ~Pipeline() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param pipeline The pipeline object to assign this object to.
         * @return Reference to this object after assignment.
         */
        Pipeline& operator=( const Pipeline& pipeline ) ;
        
        /** Conversion operator to the implementation-specific pipeline object.
         * @return The vulkan pipeline underlying this object
         */
        operator ::vk::Pipeline&() ;

        /** Conversion operator to the implementation-specific pipeline object.
         * @return The vulkan pipeline underlying this object
         */
        operator const ::vk::Pipeline&() ;
        
        /** Method to initialize this compute pipeline with the input shader.
         * @param device The device to use for generating this compute pipeline.
         * @param nyx_path The path to the NyxShader file on disk to use for this pipeline.
         */
        void initialize( unsigned device, const char* nyx_path ) ;

        /** Method to initialize this graphics pipeline with the input renderpass and shader.
         * @param pass The render pass to use for this pipeline creation.
         * @param nyx_path The path to the NyxShader file on disk to use for this pipeline.
         */
        void initialize( const nyx::vkg::RenderPass& pass, const char* nyx_path ) ;
        
        /** Method to initialize this compute pipeline with the input shader.
         * @param device The device to use for generating this compute pipeline.
         * @param nyx_bytes The path to the NyxShader file on disk to use for this pipeline.
         * @param size The size in bytes of the array.
         */
        void initialize( unsigned device, const unsigned char* nyx_bytes, unsigned size ) ;

        /** Method to initialize this graphics pipeline with the input renderpass and shader.
         * @param pass The render pass to use for this pipeline creation.
         * @param nyx_bytes The path to the NyxShader file on disk to use for this pipeline.
         * @param size The size in bytes of the array.
         */
        void initialize( const nyx::vkg::RenderPass& pass, const unsigned char* nyx_bytes, unsigned size ) ;
        
        /** Method to initialize this compute pipeline with the input shader.
         * @param shader The shader to use to describe this pipeline.
         */
        void initialize( const NyxShader& shader ) ;
        
        /** Method to initialize this render pipeline with the inpuit render pass and shader.
         * @param pass The render pass to use for this pipeline creation.
         * @param shader The KgShader to describe this pipeline.
         */
        void initialize( const nyx::vkg::RenderPass& pass, const NyxShader& shader ) ;
        
        /**
         * @return 
         */
        bool initialized() const ;

        /** Method to set the push constant size in bytes for this pipeline.
         * @return The push constant size in bytes.
         */
        void setPushConstanceByteSize( unsigned size ) ;
        
        /** Method to set the push constant pipeline bitflag.
         * @param stage The stages the push constant is accessed from.
         */
        void setPushConstantStageFlag( unsigned stage ) ;

        /** Method to release all allocation by this object.
         */
        void reset() ;
        
        /** Method to see if this is a graphics pipeline or not.
         * @return Whether or not this object is a graphics pipeline.
         */
        bool isGraphics() const ;
        
        /** Method to add a viewport to this pipeline.
         * @param viewport The library viewport to add.
         */
        void addViewport( const nyx::Viewport& viewport ) ;
        
        /** Method to retrieve a const-reference to this object's internal vulkan pipeline.
         * @return The internal vulkan pipeline of this object.
         */
        const vk::Pipeline& pipeline() const ;
        
        /** Method to retrieve the internal vulkan pipeline layout of this object.
         * @return The internal vulkan pipeline layout of this object.
         */
        const vk::PipelineLayout& layout() const ;
        
        /** Method to set whether this object should write depth values out.
         * @param val Whether or not this object should write depth values out.
         */
        void setTestDepth( bool val ) ;
        
        /** Method to retrieve reference to this object's vulkan shader generator.
         * @return Reference to this object's to this object's vulkan shader generator.
         */
        const nyx::vkg::NyxShader& shader() const ;
        
      private:
      
        /** Forward Declared structure to contain this object's internal data.
         */
        struct PipelineData *pipeline_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        PipelineData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const PipelineData& data() const ;
    };
  }
}