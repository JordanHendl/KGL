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
 * File:   KgShader.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 2:26 PM
 */

#include "KgShader.h"
#include "Device.h"

namespace kgl
{
  namespace vkg
  {
    struct KgShaderData
    {
      kgl::vkg::Device device ;
      
      /** Default constructor.
       */
      KgShaderData() ;
    };
    
    KgShaderData::KgShaderData()
    {
      
    }

    KgShader::KgShader()
    {
      this->shader_data = new KgShaderData() ;
    }

    KgShader::KgShader( const KgShader& shader )
    {
      this->shader_data = new KgShaderData() ;
      
      *this->shader_data = *shader.shader_data ;
    }

    KgShader::~KgShader()
    {
      delete this->shader_data ;
    }

    KgShader& KgShader::operator=( const KgShader& shader )
    {
      *this->shader_data = *shader.shader_data ;
      
      return *this ;
    }

    void KgShader::initialize( const kgl::vkg::Device& device, const char* kg_path )
    {
      data().device = device ;
      kg_path += 1 ;
    }

    void KgShader::initializePreloaded( const kgl::vkg::Device& device, const char* kg_data )
    {
      data().device = device ;
      
      kg_data += 1 ;
    }

    unsigned KgShader::numStages() const
    {
      return 1 ;
    }

    unsigned KgShader::numVertexAttributes() const
    {
      return 1 ;
    }

    unsigned KgShader::numVertexBindings() const
    {
      return 1 ;
    }

//    const vk::VertexInputAttributeDescription* KgShader::attributes() const
//    {
//      
//    }
//
//    const vk::VertexInputBindingDescription* KgShader::bindings() const
//    {
//    
//    }
//
//    const vk::PipelineShaderStageCreateInfo* KgShader::infos() const
//    {
//    
//    }

    void KgShader::reset()
    {
    
    }

    KgShaderData& KgShader::data()
    {
      return *this->shader_data ;
    }

    const KgShaderData& KgShader::data() const
    {
      return *this->shader_data ;
    }
  }
}

