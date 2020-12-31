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
#include <chrono>
#include <thread>

int main(int argc, char** argv) 
{
  kgl::lx::Window window ;
  
  window.initialize( "Test", 1024, 720 ) ;
  
  std::this_thread::sleep_for( std::chrono::seconds( 5 ) ) ;
  return 0;
}

