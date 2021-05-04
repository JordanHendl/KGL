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
#include <stdio.h>
#include <cstring>
#include <string>
#include <xcb/xcb.h>
#include <iostream>
#include <unordered_map>
#include <xcb/xproto.h>
#include <cmath>

namespace nyx
{
  namespace lx
  {
    static nyx::EventManager manager ;

    /** This method converts an XCB key to a library enum.
     * @note This is apparently O(1)? TODO: Look for better ways of doing this.
     * @param key The XCB key code to convert.
     * @return A Library Key enum.
     */
    nyx::Key keyFromXCB( unsigned char key )
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
        case 9   : return nyx::Key::ESC        ;
        case 10  : return nyx::Key::One        ;
        case 11  : return nyx::Key::Two        ;
        case 12  : return nyx::Key::Three      ;
        case 13  : return nyx::Key::Four       ;
        case 14  : return nyx::Key::Five       ;
        case 15  : return nyx::Key::Six        ;
        case 16  : return nyx::Key::Seven      ;
        case 17  : return nyx::Key::Eight      ;
        case 18  : return nyx::Key::Nine       ;
        case 19  : return nyx::Key::Zero       ;
        case 20  : return nyx::Key::Hyphen     ;
        case 21  : return nyx::Key::Equals     ;
        case 22  :                         
        case 23  : return nyx::Key::None       ;
        case 24  : return nyx::Key::Q          ;
        case 25  : return nyx::Key::W          ;
        case 26  : return nyx::Key::E          ;
        case 27  : return nyx::Key::R          ;
        case 28  : return nyx::Key::T          ;
        case 29  : return nyx::Key::Y          ;
        case 30  : return nyx::Key::U          ;
        case 32  : return nyx::Key::I          ;
        case 33  : return nyx::Key::O          ;
        case 34  : return nyx::Key::LBracket   ;
        case 35  : return nyx::Key::RBracket   ;
        case 36  : return nyx::Key::Return     ;
        case 37  : return nyx::Key::LCtrl      ;
        case 38  : return nyx::Key::A          ;
        case 39  : return nyx::Key::S          ;
        case 40  : return nyx::Key::D          ;
        case 41  : return nyx::Key::F          ;
        case 42  : return nyx::Key::G          ;
        case 43  : return nyx::Key::H          ;
        case 44  : return nyx::Key::J          ;
        case 45  : return nyx::Key::K          ;
        case 46  : return nyx::Key::L          ;
        case 47  : return nyx::Key::Semicolon  ;
        case 48  : return nyx::Key::Apostraphe ;
        case 49  : return nyx::Key::Backtick   ;
        case 50  : return nyx::Key::LShift     ;
        case 51  : return nyx::Key::BSlash     ;
        case 52  : return nyx::Key::Z          ;
        case 53  : return nyx::Key::X          ;
        case 54  : return nyx::Key::C          ;
        case 55  : return nyx::Key::V          ;
        case 56  : return nyx::Key::B          ;
        case 57  : return nyx::Key::N          ;
        case 58  : return nyx::Key::M          ;
        case 59  : return nyx::Key::Comma      ;
        case 60  : return nyx::Key::Period     ;
        case 61  : return nyx::Key::FSlash     ;
        case 62  :  
        case 63  :  
        case 64  : return nyx::Key::None       ; 
        case 65  : return nyx::Key::Space      ; 
        case 66  : return nyx::Key::FSlash     ;
        
