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
#include <event/Event.h>
#include <string>
#include <xcb/xcb.h>
#include <iostream>
#include <unordered_map>
namespace kgl
{
  namespace lx
  {
    static kgl::EventManager manager ;

    /** This method converts an XCB key to a library enum.
     * @note This is apparently O(1)? TODO: Look for better ways of doing this.
     * @param key The XCB key code to convert.
     * @return A Library Key enum.
     */
    kgl::Key keyFromXCB( unsigned char key )
    {
      switch( key )
      {
        case 0   : 
        case 1   : 
        case 2   : 
        case 3   : 
        case 4   : 
        case 5   : 
        case 6   : 
        case 7   : 
        case 8   : 
        case 9   : return kgl::Key::None       ;
        case 10  : return kgl::Key::One        ;
        case 11  : return kgl::Key::Two        ;
        case 12  : return kgl::Key::Three      ;
        case 13  : return kgl::Key::Four       ;
        case 14  : return kgl::Key::Five       ;
        case 15  : return kgl::Key::Six        ;
        case 16  : return kgl::Key::Seven      ;
        case 17  : return kgl::Key::Eight      ;
        case 18  : return kgl::Key::Nine       ;
        case 19  : return kgl::Key::Zero       ;
        case 20  : return kgl::Key::Hyphen     ;
        case 21  : return kgl::Key::Equals     ;
        case 22  :                         
        case 23  : return kgl::Key::None       ;
        case 24  : return kgl::Key::Q          ;
        case 25  : return kgl::Key::W          ;
        case 26  : return kgl::Key::E          ;
        case 27  : return kgl::Key::R          ;
        case 28  : return kgl::Key::T          ;
        case 29  : return kgl::Key::Y          ;
        case 30  : return kgl::Key::U          ;
        case 32  : return kgl::Key::I          ;
        case 33  : return kgl::Key::O          ;
        case 34  : return kgl::Key::LBracket   ;
        case 35  : return kgl::Key::RBracket   ;
        case 36  : return kgl::Key::Return     ;
        case 37  : return kgl::Key::LCtrl      ;
        case 38  : return kgl::Key::A          ;
        case 39  : return kgl::Key::S          ;
        case 40  : return kgl::Key::D          ;
        case 41  : return kgl::Key::F          ;
        case 42  : return kgl::Key::G          ;
        case 43  : return kgl::Key::H          ;
        case 44  : return kgl::Key::J          ;
        case 45  : return kgl::Key::K          ;
        case 46  : return kgl::Key::L          ;
        case 47  : return kgl::Key::Semicolon  ;
        case 48  : return kgl::Key::Apostraphe ;
        case 49  : return kgl::Key::Backtick   ;
        case 50  : return kgl::Key::LShift     ;
        case 51  : return kgl::Key::BSlash     ;
        case 52  : return kgl::Key::Z          ;
        case 53  : return kgl::Key::X          ;
        case 54  : return kgl::Key::C          ;
        case 55  : return kgl::Key::V          ;
        case 56  : return kgl::Key::B          ;
        case 57  : return kgl::Key::N          ;
        case 58  : return kgl::Key::M          ;
        case 59  : return kgl::Key::Comma      ;
        case 60  : return kgl::Key::Period     ;
        case 61  : return kgl::Key::FSlash     ;
        case 62  :  
        case 63  :  
        case 64  : return kgl::Key::None       ; 
        case 65  : return kgl::Key::Space      ; 
        case 66  : return kgl::Key::FSlash     ;
        
        case 111 : return kgl::Key::Up         ;
        case 113 : return kgl::Key::Left       ;
        case 114 : return kgl::Key::Right      ;
        case 116 : return kgl::Key::Down       ;
        default  : return kgl::Key::None       ;
      }
    }
    
    /** Method to retrieve a library mouse click from an XCB button press.
     * @param key The XCB button key press to convert.
     * @return A Library Key Enum.
     */
    kgl::MouseButton mouseButtonFromXCB( unsigned char key )
    {
      switch( key )
      {
        case 1  : return kgl::MouseButton::LeftClick   ;
        case 2  : return kgl::MouseButton::MiddleClick ;
        case 3  : return kgl::MouseButton::RightClick  ;
        case 8  : return kgl::MouseButton::Button01    ;
        case 9  : return kgl::MouseButton::Button02    ;
        default : return kgl::MouseButton::None        ;
      }
    }
    
    /** Method to handle a XCB mouse press.
     * @param press The XCB Press event to handle.
     */
    void handleMousePress( const xcb_button_press_event_t* press )
    {
      kgl::Event event = kgl::makeMouseButtonEvent( Event::Type::MouseButtonDown , mouseButtonFromXCB( press->detail ) ) ;
      manager.pushEvent( event ) ;
    }
    
