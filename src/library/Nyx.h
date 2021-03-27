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
 * File:   Library.h
 * Author: Jordan Hendl
 *
 * Created on December 30, 2020, 8:15 PM
 */

#pragma once 

namespace nyx
{
  /** Reflective enumeration for a library error severity.
   */
  class Severity
  {
    public:

      /** The type of error.
       */
      enum
      {
        None,
        Info,
        Warning,
        Fatal,
      };

      /** Default constructor.
       */
      Severity() ;

      /** Copy constructor. Copies the input.
       * @param severity The input to copy.
       */
      Severity( const Severity& severity ) ;

      /** Copy constructor. Copies the input.
       * @param error The input to copy.
       */
      Severity( unsigned error ) ;

      /** Method to convert this error into a string.
       * @return The C-string representation of this severity.
       */
      const char* toString() const ;

      /** Method to retrieve the error associated with this object.
       * @return The severity associated with this object.
       */
      unsigned severity() const ;

      /** Conversion operator of this object to an unsigned integer representing the error.
       * @return The severity of this object.
       */
      operator unsigned() const ;

    private:

      /** Underlying variable holding the severity of this object.
       */
      unsigned sev ; 
  };

  /** Reflective enumeration for a library error.
   */
  class Error
  {
    public:

      /** The type of error.
       */
      enum
      {
        None,                   ///< No Error.
        Success,                ///< Success. Nothing to do.
        InvalidNMP,             ///< An invalid parameter was found while parsing an .nmp file.
        InvalidImageConversion, ///< A templated image imported an incompatible implementation image.
        LooseMemory,            ///< An object was deconstructed without freeing it's memory.
      };

      /** Default constructor.
       */
      Error() ;

      /** Copy constructor. Copies the input.
       * @param error The input to copy.
       */
      Error( const Error& error ) ;

      /** Copy constructor. Copies the input.
       * @param error The input to copy.
       */
      Error( unsigned error ) ;

      /** Method to retrieve the severity of this error.
       * @return Method to retrieve the severity of this error.
       */
      nyx::Severity severity() const ;

      /** Method to convert this error into a string.
       * @return The C-string representation of this error.
       */
      const char* toString() const ;

      /** Method to retrieve the error associated with this object.
       * @return The error associated with this object.
       */
      unsigned error() const ;

      /** Conversion operator of this object to an unsigned integer representing the error.
       * @return The error of this object.
       */
      operator unsigned() const ;

    private:

      /** Underlying variable holding the error of this object.
       */
      unsigned err ; 
  };

  /** Abstract class for an error handler.
   * If you wish to have an object handle errors, inherit this class and implement the handleError() function.
   */
  class ErrorHandler
  {
    public:

      /** Virtual method to handle a library error.
       * @param error The error to handle.
       */
      virtual void handleError( nyx::Error error ) = 0 ;

      /** Virtual deconstructor for inheritance.
       */
      virtual ~ErrorHandler() ;
  };
  
  /** Static function to allow a custom error handler to be set for this library.
   * @param error_handler The error handler to be used by this library.
   */
  void setErrorHandler( void ( *error_handler )( nyx::Error ) ) ;

  /** Static function to allow a custom error handler to be set for this library.
   * @param handler The error handler to be used by this library.
   */
  void setErrorHandler( nyx::ErrorHandler* handler ) ;
  
  /** Static function to handle a library error.
   * @param error
   */
  void handleError( nyx::Error error ) ;
}
