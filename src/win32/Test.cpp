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

#include "Window.h"
#include <event/Event.h>
#include <Athena/Manager.h>
#include <iostream>
static const unsigned object_input_expected = 250;
static const unsigned only_type_expected = 480;
static unsigned object_input = 0;
static unsigned only_type = 0;
class Object
{
public:
  void inputB(const nyx::Event& event)
  {
    if (event.key() == nyx::Key::B)
    {
      object_input = object_input_expected;
    }
  }
};

void onlyKeyUp(const nyx::Event& event)
{
  if (event.type() == nyx::Event::Type::KeyUp)
  {
    only_type = only_type_expected;
  }
}

static Object            obj     ;
static athena::Manager   manager ;
static nyx::EventManager event   ;

bool checkMethodInput()
{
  nyx::Event tmp = nyx::makeKeyEvent(nyx::Event::Type::KeyDown, nyx::Key::B);

  event.pushEvent(tmp);
  if (object_input == object_input_expected) return true;
  return false;
}

bool checkTypeOnlyInput()
{
  nyx::Event tmp = nyx::makeKeyEvent(nyx::Event::Type::KeyUp, nyx::Key::A);

  event.pushEvent(tmp);
  if (only_type == only_type_expected) return true;
  return false;
}

bool testWindowCreation()
{
  nyx::win32::Window window;

  window.initialize("Test", 1024, 720);

  if (window.handle() )
  {
    return true;
  }

  return false;
}

int main()
{
  event.enroll( &obj, &Object::inputB, nyx::Key::B, "OnlyBMethod" ) ;
  event.enroll( &onlyKeyUp, nyx::Key::A, "OnlyA" ) ;

  manager.add( "1) Win32 Event Handler: Method Only Specific Key Events Test."   , &checkMethodInput   ) ;
  manager.add( "2) Win32 Event Handler: Function Only Specific Type Events Test.", &checkMethodInput   ) ;
  manager.add( "3) Win32 Window Creation Test."                                  , &testWindowCreation ) ;

  return manager.test( athena::Output::Verbose ) ;
}