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

namespace kgl
{
  template<typename OS, typename API>
  class Window
  {
    public:
      
      /** Default constructor.
       */
      Window() = default ;
      
      /** Default deconstructor.
       */
      ~Window() = default ;
      
      /** Method to initialize this object.
       * @param window_name The title of the window. 
       * @param width The width of the window in pixels.
       * @param height The height of the window in pixels.
       */
      void initialize( const char* window_title, unsigned width, unsigned height ) ;
      
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
  void Window<OS, API>::initialize( const char* window_title, unsigned width, unsigned height )
  {
    os_window.initialize( window_title, width, height ) ;
    
    this->api_context = API::contextFromWindow( this->os_window ) ;
  }

  template<typename OS, typename API>
  void Window<OS, API>::setMonitor( unsigned monitor_id )
  {
    os_window.setMonitor( monitor_id ) ;
  }

  template<typename OS, typename API>
  void Window<OS, API>::setFullscreen( bool value )
  {
    os_window.setFullscreen( value ) ;
  }

  template<typename OS, typename API>
  void Window<OS, API>::setResizable( bool value )
  {
    os_window.setResizable( value ) ;
  }

  template<typename OS, typename API>
  void Window<OS, API>::setBorderless( bool value )
  {
    os_window.setBorderless( value ) ;
  }

  template<typename OS, typename API>
  void Window<OS, API>::setMinimize( bool value )
  {
    os_window.setMinimize( value ) ;
  }

  template<typename OS, typename API>
  void Window<OS, API>::setMaximized( bool value )
  {
    os_window.setMaximize( value ) ;
  }

  template<typename OS, typename API>
  const typename API::Context& Window<OS, API>::context() const
  {
    return this->api_context ;
  }
  
  template<typename OS, typename API>
  const typename OS::Window& Window<OS, API>::window() const 
  {
    return this->os_window ;
  }
}
#endif

