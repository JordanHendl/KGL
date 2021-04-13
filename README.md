# NYX
  Nyx is a GPU library aimed to supply a generic interface to interact with the GPU, while also providing specific libraries for API's and OS's for more fine-grained access if needed.

## How to build and install
  Nyx is built using **CMake**. If on linux, to build from source, simply do: 
  
  ```
  mkdir build
  cd build
  cmake ..
  make 
  ```
  
  For Windows, do 
  
  ```
  mkdir build
  cd build
  cmake ..
  ```
  
  And then simply open the generated .sln with whatever editor of your preference and build.

  Generated RPM's default install to '/usr/local/lib/Nyx' on UNIX, and 'C:\Program Files\Nyx' on Windows.

  Using with CMake: 
  1) Add the path to the install to your *CMAKE_PREFIX_PATH*.
  2) ```FIND_PACKAGE( nyxgpu ) ```
  3) Link against any Nyx library ( nyx, nyx_vkg, nyx_linux, etc. ) you need!
  
## How to use
  The core library is intended to be a templated GPGPU library that enables development using whatever platform and API.

### Example 1 :
  ```
  // Generic Memory Object
  #include <library/Memory.h>

  // For vulkan API.
  #include <vkg/Vulkan.h>

  // For CUDA API.
  #include <cg/Cuda.h>

  int main()
  {
    // Aliases for API's.
    using VULKAN = nyx::vkg::Vulkan ;
    using CUDA   = nyx::cg::Cuda    ;

    nyx::Memory<CUDA  > cuda_memory   ;
    nyx::Memory<VULKAN> vulkan_memory ;
  
    // do things with them here.

    return 0 ;
  }
  ```

For more examples, I suggest looking at the test files in the vulkan/cuda libraries.

## Dependancies

  Base, Nyx depends on nothing. However, if it can find the libraries on the system, it will build what it can. 
 
  E.g. If vulkan is found, Nyx Vulkan libs will be build.

  XCB is used for Linux windowing, however it should be already installed on most systems.

  AthenaLib is used for testing. To output tests, install the athena testing library. ( See https://github.com/JordanHendl/athena )

## Closing
  For usages of each specific API, I suggest checking out the Test.cpp in each one!

  If you have any questions or suggestions shoot me an email at jordiehendl@gmail.com
  or hit me up on twitter ( twitter.com/overcasterisk )
