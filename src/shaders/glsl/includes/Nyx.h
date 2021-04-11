/*
 * Copyright (C) 2021 Jordan Hendl
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
 * File:   Nyx.h
 * Author: Jordan Hendl
 *
 * Created on January 28, 2021, 7:23 AM
 */

#ifndef NYXGLSL_H
#define NYXGLSL_H

#extension GL_EXT_buffer_reference    : enable 
#extension GL_EXT_buffer_reference2   : enable 
#extension GL_EXT_scalar_block_layout : enable 

#define NyxBuffer layout( buffer_reference, scalar ) buffer
#define NyxPushConstant layout( push_constant ) uniform
#define nyx_seek( iter, pos  ) ( iter.position = pos < iter.position ? pos : iter.position ) ;
#define nyx_get(  buff, iter ) buff[ iter.position ]
#define nyx_set(  buff, iter, value ) buff[ iter.position ] = value
struct NyxIterator
{
  uint size         ;
  uint element_size ;
  uint position     ;
};

#endif /* NYXGLSL_H */