        case 111 : return nyx::Key::Up         ;
        case 113 : return nyx::Key::Left       ;
        case 114 : return nyx::Key::Right      ;
        case 116 : return nyx::Key::Down       ;
        default  : return nyx::Key::None       ;
      }
    }
    
    /** Method to retrieve a library mouse click from an XCB button press.
     * @param key The XCB button key press to convert.
     * @return A Library Key Enum.
     */
    nyx::MouseButton mouseButtonFromXCB( unsigned char key )
    {
      switch( key )
      {
        case 1  : return nyx::MouseButton::LeftClick   ;
        case 2  : return nyx::MouseButton::MiddleClick ;
        case 3  : return nyx::MouseButton::RightClick  ;
        case 8  : return nyx::MouseButton::Button01    ;
        case 9  : return nyx::MouseButton::Button02    ;
        default : return nyx::MouseButton::None        ;
      }
    }
    
    /** Method to handle a XCB mouse press.
     * @param press The XCB Press event to handle.
     */
    void handleMousePress( const xcb_button_press_event_t* press )
    {
      nyx::Event event = nyx::makeMouseButtonEvent( Event::Type::MouseButtonDown , mouseButtonFromXCB( press->detail ) ) ;
      manager.pushEvent( event ) ;
    }
    
    /** Method to handle a XCB mouse release.
     * @param press The XCB Releasee event to handle.
     */
    void handleMouseRelease( const xcb_button_release_event_t* release )
    {
      nyx::Event event = nyx::makeMouseButtonEvent( Event::Type::MouseButtonUp , mouseButtonFromXCB( release->detail ) ) ;
      manager.pushEvent( event ) ;
    }

    /** Method to handle a XCB key press.
     * @param press The XCB Key Press event to handle.
     */
    void handleKeyPress( const xcb_key_press_event_t* press )
    {
      nyx::Event event = nyx::makeKeyEvent( Event::Type::KeyDown, keyFromXCB( press->detail ) ) ;
      manager.pushEvent( event ) ;
    }
    
    void handleMouseMotion( const xcb_enter_notify_event_t* motion )
    {
      manager.updateMouseOffset( manager.mouseX() - static_cast<float>( motion->event_x ), static_cast<float>( motion->event_y ) - manager.mouseY() ) ;
      manager.updateMouse( motion->event_x, motion->event_y ) ;
    }

    /** Method to handle a XCB key release.
     * @param press The XCB Key Release event to handle.
     */
    void handleKeyRelease( const xcb_key_release_event_t* press )
    {
      nyx::Event event = nyx::makeKeyEvent( Event::Type::KeyUp, keyFromXCB( press->detail )  ) ;
      manager.pushEvent( event ) ;
    }
    
    struct WindowData
    {
      xcb_connection_t *connection  ; ///< TODO
      xcb_screen_t     *screen      ; ///< TODO
      xcb_window_t      window      ; ///< TODO
      std::string       title       ; ///< TODO
      unsigned          xpos        ; ///< TODO
      unsigned          ypos        ; ///< TODO
      unsigned          width       ; ///< TODO
      unsigned          height      ; ///< TODO
      unsigned          depth       ; ///< TODO
      unsigned          monitor     ; ///< TODO
      bool              fullscreen  ; ///< TODO
      bool              borderless  ; ///< TODO
      bool              minimized   ; ///< TODO
      bool              maximized   ; ///< TODO
      bool              resizable   ; ///< TODO
      bool              has_mouse   ; ///< TODO

      /** Default constructor.
       */
      WindowData() ;
      
      void setWindowBorderless( bool value ) ;
      void setWindowMaximized( bool value ) ;
      void setWindowMinimized( bool value ) ;
      void setWindowTitle( const char* value ) ;
      void setWindowWidth( unsigned value ) ;
      void setWindowHeight( unsigned value ) ;
      
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
    
    void WindowData::setWindowBorderless( bool value )
    {
      xcb_intern_atom_cookie_t cookie3 = xcb_intern_atom(connection, 0, strlen ( "_MOTIF_WM_HINTS" ), "_MOTIF_WM_HINTS" ) ;
      xcb_intern_atom_reply_t *reply3  = xcb_intern_atom_reply ( connection, cookie3, NULL ) ;
      
      // motif hints
      struct MotifHints
      {
          uint32_t   flags;
          uint32_t   functions;
          uint32_t   decorations;
          int32_t    input_mode;
          uint32_t   status;
      };
      
      MotifHints hints;
      
      hints.flags       = 2             ;
      hints.functions   = 0             ;
      hints.decorations = value ? 0 : 3 ;
      hints.input_mode  = 0             ;
      hints.status      = 0             ;
      
      xcb_change_property ( this->connection,
                            XCB_PROP_MODE_REPLACE,
                            this->window,
                            reply3->atom,
                            reply3->atom,
                            32,  
                            5,   
                            &hints ) ;
      
      free( reply3 ) ;
      xcb_flush( this->connection ) ;
    }

    void WindowData::setWindowMaximized( bool value )
    {
      value = value ;
    }

    void WindowData::setWindowMinimized( bool value )
    {
      value = value ;
    }

    void WindowData::setWindowTitle( const char* value )
    {
      char*       class_hint  ;
      size_t      class_len   ;
      std::string str = value ;
      
      class_len = strlen( str.c_str() ) + 1 + strlen( str.c_str() ) + 1 ;
      class_hint = new char[ class_len ] ;
      strcpy( class_hint, str.c_str() ) ;
      strcpy( class_hint + strlen( str.c_str() ) + 1, str.c_str() ) ;
      
      xcb_change_property( this->connection, XCB_PROP_MODE_REPLACE,
                           this->window,
                           XCB_ATOM_WM_NAME,
                           XCB_ATOM_STRING,
                           8,
                           str.size(),
                           str.c_str() ) ;

      xcb_change_property( this->connection, XCB_PROP_MODE_REPLACE,
                           this->window,
                           XCB_ATOM_WM_CLASS,
                           XCB_ATOM_STRING,
                           8,
                           class_len,
                           class_hint ) ;
     
      xcb_flush( this->connection ) ;
      
      delete[] class_hint ;
    }

    void WindowData::setWindowWidth( unsigned value )
    {
      struct WMSizeHints
      {
        uint32_t flags;
        int32_t  x, y;
        int32_t  width, height;
        int32_t  min_width, min_height;
        int32_t  max_width, max_height;
        int32_t  width_inc, height_inc;
        int32_t  min_aspect_num, min_aspect_den;
        int32_t  max_aspect_num, max_aspect_den;
        int32_t  base_width, base_height;
        uint32_t win_gravity;
      } ;
      
      enum WMSizeHintsFlag
      {
        WM_SIZE_HINT_US_POSITION   = 1U << 0,
        WM_SIZE_HINT_US_SIZE       = 1U << 1,
        WM_SIZE_HINT_P_POSITION    = 1U << 2,
        WM_SIZE_HINT_P_SIZE        = 1U << 3,
        WM_SIZE_HINT_P_MIN_SIZE    = 1U << 4,
        WM_SIZE_HINT_P_MAX_SIZE    = 1U << 5,
        WM_SIZE_HINT_P_RESIZE_INC  = 1U << 6,
        WM_SIZE_HINT_P_ASPECT      = 1U << 7,
        WM_SIZE_HINT_BASE_SIZE     = 1U << 8,
        WM_SIZE_HINT_P_WIN_GRAVITY = 1U << 9
      };
      
      struct WMSizeHints hints ;
      hints.flags = WM_SIZE_HINT_P_WIN_GRAVITY ;
      hints.win_gravity = XCB_GRAVITY_STATIC ;
      

      hints.flags |= WM_SIZE_HINT_P_SIZE;
      hints.x = value ;
      hints.y = this->height ;
      
      xcb_change_property(this->connection, XCB_PROP_MODE_REPLACE, this->window,
        XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS,
          32, sizeof(struct WMSizeHints) >> 2, &hints);
    }

    void WindowData::setWindowHeight( unsigned value )
    {
      struct WMSizeHints
      {
        uint32_t flags;
        int32_t  x, y;
        int32_t  width, height;
        int32_t  min_width, min_height;
        int32_t  max_width, max_height;
        int32_t  width_inc, height_inc;
        int32_t  min_aspect_num, min_aspect_den;
        int32_t  max_aspect_num, max_aspect_den;
        int32_t  base_width, base_height;
        uint32_t win_gravity;
      } ;
      
      enum WMSizeHintsFlag
      {
        WM_SIZE_HINT_US_POSITION   = 1U << 0,
        WM_SIZE_HINT_US_SIZE       = 1U << 1,
        WM_SIZE_HINT_P_POSITION    = 1U << 2,
        WM_SIZE_HINT_P_SIZE        = 1U << 3,
        WM_SIZE_HINT_P_MIN_SIZE    = 1U << 4,
        WM_SIZE_HINT_P_MAX_SIZE    = 1U << 5,
        WM_SIZE_HINT_P_RESIZE_INC  = 1U << 6,
        WM_SIZE_HINT_P_ASPECT      = 1U << 7,
        WM_SIZE_HINT_BASE_SIZE     = 1U << 8,
        WM_SIZE_HINT_P_WIN_GRAVITY = 1U << 9
      };
      
      struct WMSizeHints hints ;
      hints.flags = WM_SIZE_HINT_P_WIN_GRAVITY ;
      hints.win_gravity = XCB_GRAVITY_STATIC ;
      hints.flags |= WM_SIZE_HINT_P_SIZE;
      hints.x = this->width ;
      hints.y = value ;
      
      xcb_change_property(this->connection, XCB_PROP_MODE_REPLACE, this->window,
        XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS,
          32, sizeof(struct WMSizeHints) >> 2, &hints);
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
        std::cout << "Error" << std::endl ;
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
      
      this->setWindowTitle( this->title.c_str() ) ;
    }

    Window::Window()
    {
      this->window_data = new WindowData() ;
    }
    
    Window::~Window()
    {
      delete this->window_data ;
    }
    
    bool Window::initialized() const
    {
      return ( data().connection != nullptr && data().screen != nullptr ) ;
    }
    
    void Window::initialize( const char* window_title, unsigned width, unsigned height )
    {
      data().title  = window_title ;
      data().width  = width        ;
      data().height = height       ;
      
      data().create() ;
    }
    
    void Window::setXPosition( unsigned position )
    {
      static uint32_t values[ 2 ] ;
      values[ 0 ] = position ;

      if( this->initialized() )
      {
        xcb_configure_window( data().connection, data().window, XCB_CONFIG_WINDOW_X, static_cast<const void*>( values ) ) ;
      }
    }
    
    void Window::setYPosition( unsigned position )
    {
      if( this->initialized() )
      {
        xcb_configure_window( data().connection, data().window, XCB_CONFIG_WINDOW_Y, static_cast<const void*>( &position ) ) ;
      }
    }
    
    void Window::setWidth( unsigned width )
    {
      if( this->initialized() )
      {
        xcb_configure_window( data().connection, data().window, XCB_CONFIG_WINDOW_WIDTH, static_cast<const void*>( &width ) ) ;
      }
    }
    
    void Window::setHeight( unsigned height )
    { 
      if( this->initialized() )
      {
        xcb_configure_window( data().connection, data().window, XCB_CONFIG_WINDOW_HEIGHT, static_cast<const void*>( &height ) ) ;
      }
    }

    void Window::setMonitor( unsigned monitor_id )
    {
      data().monitor = monitor_id ;
    }
    
    void Window::setTitle( const char* value )
    {
      data().title = value ;
      if( data().connection )
      {
        xcb_change_property( data().connection, XCB_PROP_MODE_REPLACE, data().window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, data().title.size(), data().title.c_str() ) ;
      }
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
      
      if( data().connection )
      {
        data().setWindowBorderless( value ) ;
      }
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
      bool moved_mouse = false ;
      
      // Prepare notification for window destruction
      xcb_intern_atom_cookie_t  protocols_cookie = xcb_intern_atom( data().connection, 1, 12, "WM_PROTOCOLS" ) ;
      xcb_intern_atom_reply_t  *protocols_reply  = xcb_intern_atom_reply( data().connection, protocols_cookie, 0 ) ;
      xcb_intern_atom_cookie_t  delete_cookie    = xcb_intern_atom( data().connection, 0, 16, "WM_DELETE_WINDOW" ) ;
      xcb_intern_atom_reply_t  *delete_reply     = xcb_intern_atom_reply( data().connection, delete_cookie, 0 ) ;
      xcb_change_property( data().connection, XCB_PROP_MODE_REPLACE, data().window, (*protocols_reply).atom, 4, 32, 1, &(*delete_reply).atom ) ;

      xcb_generic_event_t        *event          ;
      xcb_client_message_event_t *client_message ;

      free( protocols_reply ) ;
      while( ( event = xcb_poll_for_event( data().connection ) ) )
      {
        switch( event->response_type & ~0x80 )
        {
          // Resized
          case XCB_CONFIGURE_NOTIFY :
          {
            xcb_configure_notify_event_t *configure_event = reinterpret_cast<xcb_configure_notify_event_t*>( event ) ;
            
            data().width  = configure_event->width  ;
            data().height = configure_event->height ;
            break ;
          }
          // Message 
          case XCB_CLIENT_MESSAGE :
          {
            client_message = reinterpret_cast<xcb_client_message_event_t*>( event ) ;
            
            if( client_message->data.data32[ 0 ] == ( *delete_reply ).atom )
            {
              nyx::Event event = nyx::makeKeyEvent( Event::Type::WindowExit, nyx::Key::A ) ;
              manager.pushEvent( event ) ;
              
              free( delete_reply ) ;
            }
            break ;
          }
          case XCB_BUTTON_PRESS :
          {
            handleMousePress( reinterpret_cast<xcb_button_press_event_t*>( event ) ) ;
            break ;
          } 
          case XCB_BUTTON_RELEASE :
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
          // Mouse is moving on the window.
          case XCB_MOTION_NOTIFY:
          {
            unsigned mid_x ;
            unsigned mid_y ;
            auto tmp = reinterpret_cast<xcb_enter_notify_event_t*>( event ) ;
            
            mid_x = data().width  / 2 ;
            mid_y = data().height / 2 ;
            
            moved_mouse = true ;
            if( manager.mouseX() < static_cast<float>( mid_x - 40 ) || manager.mouseY() < static_cast<float>( mid_y - 20 ) ||
                manager.mouseX() > static_cast<float>( mid_x + 40 ) || manager.mouseY() > static_cast<float>( mid_y + 20 ) )
            {
              manager.updateMouseOffset( manager.mouseX() - static_cast<float>( tmp->event_x ), static_cast<float>( tmp->event_y ) - manager.mouseY() ) ;
              manager.updateMouse      ( static_cast<float>( mid_x ), static_cast<float>( mid_y ) ) ;
              xcb_warp_pointer( data().connection, XCB_NONE, data().window, 0, 0, 0, 0, mid_x, mid_y ) ;
            }
            else
            {
              handleMouseMotion( tmp ) ;
            }
            
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
      if( !moved_mouse ) manager.updateMouseOffset( 0.f , 0.f ) ;
    }
    
    void Window::reset()
    {
      if( data().connection )
      {
        xcb_disconnect( data().connection ) ;
      }
    }
    
    unsigned Window::width() const
    {
      return data().width ;
    }
    
    unsigned Window::height() const
    {
      return data().height ;
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

