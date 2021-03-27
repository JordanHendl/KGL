#/usr/bin/python3

import os
import sys 
from pathlib import Path
from nyx_model import *

material_map = {}

def convertToMtl( filename ):
  path = Path( filename )
  extensions = "".join( Path(path).suffixes )
  return str( Path( str( path ).replace( extensions, ".mtl" ) ) )
  
def filePath( filename ):
  return filename.split( filename.split( '/' )[ -1 ] )[ 0 ]

def getName( filename ) :
  tmp = filename.split( '/' )[ -1 ]
  path = Path( tmp )
  extensions = "".join( Path( path ).suffixes )
  return str( Path( str( path ).replace( extensions, "" ) ) )

def readMaterials( filename ):
  name            =  ""
  ambient         = [ 0.0, 0.0, 0.0 ]
  diffuse         = [ 0.0, 0.0, 0.0 ]
  specular        = [ 0.0, 0.0, 0.0 ]
  transparency    = 0.0
  trans_filter    = [ 0.0, 0.0, 0.0 ]
  optical_density = 0.0
  
  if( os.path.isfile( filename ) ):
    file = open( filename, "r" )
    
    line = file.readline()
  
    material = Material()
    dirty   = False
    while( line ):
      list = line.split()
      
      if( len( list ) > 0 ):
        if( list[ 0 ] == "newmtl" ):
          if( dirty ):
            material_map[ material.name ] = material
            material = Material()
            dirty = False
            
          material.name = list[ 1 ]
          
        if( list[ 0 ] == "Ka" ):
          material.ambient = [ float( list[ 1 ] ), float( list[ 2 ] ), float( list[ 3 ] ) ]
          dirty = True
        if( list[ 0 ] == "Kd" ):
          material.diffuse = [ float( list[ 1 ] ), float( list[ 2 ] ), float( list[ 3 ] ) ]
          dirty = True
        if( list[ 0 ] == "Ks" ):
          material.specular = [ float( list[ 1 ] ), float( list[ 2 ] ), float( list[ 3 ] ) ]
          dirty = True
          
        if( list[ 0 ] == "d" or list[ 0 ] == "Tr" ):
          material.transparency = float( list[ 1 ] ) 
          dirty = True
      
        if( list[ 0 ] == "Ni" ):
          material.refractiveness = float( list[ 1 ] )
          dirty = True
        
        if( list[ 0 ] == "map_Ka" ):
          material.ambient_tex = getName( list[ 1 ] )
          dirty = True
          
        if( list[ 0 ] == "map_Kd" ):
          material.diffuse_tex = getName( list[ 1 ] )
          dirty = True
          
        if( list[ 0 ] == "map_Ks" ):
          material.specular_tex = getName( list[ 1 ] )
          dirty = True
          
        if( list[ 0 ] == "map_Ns" ):
          material.highlight_tex = getName( list[ 1 ] )
          dirty = True
          
        if( list[ 0 ] == "map_d"  ):
          material.alpha_tex = getName( list[ 1 ] )
          dirty = True
          
        if( list[ 0 ] == "map_bump"  ):
          material.bump_tex = getName( list[ 1 ] )
          dirty = True
          
        if( list[ 0 ] == "disp"  ):
          material.displacement_tex = getName( list[ 1 ] )
          dirty = True
          
      line = file.readline()
    
    material_map[ material.name ] = material
    print( f"Loading material file: {filename}" )
    file.close()
    
  

def readOBJ( filename ):
  meshes       = [] 
  vertices     = []
  normals      = []
  uvs          = []
  faces        = []
  materials    = []
  
  name   = "obj"
  
  file = open( filename, "r" )
  line = file.readline()

  while( line ):
    list = line.split()
    if( list[ 0 ] == "mtllib" ):
      readMaterials( filePath( filename ) + list[ 1 ] )
      
    if( list[ 0 ] == "o" ):
      # If we have already seen a mesh, we need to append this one to the list, then start again.
      if( len( vertices ) != 0 ):
        meshes.append( Mesh( name, vertices, faces, textures, materials ) )
        vertices .clear()
        faces    .clear()
        textures .clear()
        materials.clear()

      name = list[ 1 ]

    if( list[ 0 ] == "v" ):
      vertex = Vertex( list[ 1 ], list[ 2 ], list[ 3 ], 1.0 )
      vertices.append( vertex )
    
    if( list[ 0 ] == "vt" ):
      uv = UV( list[ 1 ], list[ 2 ] )
      uvs.append( uv )

    if( list[ 0 ] == "vn" ):
      normal = Vertex( list[ 1 ], list[ 2 ], list[ 3 ], 1.0 )
      normals.append( normal )
    
    if( list[ 0 ] == "usemtl" ):
      if( list[ 1 ] in material_map.keys() ):
        materials.append( material_map[ list[ 1 ] ] )
      
    if( list[ 0 ] == "f" ):
      indices = []
      index   = 1
      while( index < 4 ):

        face_split = list[ index ].split( '/' )
        id         = int( face_split[ 0 ] ) - 1

        indices.append( id )

        if( len( face_split ) >= 2 ):
          idv = int( face_split[ 1 ] ) - 1
          uv  = uvs[ idv ]
          vertices[ id ].setUV( uv )
  
        if( len( face_split ) > 2 ):
          idn    = int( face_split[ 2 ] ) - 1
          normal = normals[ idn ]
          vertices[ id ].setNormal( normals[ idn ] )

        index += 1

      face = Face( indices[ 0 ], indices[ 1 ], indices[ 2 ] )
      indices.clear()
      faces.append( face )

    line = file.readline()

  file.close()

  meshes.append( Mesh( name, vertices, faces, materials ) )
  model = Model( meshes )

  return model
