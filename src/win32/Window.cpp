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
 * Author: Jordan
 * 
 * Created on January 9, 2021, 6:04 AM
 */

#include "Window.h"
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <iostream>
#include <event/Event.h>
#include <thread>

static HMODULE win_module ;

/** Standard windows entry.
 * @param hModule the program module.
 * @param ul_reason_for_call No clue
 * @param lpReserved No clue.
 */
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
  win_module = hModule;
  return TRUE ;
}

namespace kgl
{
  namespace win32
  {
    /** Function to process the input window.
     * @param handle The window handle
     * @param message The message event.
     * @param wParam the wparam to process.
     * @param lParam the lparam to process.
     */
    LRESULT CALLBACK processWindow( HWND handle, UINT message, WPARAM wParam, LPARAM lParam ) ;
    
    /** Function to convert a win32 key param to a library key.
     * @param param The Window Wparam to convert.
     */
    static kgl::Key keyFromWin32Key( WPARAM param ) ;
    
    /** Structure to contain a WIN32 Window's data.
     */
    struct WindowData
    {
      HWND        handle       ; ///< TEST
      HINSTANCE   instance     ; ///< TEST
      WNDCLASSEX  window_class ; ///< TEST
      std::string title        ; ///< TEST
      unsigned    width        ; ///< TEST
      unsigned    height       ; ///< TEST
      
      /** Default constructor.
       */
      WindowData() ;
      
      /** Method to create a win32 window.
       */
      void createWindow() ;
    };

     kgl::Key keyFromWin32Key( WPARAM param )
    {
      switch (param)
      {
        case 8  : return kgl::Key::Backspace  ;
        case 9  : return kgl::Key::LTab       ;
        case 13 : return kgl::Key::Return     ;
        case 16 : return kgl::Key::LShift     ;
        case 17 : return kgl::Key::LCtrl      ;
        case 32 : return kgl::Key::Space      ;
        case 37 : return kgl::Key::Left       ;
        case 38 : return kgl::Key::Up         ;
        case 39 : return kgl::Key::Right      ;
        case 40 : return kgl::Key::Down       ;
        case 48 : return kgl::Key::Zero       ;
        case 49 : return kgl::Key::One        ;
        case 50 : return kgl::Key::Two        ;
        case 51 : return kgl::Key::Three      ;
        case 52 : return kgl::Key::Four       ;
        case 53 : return kgl::Key::Five       ;
        case 54 : return kgl::Key::Six        ;
        case 55 : return kgl::Key::Seven      ;
        case 56 : return kgl::Key::Eight      ;
        case 57 : return kgl::Key::Nine       ;
        case 58 : 
        case 59 : 
        case 60 : return kgl::Key::None       ;
        case 65 : return kgl::Key::A          ;
        case 66 : return kgl::Key::B          ;
        case 67 : return kgl::Key::C          ;
        case 68 : return kgl::Key::D          ;
        case 69 : return kgl::Key::E          ;
        case 70 : return kgl::Key::F          ;
        case 71 : return kgl::Key::G          ;
        case 72 : return kgl::Key::H          ;
        case 73 : return kgl::Key::I          ;
        case 74 : return kgl::Key::J          ;
        case 75 : return kgl::Key::K          ;
        case 76 : return kgl::Key::L          ;
        case 77 : return kgl::Key::M          ;
        case 78 : return kgl::Key::N          ;
        case 79 : return kgl::Key::O          ;
        case 80 : return kgl::Key::P          ;
        case 81 : return kgl::Key::Q          ;
        case 82 : return kgl::Key::R          ;
        case 83 : return kgl::Key::S          ;
        case 84 : return kgl::Key::T          ;
        case 85 : return kgl::Key::U          ;
        case 86 : return kgl::Key::V          ;
        case 87 : return kgl::Key::W          ;
        case 88 : return kgl::Key::X          ;
        case 89 : return kgl::Key::Y          ;
        case 90 : return kgl::Key::Z          ;
        case 112: return kgl::Key::F1         ;
        case 113: return kgl::Key::F2         ;
        case 114: return kgl::Key::F3         ;
        case 115: return kgl::Key::F4         ;
        case 116: return kgl::Key::F5         ;
        case 117: return kgl::Key::F6         ;
        case 118: return kgl::Key::F7         ;
        case 119: return kgl::Key::F8         ;
        case 120: return kgl::Key::F9         ;
        case 121: return kgl::Key::F10        ;
        case 122: return kgl::Key::F11        ;
        case 123: return kgl::Key::F12        ;
                    
        case 182: return kgl::Key::Hyphen     ;
        case 186: return kgl::Key::Semicolon  ;
        case 187: return kgl::Key::Equals     ;
        case 188: return kgl::Key::Comma      ;
        case 190: return kgl::Key::Period     ;
        case 191: return kgl::Key::FSlash     ;
        case 192: return kgl::Key::Backtick   ;
        case 219: return kgl::Key::LBracket   ;
        case 220: return kgl::Key::BSlash     ;
        case 221: return kgl::Key::RBracket   ;
        case 222: return kgl::Key::Apostraphe ;
        default : return kgl::Key::None ;
      } ;
    }
       
