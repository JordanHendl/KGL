#include "Memory.h"

namespace nyx
{
    MemoryFlags::MemoryFlags()
  {
    this->bit = MemoryFlags::DeviceLocal ;
  }
  
  MemoryFlags::MemoryFlags( int flags )
  {
    this->set( flags ) ;
  }

  MemoryFlags& MemoryFlags::operator=( int flag )
  {
    this->set( flag ) ;
    
    return *this ;
  }
  
  MemoryFlags& MemoryFlags::operator|( int flag )
  {
    this->set( this->bit | flag ) ;
    
    return *this ;
  }
  
  MemoryFlags::operator int() const
  {
    return this->value() ;
  }
  
  int MemoryFlags::value() const
  {
    return this->bit ;
  }
  
  void MemoryFlags::set( int value )
  {
    this->bit = value ;
  }
}



