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
#include <string>
#include <SDL2/SDL.h>

bool SDL2_INITIALIZED = false ;

namespace nyx
{
  namespace sdl
  {
    static nyx::EventManager manager ;

    /** This method converts an XCB key to a library enum.
     * @note This is apparently O(1)? TODO: Look for better ways of doing this.
     * @param key The XCB key code to convert.
     * @return A Library Key enum.
     */
    nyx::Key keyFromSDL( Sint32 key )
    {
      switch( key )
      {
        case SDLK_ESCAPE      : return nyx::Key::ESC        ;
        case SDLK_1           : return nyx::Key::One        ;
        case SDLK_2           : return nyx::Key::Two        ;
        case SDLK_3           : return nyx::Key::Three      ;
        case SDLK_4           : return nyx::Key::Four       ;
        case SDLK_5           : return nyx::Key::Five       ;
        case SDLK_6           : return nyx::Key::Six        ;
        case SDLK_7           : return nyx::Key::Seven      ;
        case SDLK_8           : return nyx::Key::Eight      ;
        case SDLK_9           : return nyx::Key::Nine       ;
        case SDLK_0           : return nyx::Key::Zero       ;
        case SDLK_EQUALS      : return nyx::Key::Equals     ;
        case SDLK_q           : return nyx::Key::Q          ;
        case SDLK_w           : return nyx::Key::W          ;
        case SDLK_e           : return nyx::Key::E          ;
        case SDLK_r           : return nyx::Key::R          ;
        case SDLK_t           : return nyx::Key::T          ;
        case SDLK_y           : return nyx::Key::Y          ;
        case SDLK_u           : return nyx::Key::U          ;
        case SDLK_i           : return nyx::Key::I          ;
        case SDLK_o           : return nyx::Key::O          ;
        case SDLK_LEFTBRACKET : return nyx::Key::LBracket   ;
        case SDLK_RIGHTBRACKET: return nyx::Key::RBracket   ;
        case SDLK_RETURN      : return nyx::Key::Return     ;
        case SDLK_LCTRL       : return nyx::Key::LCtrl      ;
        case SDLK_a           : return nyx::Key::A          ;
        case SDLK_s           : return nyx::Key::S          ;
        case SDLK_d           : return nyx::Key::D          ;
        case SDLK_f           : return nyx::Key::F          ;
        case SDLK_g           : return nyx::Key::G          ;
        case SDLK_h           : return nyx::Key::H          ;
        case SDLK_j           : return nyx::Key::J          ;
        case SDLK_k           : return nyx::Key::K          ;
        case SDLK_l           : return nyx::Key::L          ;
        case SDLK_SEMICOLON   : return nyx::Key::Semicolon  ;
        case SDLK_BACKQUOTE   : return nyx::Key::Backtick   ;
        case SDLK_LSHIFT      : return nyx::Key::LShift     ;
        case SDLK_BACKSLASH   : return nyx::Key::BSlash     ;
        case SDLK_z           : return nyx::Key::Z          ;
        case SDLK_x           : return nyx::Key::X          ;
        case SDLK_c           : return nyx::Key::C          ;
        case SDLK_v           : return nyx::Key::V          ;
        case SDLK_b           : return nyx::Key::B          ;
        case SDLK_n           : return nyx::Key::N          ;
        case SDLK_m           : return nyx::Key::M          ;
        case SDLK_COMMA       : return nyx::Key::Comma      ;
        case SDLK_PERIOD      : return nyx::Key::Period     ;
        case SDLK_SLASH       : return nyx::Key::FSlash     ;
        case SDLK_SPACE       : return nyx::Key::Space      ; 
        
        case SDLK_UP    : return nyx::Key::Up         ;
        case SDLK_LEFT  : return nyx::Key::Left       ;
        case SDLK_RIGHT : return nyx::Key::Right      ;
        case SDLK_DOWN  : return nyx::Key::Down       ;
        
        default  : return nyx::Key::None       ;
      }
    }
    
    /** Method to retrieve a library mouse click from an XCB button press.
     * @param key The XCB button key press to convert.
     * @return A Library Key Enum.
     */
    nyx::MouseButton mouseButtonFromSDL( Uint8 key )
    {
      switch( key )
      {
        case SDL_BUTTON_LEFT   : return nyx::MouseButton::LeftClick   ;
        case SDL_BUTTON_RIGHT  : return nyx::MouseButton::MiddleClick ;
        case SDL_BUTTON_MIDDLE : return nyx::MouseButton::RightClick  ;
        case SDL_BUTTON_X1     : return nyx::MouseButton::Button01    ;
        case SDL_BUTTON_X2     : return nyx::MouseButton::Button02    ;
        default : return nyx::MouseButton::None                       ;
      }
    }
    
