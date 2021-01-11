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
 * File:   Test.cpp
 * Author: Jordan Hendl
 *
 * Created on December 31, 2020, 1:30 AM
 */

#include "Window.h"
#include <event/Event.h>
#include <KT/Manager.h>
#include <iostream> 
static const unsigned object_input_expected = 250 ;
static const unsigned only_type_expected    = 480 ;
static unsigned object_input = 0 ;
static unsigned only_type    = 0 ;
class Object
{
public :
  void inputB( const kgl::Event& event )
  {
    if( event.key() == kgl::Key::B )
    {
      object_input = object_input_expected ;
    }
  }
};

void onlyKeyUp( const kgl::Event& event )
{
  if( event.type() == kgl::Event::Type::KeyUp )
  {
    only_type = only_type_expected ;
  }
}

static Object               obj     ;
static karma::test::Manager manager ;
static kgl::EventManager    event   ;

bool checkMethodInput()
{
  kgl::Event tmp = kgl::makeKeyEvent( kgl::Event::Type::KeyDown, kgl::Key::B ) ;
  
  event.pushEvent( tmp ) ;
  if( object_input == object_input_expected ) return true ;
  return false ;
}

bool checkTypeOnlyInput()
{
  kgl::Event tmp = kgl::makeKeyEvent( kgl::Event::Type::KeyUp, kgl::Key::A ) ;
  
  event.pushEvent( tmp ) ;
  if( only_type == only_type_expected ) return true ;
  return false ;
}

bool testWindowCreation()
{
  kgl::lx::Window window ;
  
  window.initialize( "Test", 1024, 720 ) ;

  if( window.window() )
  {
    return true ;
  }
  
  return false ;
}
int main() 
{
  event.enroll( &obj, &Object::inputB, kgl::Key::B, "OnlyBMethod" ) ;
  event.enroll( &onlyKeyUp, kgl::Key::A, "OnlyA" ) ;
  manager.add( "1) Linux Event Handler: Method Only Specific Key Events Test."   , &checkMethodInput   ) ;
  manager.add( "2) Linux Event Handler: Function Only Specific Type Events Test.", &checkMethodInput   ) ;
  manager.add( "3) Linux Window Creation Test."                                  , &testWindowCreation ) ;

  return manager.test( karma::test::Output::Verbose ) ;
}

