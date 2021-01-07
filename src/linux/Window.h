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
 * Created on December 30, 2020, 9:00 PM
 */

#ifndef KGL_LINUX_WINDOW_H
#define KGL_LINUX_WINDOW_H

typedef struct xcb_connection_t xcb_connection_t ; 
typedef        unsigned         xcb_window_t     ;
namespace kgl
{
  namespace lx
  {
    class Window
    {
      public:
        
        /** Default constructor.
         */
        Window() ;
        
        /** Default deconstructor.
         */
        ~Window() ;
        
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
        
        /** Method to process events being sent to the window.
         */
        void handleEvents() ;
        
        /** Method to reset this object to it's initial state.
         */
        void reset() ;

        /** Method to retrieve the implementation-specific connection object.
         * @return The implementation-specific connection object.
         */
        xcb_connection_t* connection() const ;
        
        /** Method to retrieve the implementation-specific window object.
         * @return The implementation-specific window object.
         */
        xcb_window_t window() const ; 

      private:
        
        /** Forward-declared structure to contain this object's internal data.
         */
        struct WindowData* window_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        WindowData& data() ;
        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const WindowData& data() const ;
    };
  }
}

#endif /* WINDOW_H */

