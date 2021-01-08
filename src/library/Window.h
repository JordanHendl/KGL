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
 * File:   Window.h
 * Author: Jordan Hendl
 *
 * Created on December 30, 2020, 8:14 PM
 */

#ifndef KGL_WINDOW_H
#define KGL_WINDOW_H

#ifdef _WIN32
  #include "./windows/Win32.h"
  #include "./windows/Window.h"
#elif __linux__ 
  #include "./linux/Linux.h"
  #include "./linux/Window.h"
#endif

namespace kgl
{
  template<typename OS, typename API>
  class BaseWindow
  {
    public:
      
      /** Default constructor.
       */
      BaseWindow() = default ;
      
      /** Default deconstructor.
       */
      ~BaseWindow() = default ;
      
      /** Method to initialize this object.
       * @param window_name The title of the window. 
       * @param width The width of the window in pixels.
       * @param height The height of the window in pixels.
       */
      void initialize( const char* window_title, unsigned width, unsigned height ) ;
      
      /** Method to process events being sent to the window.
       */
      void handleEvents() ;
        
      /** Method to reset this object back to it's initial state.
       */
      void reset() ;

      /** Method to set the monitor to put the window on.
       * @param monitor_id The ID of the monitor to put the window on.
       */
      void setMonitor( unsigned monitor_id ) ;
      
      /** Method to set whether the window should be fullscreen or not.
       * @param value Whether or not the window should be fullscreen.
       */
      void setFullscreen( bool value ) ;
      
      /** Method to set whether or not the window should be resizable.
       * @param value Whether or not the window should be resizable.
       */
      void setResizable( bool value ) ;
      
      /** Method to set whether or not the window should be borderless.
       * @param value Whether or not the window should be borderless.
       */
      void setBorderless( bool value ) ;
      
      /** Method to set whether or not the window should be minimized or not.
       * @param value Whether or not the window should be minimized.
       */
      void setMinimize( bool value ) ;

      /** Method to set whether or not the window should be maximized or not.
       * @param value Whether or not the menu should be maximized.
       */
      void setMaximized( bool value ) ;
      
      /** Method to retrieve the Library-OS specific window from this object.
       * @return The Library-OS specific window from this object.
       */
      const typename OS::Window& window() const ;

      /** Method to retrieve the Graphics API-specific context from this window.
       * @return The graphics API-specific context from this window.
       */
      const typename API::Context& context() const ;
    private:
      typename OS::Window   os_window   ;
      typename API::Context api_context ;
  };
  
  template<typename OS, typename API>
  void BaseWindow<OS, API>::initialize( const char* window_title, unsigned width, unsigned height )
  {
    os_window.initialize( window_title, width, height ) ;
    
    this->api_context = API::contextFromBaseWindow( this->os_window ) ;
  }
  
  template<typename OS, typename API>
  void BaseWindow<OS, API>::reset()
  {
    os_window.reset() ;
  }

  template<typename OS, typename API>
  void BaseWindow<OS, API>::setMonitor( unsigned monitor_id )
  {
    os_window.setMonitor( monitor_id ) ;
  }

  template<typename OS, typename API>
  void BaseWindow<OS, API>::setFullscreen( bool value )
  {
    os_window.setFullscreen( value ) ;
  }

  template<typename OS, typename API>
  void BaseWindow<OS, API>::setResizable( bool value )
  {
    os_window.setResizable( value ) ;
  }

  template<typename OS, typename API>
  void BaseWindow<OS, API>::setBorderless( bool value )
  {
    os_window.setBorderless( value ) ;
  }

  template<typename OS, typename API>
  void BaseWindow<OS, API>::setMinimize( bool value )
  {
    os_window.setMinimize( value ) ;
  }

  template<typename OS, typename API>
  void BaseWindow<OS, API>::setMaximized( bool value )
  {
    os_window.setMaximize( value ) ;
  }

  template<typename OS, typename API>
  const typename API::Context& BaseWindow<OS, API>::context() const
  {
    return this->api_context ;
  }
  
  template<typename OS, typename API>
  const typename OS::Window& BaseWindow<OS, API>::window() const 
  {
    return this->os_window ;
  }
  
  template<typename OS, typename API>
  void BaseWindow<OS, API>::handleEvents()
  {
    this->os_window.handleEvents() ;
  }
  #ifdef _WIN32
    template<typename API>
    using Window = kgl::BaseWindow<kgl::win32::Win32, API> ;
  #elif __linux__ 
    template<typename API>
    using Window = kgl::BaseWindow<kgl::lx::Linux, API> ;
  #endif
}
#endif