    LRESULT CALLBACK processWindow( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
    {
      static kgl::EventManager manager ;
      static kgl::Event        event   ;
      static int               xmouse  ;
      static int               ymouse  ;
      static unsigned          button  ;

      switch (message)
      {
         case WM_SIZE : 
           unsigned width  ;
           unsigned height ;

           width  = LOWORD( lParam ) ;
           height = HIWORD( lParam ) ;
           break ;
         case WM_MOUSEMOVE :
           xmouse = GET_X_LPARAM( lParam ) ;
           ymouse = GET_Y_LPARAM( lParam ) ;
           break ;
         case WM_KEYDOWN :
           event = kgl::makeKeyEvent( kgl::Event::Type::KeyDown, keyFromWin32Key( wParam ) ) ;
           manager.pushEvent( event ) ;
           break ;
         case WM_KEYUP   :
           event = kgl::makeKeyEvent( kgl::Event::Type::KeyUp, keyFromWin32Key( wParam ) ) ;
           manager.pushEvent( event ) ;

           break ;
         case WM_MBUTTONDOWN :
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyDown, kgl::MouseButton::MiddleClick ) ;
           manager.pushEvent( event ) ;
           break ;
         case WM_MBUTTONUP :
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyUp, kgl::MouseButton::MiddleClick ) ;
           manager.pushEvent( event ) ;
           break ;                                                   
         case WM_LBUTTONDOWN :
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyDown, kgl::MouseButton::LeftClick ) ;
           manager.pushEvent( event ) ;
           break ;
         case WM_LBUTTONUP :
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyUp, kgl::MouseButton::LeftClick ) ;
           manager.pushEvent( event ) ;
           break ;                                                                                
         case WM_RBUTTONDOWN :
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyDown, kgl::MouseButton::RightClick ) ;
           manager.pushEvent( event ) ;
           break ;
         case WM_RBUTTONUP :
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyUp, kgl::MouseButton::RightClick ) ;
           manager.pushEvent( event ) ;
           break ;                                                                                
         case WM_XBUTTONDOWN :
           button = GET_XBUTTON_WPARAM( wParam ) ;
           if( button == XBUTTON1 )
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyDown, kgl::MouseButton::Button01 ) ;
           else
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyDown, kgl::MouseButton::Button02 ) ;

           manager.pushEvent( event ) ;
           break ;                                                                                
         case WM_XBUTTONUP :
           button = GET_XBUTTON_WPARAM( wParam ) ;
           if( button == XBUTTON1 )
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyUp, kgl::MouseButton::Button01 ) ;
           else
           event = kgl::makeMouseButtonEvent( kgl::Event::Type::KeyUp, kgl::MouseButton::Button02 ) ;

           manager.pushEvent( event ) ;
           break ;
      };

      return DefWindowProc( handle, message, wParam, lParam ) ;
    }
    
    WindowData::WindowData()
    {
      this->height = 0  ;
      this->width  = 0  ;
      this->title  = "" ;
    }
    void WindowData::createWindow()
    {
      static const char* CLASS_NAME = "KGL_WIN32_WINDOW" ;
      this->instance = win_module ;
      this->window_class               = {}                      ;
      this->window_class.hInstance     = this->instance          ;
      this->window_class.lpfnWndProc   = processWindow           ;
      this->window_class.lpszClassName = CLASS_NAME              ;  
      this->window_class.cbSize        = sizeof( WNDCLASSEX )    ;
      this->window_class.style         = CS_HREDRAW | CS_VREDRAW ;
      this->window_class.cbClsExtra    = 0                       ;
      this->window_class.cbWndExtra    = 0                       ;
      this->window_class.hIcon         = LoadIcon( this->instance, MAKEINTRESOURCE( IDI_APPLICATION ) ) ;
      this->window_class.hCursor       = LoadCursor( nullptr, IDC_ARROW ) ;
      this->window_class.hbrBackground = reinterpret_cast<HBRUSH>( GetStockObject( BLACK_BRUSH ) ) ;
      this->window_class.lpszMenuName  = nullptr                 ;
      this->window_class.lpszClassName = CLASS_NAME              ;
      this->window_class.hIcon         = LoadIcon( this->instance, MAKEINTRESOURCE( IDI_APPLICATION ) ) ;

      if (!RegisterClassEx(&this->window_class))
      {
        exit( 1 ) ;
      }

      
      this->handle = CreateWindowEx( 0, CLASS_NAME, this->title.c_str(), WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT, this->width, this->height,
                                     nullptr, nullptr, this->instance, nullptr ) ;
      
      if( this->handle == nullptr )
      {
       // TODO: Error here.
       exit( 1 ) ;
      }
      else
      {
        ShowWindow( this->handle, SW_SHOW ) ;
      }
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
      
      data().createWindow() ;
    }

    void Window::setMonitor( unsigned monitor_id )
    {
    
    }

    void Window::setFullscreen( bool value )
    {
    
    }

    void Window::setResizable( bool value )
    {
    
    }

    void Window::setBorderless( bool value )
    {
    
    }

    void Window::setMinimize( bool value )
    {
    
    }

    void Window::setMaximized( bool value )
    {
    
    }

    void Window::handleEvents()
    {
      MSG  message        ;
      BOOL exit_condition ;

      message = {};

      while ( ( exit_condition = PeekMessage( &message, data().handle, 0, 0, PM_REMOVE ) ) != 0 )
      {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
    }
    
    const HWND& Window::handle() const
    {
      return data().handle ;
    }

    const HINSTANCE& Window::instance() const
    {
      return data().instance ;
    }

    void Window::reset()
    {
    
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
