/*
 * The MIT License
 *
 * Copyright 2020 Jordan Hendl.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   Test.cpp
 * Author: jhendl
 *
 * Created on December 23, 2020, 10:18 PM
 */

#include "vkg/Vulkan.h"
#include "vkg/Device.h"
#include "vkg/Instance.h"
#include "Memory.h"
#include <vulkan/vulkan.hpp>

int main()
{
  using Impl = ::kgl::vkg::Vulkan ;
  
  kgl::vkg::Instance instance   ;
  kgl::vkg::Device   device     ;
  kgl::Memory<Impl>  memory_one ;
  kgl::Memory<Impl>  memory_two ;
  
  instance.setApplicationName( "KGL-VKG Test App" ) ;
  
  instance.initialize() ;
  device.initialize( instance.device( 0 ) ) ;
  
  memory_one.initialize<float>( 200, device, true, vk::MemoryPropertyFlagBits::eDeviceLocal ) ;
  
  return 0 ;
}