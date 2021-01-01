GPU Library for personal use with Karma framework.

Library is intended to be a generic gpgpu library that enables development using whatever platform and API.

This is done through template implementations, e.g.

```
// Generic Memory Object
#include <Memory.h>

// For vulkan API.
#include <vkg/Vulkan.h>

// For CUDA API.
#include <cg/Cuda.h>

int main()
{
  // Aliases for API's.
  using VULKAN = kgl::vkg::Vulkan ;
  using CUDA   = kgl::cg::Cuda    ;

  kgl::Memory<CUDA  > cuda_memory   ;
  kgl::Memory<VULKAN> vulkan_memory ;

  // do things with them here.

  return 0 ;
}
```


For usages of each specific API, I suggest checking out the Test.cpp in each one!
