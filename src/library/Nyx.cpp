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
 * File:   Library.cpp
 * Author: Jordan Hendl
 * 
 * Created on December 30, 2020, 8:15 PM
 */

#include "Nyx.h"
#include <iostream>

namespace nyx
{
  #if defined ( __unix__ ) || defined( _WIN32 )
    constexpr const char* END_COLOR    = "\x1B[m"     ;
    constexpr const char* COLOR_RED    = "\u001b[31m" ;
    constexpr const char* COLOR_GREEN  = "\u001b[32m" ;
    constexpr const char* COLOR_YELLOW = "\u001b[33m" ;
    constexpr const char* COLOR_GREY   = "\x1B[1;30m" ;
    constexpr const char* UNDERLINE    = "\u001b[4m"  ;
  #else
    constexpr const char* END_COLOR    = "" ;
    constexpr const char* COLOR_GREEN  = "" ;
    constexpr const char* COLOR_YELLOW = "" ;
    constexpr const char* COLOR_GREY   = "" ;
    constexpr const char* COLOR_RED    = "" ;
    constexpr const char* COLOR_WHITE  = "" ;
  #endif

  /** The structure to contain all of the global nyx library data.
   */
  struct NyxData
  {
    typedef void ( *Callback )( nyx::Error ) ;

    Callback           error_cb ;
    nyx::ErrorHandler* handler  ;

    /** Default constructor.
     */
    NyxData() ;
  };

  /** Static container for all of the nyx library's global data.
   */
  static NyxData data ;

  const char* colorFromSeverity( nyx::Severity severity )
  {
    switch ( severity )
    {
      case nyx::Severity::Info    : return nyx::COLOR_GREY   ;
      case nyx::Severity::Warning : return nyx::COLOR_YELLOW ;
      case nyx::Severity::Fatal   : return nyx::COLOR_RED    ;
      default : return nyx::COLOR_RED ;
    }
  }

  void defaultHandler( nyx::Error error )
  {
    auto severity = error.severity() ;

    std::cout << colorFromSeverity( severity ) << "-- " << severity.toString() << " | " << "Nyx::vkg Error: " << error.toString() << "." << nyx::END_COLOR << std::endl ;
    if( severity == nyx::Severity::Fatal ) exit( -1 ) ;
  }

  NyxData::NyxData()
  {
    this->error_cb = &nyx::defaultHandler ;
    this->handler  = nullptr ;
  }

  nyx::Severity::Severity()
  {
    this->sev = Severity::None ;
  }

  nyx::Severity::Severity( const nyx::Severity& severity )
  {
    this->sev = severity.sev ;
  }

  nyx::Severity::Severity( unsigned error )
  {
    this->sev = error ;
  }

  unsigned nyx::Severity::severity() const
  {
    return *this ;
  }

  nyx::Severity::operator unsigned() const
  {
    return this->sev ;
  }

  const char* nyx::Severity::toString() const
  {
    switch( this->sev )
    {
      case Severity::Warning : return "Warning" ;
      case Severity::Fatal   : return "Fatal"   ;
      case Severity::None    : return "None"    ;
      case Severity::Info    : return "Info"    ;
      default : return "Unknown Severity" ;
    }
  }

  nyx::Error::Error()
  {
    this->err = Error::None ;
  }

  nyx::Error::Error( const nyx::Error& error )
  {
    this->err = error.err ;
  }

  nyx::Error::Error( unsigned error )
  {
    this->err = error ;
  }

  unsigned nyx::Error::error() const
  {
    return *this ;
  }

  nyx::Error::operator unsigned() const
  {
    return this->err ;
  }

  const char* nyx::Error::toString() const
  {
    switch( this->err )
    {
      case nyx::Error::InvalidImageConversion : return "A Framework image has been converted to a Nyx Image with a mismatched format." ;
      default : return "Unknown Error" ;
    }
  }

  nyx::Severity nyx::Error::severity() const
  {
    switch( this->err )
    {
      case nyx::Error::InvalidImageConversion : return nyx::Severity::Warning ;
      default : return nyx::Severity::Fatal ;
    }
  }

  void handleError( nyx::Error error )
  {
    if( error != nyx::Error::Success )
    {
      if( data.error_cb != nullptr )
      {
        ( data.error_cb )( error ) ;
      }

      if( data.handler != nullptr )
      {
        data.handler->handleError( error ) ;
      }
    }
  }
  void setErrorHandler( void ( *error_handler )( nyx::Error ) )
  {
    data.error_cb = error_handler ;
  }
  
  void setErrorHandler( nyx::ErrorHandler* handler )
  {
    data.handler = handler ;
  }
}