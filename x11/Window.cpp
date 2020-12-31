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
 * File:   Window.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 9:00 PM
 */

#include "Window.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <string>

namespace kgl
{
  namespace x11
  {
    struct WindowData
    {
      ::Display  *dis        ;
      ::Window    win        ;
      ::GC        gc         ;
      int         screen     ;
      std::string title      ;
      unsigned    width      ;
      unsigned    height     ;
      unsigned    monitor    ;
      bool        fullscreen ;
      bool        borderless ;
      bool        minimized  ;
      bool        maximized  ;
      bool        resizable  ;

      /** Default constructor.
       */
      WindowData() ;
      
      /** Method to create the X11 window.
       */
      void create() ;
    };
    
    WindowData::WindowData()
    {
      
    }
    
    void WindowData::create()
    {
      
    }

    Window::Window()
    {
      this->window_data = new WindowData() ;
    }
    
    Window::~Window()
    {
      delete this->window_data ;
    }
    
    void Window::initialize( const char* window_title, unsigned width, unsigned height )
    {
      data().title  = window_title ;
      data().width  = width        ;
      data().height = height       ;
      
      data().create() ;
    }
    
    void Window::setMonitor( unsigned monitor_id )
    {
      data().monitor = monitor_id ;
    }
    
    void Window::setFullscreen( bool value )
    {
      data().fullscreen = value ; 
    }

    void Window::setResizable( bool value )
    {
      data().resizable = value ;
    }

    void Window::setBorderless( bool value )
    {
      data().borderless = value ;
    }

    void Window::setMinimize( bool value )
    {
      data().minimized = value ;
    }

    void Window::setMaximized( bool value )
    {
      data().maximized = value ;
    }

    WindowData& Window::data()
    {
      *this->window_data ;
    }

    const WindowData& Window::data() const
    {
      *this->window_data ;
    }
  }
}

