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
#include <string>
#include <xcb/xcb.h>

namespace kgl
{
  namespace lx
  {
    struct WindowData
    {
      xcb_connection_t *connection  ;
      xcb_screen_t     *screen      ;
      xcb_window_t      window      ;
      std::string       title       ;
      unsigned          xpos        ;
      unsigned          ypos        ;
      unsigned          width       ;
      unsigned          height      ;
      unsigned          depth       ;
      unsigned          monitor     ;
      bool              fullscreen  ;
      bool              borderless  ;
      bool              minimized   ;
      bool              maximized   ;
      bool              resizable   ;
      

      /** Default constructor.
       */
      WindowData() ;
      
      /** Method to create the X11 window.
       */
      void create() ;
    };
    
    WindowData::WindowData()
    {
      this->borderless = false ;
    }
    
    void WindowData::create()
    {
      const unsigned event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK ;
      
      int                   screen_id       ;
      xcb_screen_iterator_t screen_iter     ;
      unsigned              value_list[ 2 ] ;
      unsigned      border_size ;

      screen_id   = 0                        ;
      border_size = this->borderless ? 0 : 2 ;
      
      this->connection = xcb_connect( nullptr, &screen_id ) ;
      
      if( xcb_connection_has_error( this->connection ) )
      {
        
      }
      
      screen_iter = xcb_setup_roots_iterator( xcb_get_setup( this->connection ) ) ;
      for( unsigned index = screen_id; index > 0; index-- )
      {
        xcb_screen_next( &screen_iter ) ;
      }
      
      this->screen = screen_iter.data ;
      this->window = xcb_generate_id( this->connection ) ;
      value_list[ 0 ] =  this->screen->black_pixel ;
      value_list[ 1 ] =  0                         ;
      
      xcb_create_window( 
        this->connection,
        XCB_COPY_FROM_PARENT,
        this->window, 
        this->screen->root,
        this->xpos,
        this->ypos,
        this->width, 
        this->height,
        border_size,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        this->screen->root_visual,
        event_mask,
        value_list 
                       ) ;
      
      xcb_change_property( this->connection, XCB_PROP_MODE_REPLACE, this->window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, this->title.size(), this->title.c_str() ) ;
      xcb_map_window( this->connection, this->window ) ;
      xcb_flush( this->connection ) ;
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
    
    void Window::handleEvents()
    {
      xcb_atom_t wmProtocols ;
      xcb_atom_t wmDeleteWin ;

      xcb_generic_event_t        *event          ;
      xcb_client_message_event_t *client_message ;
      
      event = xcb_wait_for_event( data().connection ) ;
      if( event )
      {
        switch( event->response_type & ~0x80 )
        {
          case XCB_CLIENT_MESSAGE:
            client_message = reinterpret_cast<xcb_client_message_event_t*>( event ) ;
            
            if( client_message->data.data32[ 0 ] == wmDeleteWin )
            {
              
            }
            break ;
        }
      }
    }
    xcb_connection_t* Window::connection() const
    {
      return data().connection ;
    }

    xcb_window_t Window::window() const
    {
      return data().window ;
    }
    WindowData& Window::data()
    {
      return *this->window_data ;
    }

    const WindowData& Window::data() const
    {
      return *this->window_data ;
    }
  }
}