    void convertEvent( SDL_Event& event )
    {
      nyx::Event nyx_event ;
      
      if( event.type == SDL_KEYDOWN )
      {
        nyx_event = nyx::makeKeyEvent( nyx::Event::Type::KeyDown, keyFromSDL( event.key.keysym.sym ) ) ;
        manager.pushEvent( nyx_event ) ;
      }
      else if( event.type == SDL_KEYUP )
      {
        nyx_event = nyx::makeKeyEvent( nyx::Event::Type::KeyUp, keyFromSDL( event.key.keysym.sym ) ) ;
        manager.pushEvent( nyx_event ) ;
      }

      else if( event.type == SDL_MOUSEBUTTONDOWN )
      {
        nyx_event = nyx::makeMouseButtonEvent( nyx::Event::Type::MouseButtonDown, mouseButtonFromSDL( event.button.button ) ) ;
        manager.pushEvent( nyx_event ) ;
      }
      else if( event.type == SDL_MOUSEBUTTONUP )
      {
        nyx_event = nyx::makeMouseButtonEvent( nyx::Event::Type::MouseButtonUp, mouseButtonFromSDL( event.button.button ) ) ;
        manager.pushEvent( nyx_event ) ;
      }
      else if( event.type == SDL_QUIT )
      {
        nyx_event = nyx::makeKeyEvent( Event::Type::WindowExit, nyx::Key::ESC ) ;
        manager.pushEvent( nyx_event ) ;
      }
    }

    struct WindowData
    {
      SDL_Window*       window      ;
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
      this->resizable = false ;
      this->fullscreen = false ;
      this->width = 1280 ;
      this->height = 1024 ;
      this->xpos = 0 ;
      this->ypos = 0 ;
      this->title = "" ;
      this->minimized = false ;
    }
    
    void WindowData::setWindowBorderless( bool value )
    {
      this->borderless = value ;
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
      this->title = value ;
    }

    void WindowData::setWindowWidth( unsigned value )
    {
      this->width = value ;
    }

    void WindowData::setWindowHeight( unsigned value )
    {
      this->height = value ;
    }

    void WindowData::create()
    {
      if( !SDL2_INITIALIZED )
      {
        SDL_Init( SDL_INIT_EVERYTHING ) ;
        SDL2_INITIALIZED = true ;
      }

      int flags = SDL_WINDOW_VULKAN ;
      
      if     ( this->resizable                      ) flags |= SDL_WINDOW_RESIZABLE          ;
      if     ( this->maximized                      ) flags |= SDL_WINDOW_MAXIMIZED          ;
      if     ( this->borderless && this->fullscreen ) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP ;
      else if( this->fullscreen                     ) flags |= SDL_WINDOW_FULLSCREEN         ;
      else if( this->borderless                     ) flags |= SDL_WINDOW_BORDERLESS         ;
      
      this->window = SDL_CreateWindow( this->title.c_str(), this->xpos, this->ypos, this->width, this->height, flags ) ;
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
      return ( data().window != nullptr ) ;
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
      data().xpos = position ;
    }
    
    void Window::setYPosition( unsigned position )
    {
      data().ypos = position ;
    }
    
    void Window::setWidth( unsigned width )
    {
      data().setWindowWidth( width ) ;
    }
    
    void Window::setHeight( unsigned height )
    { 
      data().setWindowHeight( height ) ;
    }

    void Window::setMonitor( unsigned monitor_id )
    {
      data().monitor = monitor_id ;
    }
    
    void Window::setTitle( const char* value )
    {
      data().setWindowTitle( value ) ;
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
      
      data().setWindowBorderless( value ) ;
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
      bool      mouse_move ;
      SDL_Event event      ;
      int       x, y       ;
      
      SDL_GetMouseState( &x, &y ) ;
      
      mouse_move = false ;
      while( SDL_PollEvent( &event ) != 0 )
      {
        if( event.type == SDL_MOUSEMOTION )
        {
          manager.updateMouseOffset( event.motion.xrel, event.motion.yrel ) ;
          manager.updateMouse      ( event.motion.x   , event.motion.y    ) ;
          mouse_move = true ;
        }
        convertEvent( event ) ;
      }
      
      if( !mouse_move )
      {
        manager.updateMouseOffset( manager.mouseX() - x, manager.mouseY() - y ) ;
        manager.updateMouse      ( x                   , y                    ) ;
      }
    }
    
    void Window::reset()
    {
    }
    
    unsigned Window::width() const
    {
      return data().width ;
    }
    
    unsigned Window::height() const
    {
      return data().height ;
    }

    SDL_Window* Window::window() const
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