    /** Method to handle a XCB mouse release.
     * @param press The XCB Releasee event to handle.
     */
    void handleMouseRelease( const xcb_button_release_event_t* release )
    {
      kgl::Event event = kgl::makeMouseButtonEvent( Event::Type::MouseButtonUp , mouseButtonFromXCB( release->detail ) ) ;
      manager.pushEvent( event ) ;
    }

    /** Method to handle a XCB key press.
     * @param press The XCB Key Press event to handle.
     */
    void handleKeyPress( const xcb_key_press_event_t* press )
    {
      kgl::Event event = kgl::makeKeyEvent( Event::Type::KeyDown, keyFromXCB( press->detail ) ) ;
//      std::cout << +press->detail << std::endl ;
      manager.pushEvent( event ) ;
    }

    /** Method to handle a XCB key release.
     * @param press The XCB Key Release event to handle.
     */
    void handleKeyRelease( const xcb_key_release_event_t* press )
    {
      kgl::Event event = kgl::makeKeyEvent( Event::Type::KeyUp, keyFromXCB( press->detail )  ) ;
//      std::cout << +press->detail << std::endl ;
      manager.pushEvent( event ) ;
    }
    
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
      bool              has_mouse   ;

      /** Default constructor.
       */
      WindowData() ;
      
      /** Method to create the X11 window.
       */
      void create() ;
    };
    
    WindowData::WindowData()
    {
      this->borderless = false   ;
      this->connection = nullptr ;
      this->screen     = nullptr ;
    }
    
    void WindowData::create()
    {
      const unsigned event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK ;
      
      int                   screen_id       ;
      xcb_screen_iterator_t screen_iter     ;
      unsigned              value_list[ 2 ] ;
      unsigned              border_size     ;

      screen_id   = 0                        ;
      border_size = this->borderless ? 0 : 2 ;
      
      this->connection = xcb_connect( nullptr, &screen_id ) ;
      
      if( xcb_connection_has_error( this->connection ) )
      {
        std::cout << "FUCK ERROR" << std::endl ;
      }
      
      screen_iter = xcb_setup_roots_iterator( xcb_get_setup( this->connection ) ) ;
      for( unsigned index = screen_id; index > 0; index-- )
      {
        xcb_screen_next( &screen_iter ) ;
      }
      
      this->screen = screen_iter.data ;
      this->window = xcb_generate_id( this->connection ) ;
      value_list[ 0 ] =  this->screen->black_pixel ;
      value_list[ 1 ] = XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
                        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                        XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
                        XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE ;
      
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
      xcb_atom_t wmDeleteWin ;

      xcb_generic_event_t        *event          ;
      xcb_client_message_event_t *client_message ;

      while( ( event = xcb_poll_for_event( data().connection ) ) )
      {
        switch( event->response_type & ~0x80 )
        {
          case XCB_CLIENT_MESSAGE:
            client_message = reinterpret_cast<xcb_client_message_event_t*>( event ) ;
            
            if( client_message->data.data32[ 0 ] == wmDeleteWin )
            {
              
            }
            break ;
          case XCB_BUTTON_PRESS:
            handleMousePress( reinterpret_cast<xcb_button_press_event_t*>( event ) ) ;
            
            break ;
            
          case XCB_BUTTON_RELEASE:
          {
            handleMouseRelease( reinterpret_cast<xcb_button_release_event_t*>( event ) ) ;
            
            break ;
          }
          case XCB_ENTER_NOTIFY : // Mouse enters window.
          {
            data().has_mouse = true ;
            break ;
          }
          case XCB_LEAVE_NOTIFY: // Mouse leaves window.
          {
            data().has_mouse = false ;
            break ;
          }
          case XCB_MOTION_NOTIFY:
          {
//            auto motion = reinterpret_cast<xcb_enter_notify_event_t*>( event ) ;
            std::cout << "Motion Notify" << std::endl ;
            break ;
          }
          case XCB_KEY_PRESS:
          {
            handleKeyPress( reinterpret_cast<xcb_key_press_event_t*>( event ) ) ;
            break ;
          }
          case XCB_KEY_RELEASE:
          {
            handleKeyRelease( reinterpret_cast<xcb_key_release_event_t*>( event ) ) ;
            break ;
          }
        }
        free( event ) ;
      }
    }
    
    void Window::reset()
    {
      if( data().connection )
      {
        xcb_disconnect( data().connection ) ;
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

