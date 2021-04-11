#/usr/bin/python3

import sys 
import nyx_model
import struct
import json

MAGIC = 0x757775

def write( model, output_name ):
  
  contents = {}
  for mesh in model.meshes:
    contents[ mesh.name ] = {} ;
    contents[ mesh.name ][ "num_vertices"  ] =  len( mesh.vertices  )
    contents[ mesh.name ][ "num_faces"     ] =  len( mesh.faces     )
    contents[ mesh.name ][ "vertices"      ] = [] ;
    contents[ mesh.name ][ "faces"         ] = [] ;
    contents[ mesh.name ][ "material"      ] = [] ;

    for face in mesh.faces:
      contents[ mesh.name ][ "vertices" ].append( mesh.vertices[ face.x ].__dict__ )
      contents[ mesh.name ][ "vertices" ].append( mesh.vertices[ face.y ].__dict__ )
      contents[ mesh.name ][ "vertices" ].append( mesh.vertices[ face.z ].__dict__ )
      
    for face in mesh.faces:
      contents[ mesh.name ][ "faces" ].append( face.x )
      contents[ mesh.name ][ "faces" ].append( face.y )
      contents[ mesh.name ][ "faces" ].append( face.z )

    for material in mesh.materials:
      contents[ mesh.name ][ "material" ].append( material.__dict__ )
     
  file = open( output_name, "w" )
  json.dump( contents, file, indent=2 )
  file.close()
  