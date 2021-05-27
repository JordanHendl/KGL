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
 * File:   Event.h
 * Author: Jordan Hendl
 *
 * Created on January 5, 2021, 8:18 PM
 */

#pragma once 

namespace nyx
{
  class      Event       ;
  enum class Key         ;
  enum class MouseButton ;
  
  /** Method to retrieve a C-string from an event.
   * @param event The event to convert to string.
   * @return The C-string representation of the event.
   */
  const char* toString( const Event& event ) ;

  /** Method to convert a Key to a string.
   * @param key The key to convert.
   * @return The string representation of the key.
   */
  const char* toString( const Key& key ) ;

  /** Method to retrieve a string representation of a MouseButton.
   * @param button The button to convert to string.
   * @return The string representation of the button.
   */
  const char* toString( const MouseButton& button ) ;

  /** All keys mapped for return from the NYX event handler.
   */
  enum class Key
  {
    None,
    ESC,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    Backtick,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Zero,
    Hyphen,
    Equals,
    Backspace,
    LTab,
    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    LBracket,
    RBracket,
    BSlash,
    Capslock,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    Semicolon,
    Apostraphe,
    Return,
    LShift,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    Comma,
    Period,
    FSlash,
    LCtrl,
    WinKey,
    LAlt,
    Space,
    RAlt,
    Fn,
    RCtrl,
    Left,
    Up,
    Right,
    Down
  };
 
  /** NYX Mouse Button Events
   */
  enum class MouseButton
  {
    None,
    LeftClick,
    MiddleClick,
    RightClick,
    WheelUp,
    WheelDown,
    Button01,
    Button02
  };
  
  /** Class to encapsulate a NYX event.
   */
  class Event
  {
    public:
      
      /** The types of event possible.
       */
      enum class Type : unsigned
      {
        None,
        KeyDown,
        KeyUp,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheelUp,
        MouseWheelDown,
        JoystickAxis,
        JoystickButton,
        JoystickInitialize,
        WindowExit,
      };
      
      /** Default constructor.
       */
      Event() ;
      
      /** Copy constructor.
       * @param event The event to copy from.
       */
      Event( const Event& event ) ;

      /** Default constructor.
       */
      ~Event() = default ;
      
      /** Method to retrieve the type of event.
       * @return Returns the type of event.
       */
      Type type() const ;
      
      /** Method to retrieve the key for this event.
       * @note, is Key::None if this is not a key event.
       * @return The key associated with this event.
       */
      Key key() const ;
      
      /** Method to retrieve the mouse button for this event.
       * @note is MouseButton::None if this event was not a mouse button event.
       * @return The mouse button associated with this event.
       */
      MouseButton button() const ;

    private:
      
      /** Friend function to make a key event.
       */
      friend Event makeKeyEvent( Event::Type type, nyx::Key key ) ;
      
      /** Friend function to make a button event.
       */
      friend Event makeMouseButtonEvent( Event::Type type, nyx::MouseButton button ) ;

      Type        event_type   ;
      Key         event_key    ;
      MouseButton event_button ;
  };
  
    /** Method to retrieve a C-string from an event.
   * @param event The event to convert to string.
   * @return The C-string representation of the event.
   */
  const char* toString( const Event::Type& event ) ;
  
  /** Function to create a key event from the input parameters.
   * @param type The type of event.
   * @param key The key that was pressed.
   * @return The created event.
   */
  Event makeKeyEvent( Event::Type type, nyx::Key key ) ;
  
  /** Function to create a mouse button event from the input params.
   * @param type The type of event.
   * @param button The button that was pressed.
   * @return The created event.
   */
  Event makeMouseButtonEvent( Event::Type type, nyx::MouseButton button ) ;

  /** Class to manage library event handling.
   */
  class EventManager
  {
    public:
      typedef void ( *EventCallback )( const Event& ) ;

      /** Class for recieving published data through function pointers AKA 'setters'.
       */
      class Subscriber
      {
        public:
          /** Virtual deconstructor.
           */
          virtual ~Subscriber() {} ;
          
          /** Method to fullfill a subscription using the input pointer.
           * @param pointer Pointer to the data requested by this object's subscription.
           * @param idx The index to use for the subscription.
           */
          virtual void execute( const Event& event ) = 0 ;
      };
      
      /** Default constructor.
       */
      EventManager() ;
      
      /** Deconstructor. Resets this object & removes all callbacks registered with it.
       */
      ~EventManager() ;
      
      /** Method to push an event to the manager.
       * @param event The event to push to all valid subscribers.
       */
      void pushEvent( const Event& event ) ;

      /** Method to subscribe a callback with the manager.
       * @param callback The callback to subscribe with.
       * @param key The key to use to reference the callback.
       */
      void enroll( EventCallback callback, const char* key ) ;
      
      /** Method to subscribe a callback with the manager.
       * @param callback The callback to subscribe with.
       * @param type The type of event to recieve signals for.
       * @param key The key to use to reference the callback.
       */
      void enroll( EventCallback callback, nyx::Event::Type type, const char* key ) ;
      
