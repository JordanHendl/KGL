#/usr/bin/python3

from nyx_model import *
def readMaterials( filename )
  #TODO: Implement reading of the OBJ .MTL file for material info.
  print( filename )

def readOBJ( filename ):
  meshes    = [] 
  vertices  = []
  normals   = []
  uvs       = []
  faces     = []
  textures  = []
  materials = []
  
  name   = "obj"
  
  file = open( filename, "r" )
  line = file.readline()

  while( line ):
    list = line.split()
    

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
  meshes.append( Mesh( name, vertices, faces, textures, materials ) )
  model = Model( meshes )

  return model
