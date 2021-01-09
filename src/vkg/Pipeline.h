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

#ifndef KGL_VKG_PIPELINE_H
#define KGL_VKG_PIPELINE_H

namespace vk
{
  class PipelineLayout ;
  class Pipeline       ;
}

namespace kgl
{
  namespace vkg
  {
    class Device        ;
    class RenderPass    ;
    class CommandBuffer ;
    class KgShader      ;
    
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
        
        /** Method to initialize this graphics pipeline with the input renderpass and shader.
         * @param pass The render pass to use for this pipeline creation.
         * @param kg_path The path to the KgShader file on disk to use for this pipeline.
         */
        void initialize( const kgl::vkg::RenderPass& pass, const char* kg_path ) ;
        
        /** Method to initialize this compute pipeline with the input shader.
         * @param kg_path The path to the KgShader file on disk to use for this pipeline.
         */
        void initialize( const char* kg_path ) ;
        
        /** Method to initialize this graphics pipeline with the input renderpass and shader.
         * @param pass The render pass to use for this pipeline creation.
         * @param kg_bytes The pre-loaded data of a KgShader to use for this object's shader creation
         */        
        void initializePreloaded( const kgl::vkg::RenderPass& pass, const char* kg_bytes ) ;
        
        /** Method to initialize this compute pipeline with the input shader.
         * @param kg_bytes The pre-loaded data of a KgShader to use for this object's shader creation
         */
        void initializePreloaded( const char* kg_bytes ) ;
        
        /** Method to release all allocation by this object.
         */
        void reset() ;
        
        /** Method to retrieve a const-reference to this object's internal vulkan pipeline.
         * @return The internal vulkan pipeline of this object.
         */
        const vk::Pipeline& pipeline() const ;
        
        /** Method to retrieve the internal vulkan pipeline layout of this object.
         * @return The internal vulkan pipeline layout of this object.
         */
        const vk::PipelineLayout& layout() const ;
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

#endif