      /** Method to subscribe a callback with the manager.
       * @param callback The callback to subscribe with.
       * @param keysym The key to recieve signals for.
       * @param key The key to use to reference the callback.
       */
      void enroll( EventCallback callback, nyx::Key keysym,  const char* key ) ;
      
      
      /** Method to subscribe a callback with the manager.
       * @param obj The object to use for calling the callback.
       * @param callback The callback to subscribe with.
       * @param key The key to use to reference the callback.
       */
      template<class Object>
      void enroll( Object* obj, void (Object::*callback)( const Event& ), const char* key ) ;
      
      /** Method to subscribe a callback with the manager.
       * @param obj The object to use for calling the callback.
       * @param callback The callback to subscribe with.
       * @param type The type of events to recieve signals for.
       * @param key The key to use to reference the callback.
       */
      template<class Object>
      void enroll( Object* obj, void (Object::*callback)( const Event& ), nyx::Event::Type type, const char* key ) ;

      /** Method to subscribe a callback with the manager.
       * @param obj The object to use for calling the callback.
       * @param callback The callback to subscribe with.
       * @param keysym The library key to recieve signals for.
       * @param key The key to use to reference the callback.
       */
      template<class Object>
      void enroll( Object* obj, void (Object::*callback)( const Event& ), nyx::Key keysym, const char* key ) ;
      
      /** Method to set the current mouse's position.
       * @param x The x-cooordinate on the screen.
       * @param y The y-cooordinate on the screen.
       */
      void updateMouse( float x, float y ) ;
      
      /** Method to set the current mouse's position.
       * @param x The x-cooordinate on the screen.
       * @param y The y-cooordinate on the screen.
       */
      void updateMouseOffset( float x, float y ) ;
      
      float mouseX() const ;
      float mouseY() const ;
      float mouseDeltaX() const ;
      float mouseDeltaY() const ;
      
      /** Method to remove all callbacks from this object.
       */
      void reset() ;
    private:

      /** Template class to encapsulate a subscriber that recieves data via object.
       */
      template<class Object>
      class MethodSubscriber : public Subscriber
      {
        public:
          typedef void ( Object::*Callback )( const nyx::Event& ) ;
          
          MethodSubscriber( Object* obj, Callback callback ) ;
          
          void execute( const Event& event ) ;
        private:
          Object*  object   ;
          Callback callback ;
      };

      /** Template class to encapsulate a subscriber that recieves data via function.
       */
      class FunctionSubscriber : public Subscriber
      {
        public:
          typedef void (*Callback)( const Event& ) ;

          FunctionSubscriber( Callback callback ) ;
          
          void execute( const Event& event ) ;
        private:
          Callback callback ;
      };
      
      /** Base cases to recieve data from template functions.
       */
      void enrollBase( Subscriber* subscriber, const char* key ) ;
      void enrollBase( Subscriber* subscriber, nyx::Event::Type type, const char* key ) ;
      void enrollBase( Subscriber* subscriber, nyx::Key keysym, const char* key ) ;
      
      /** Forward-declared structure to contain this object's internal data.
       */
      struct EventManagerData* manager_data ;
      
      /** Method to retrieve a reference to this object's internal data.
       * @return Reference to this object's internal data.
       */
      EventManagerData& data() ;
      
      /** Method to retrieve a const-reference to this object's internal data.
       * @return Const-reference to this object's internal data.
       */
      const EventManagerData& data() const ;
  };
  
  template<class Object>
  EventManager::MethodSubscriber<Object>::MethodSubscriber( Object* obj, Callback callback )
  {
    this->object   = obj      ;
    this->callback = callback ;
  }
  
  template<class Object>
  void EventManager::MethodSubscriber<Object>::execute( const Event& event )
  {
   ( ( this->object )->*( this->callback ) )( event ) ;
  }
  
  template<class Object>
  void EventManager::enroll( Object* obj, void (Object::*setter)( const Event& ), const char* key )
  {
    typedef EventManager::MethodSubscriber<Object> Callback ;
    Callback *callback ;
    
    callback = new Callback( obj, reinterpret_cast<typename Callback::Callback>( setter ) ) ;
    
    this->enrollBase( callback, key ) ;
  }
  
  template<class Object>
  void EventManager::enroll( Object* obj, void (Object::*setter)( const Event& ), nyx::Event::Type type, const char* key )
  {
    typedef EventManager::MethodSubscriber<Object> Callback ;
    Callback *callback ;
    
    callback = new Callback( obj, reinterpret_cast<typename Callback::Callback>( setter ) ) ;
    
    this->enrollBase( callback, type, key ) ;
  }

  template<class Object>
  void EventManager::enroll( Object* obj, void (Object::*setter)( const Event& ), nyx::Key keysym, const char* key )
  {
    typedef EventManager::MethodSubscriber<Object> Callback ;
    Callback *callback ;
    
    callback = new Callback( obj, reinterpret_cast<typename Callback::Callback>( setter ) ) ;
    
    this->enrollBase( callback, keysym, key ) ;
  }
}

