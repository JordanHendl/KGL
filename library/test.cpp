#include "vkg/Vulkan.h"
#include "Memory.h"
#include <vulkan/vulkan.hpp>

int main()
{
  using Impl = ::kgl::vkg::Vulkan ;
  vk::Device device ;
  
  kgl::Memory<Impl> memory_one ;
  kgl::Memory<Impl> memory_two ;
  memory_one.initialize<float>( 200, device, true, vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostCoherent ) ;
  memory_two.initialize<float>( 200, device, true, vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostCoherent ) ;
  
  memory_two.initialize<float>( 200, device, true, vk::MemoryPropertyFlagBits::eDeviceLocal ) ;
// This doesn't work per Khronos Group's weird design with vulkan.hpp.
//  memory_two.initialize<float>( 200, device, true, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::MemoryPropertyFlagBits::eHostCoherent ) ;
  
  
}

