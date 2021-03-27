/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Pass.h
 * Author: jhendl
 *
 * Created on March 23, 2021, 5:26 AM
 */

#pragma once

namespace nyx
{
  namespace vkg
  {
    class Chain ;
    class Pass
    {
      public:
        Pass() = default ;
        ~Pass() = default ;
        void initialize( unsigned device ) ;
        void reset() ;
        Chain& makeChain() ;
      private:
        
    };
  }
}
