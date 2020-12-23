#include "Memory.h"
#include "Vulkan.h"
#include <vulkan/vulkan.hpp>

int main()
{
  using Impl = kgl::vkg::Vulkan ;
  vk::Device device ;
  
  kgl::Memory<Impl> memory_one ;
  kgl::Memory<Impl> memory_two ;
  memory_one.initialize<float>( 200, device ) ;
  memory_two.initialize<float>( 200, device ) ;
  
  memory_one.copy( memory_two ) ;
}

