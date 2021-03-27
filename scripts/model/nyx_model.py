#/usr/bin/python3

import json

class Face:
  def __init__( self, x, y, z ):
    self.x = x 
    self.y = y 
    self.z = z 

class Material:
  name           = ""
  ambient        = [ 0.0, 0.0, 0.0 ]
  diffuse        = [ 0.0, 0.0, 0.0 ]
  specular       = [ 0.0, 0.0, 0.0 ]
  reflective     = 0.0
  shininess      = 0.0
  transparency   = 1.0
  refractiveness = 0.0
  
  ambient_tex      = ""
  diffuse_tex      = ""
  specular_tex     = ""
  reflective_tex   = ""
  shininess_tex    = ""
  displacement_tex = ""
  alpha_tex        = ""
  bump_tex         = ""
  highlight_tex    = ""
  
class UV:
  def __init__( self, u, v ):
    self.u = u
    self.v = v 

class Vertex:
  def __init__( self, x, y, z, w ):
    self.x = float( x ) 
    self.y = float( y ) 
    self.z = float( z ) 
    self.w = float( w )

  def setNormal( self, normal ):
    self.nx = float( normal.x )
    self.ny = float( normal.y )
    self.nz = float( normal.z )
 
  def setUV( self, uv ):
    self.u = float( uv.u )
    self.v = float( uv.v )

  x  = 0.0
  y  = 0.0
  z  = 0.0
  w  = 0.0
  u  = 0.0
  v  = 0.0
  nx = 0.0
  ny = 0.0
  nz = 0.0

class Mesh:
  def __init__( self, name, vertices, faces, materials ):
    self.name      = name
    self.vertices  = vertices 
    self.faces     = faces
    self.materials = materials 

class Model:
  def __init__( self, meshes ):
    self.meshes = meshes 
    
def printModel( model ):
    mesh_index = 0
    for mesh in model.meshes:
      print( f"  Mesh ( {mesh.name}, {mesh_index} )" )
      mesh_index += 1 
      face_index =  0
      for face in mesh.faces:
        print( f"  Face: {face_index} ")
        face_index += 1 
        
        vertex = mesh.vertices[ face.x ]
        print( f"     ├─Vertex : {vertex.x}, {vertex.y}, {vertex.z}, {vertex.w}" )
        print( f"     ├─Normal : {vertex.nx}, {vertex.ny}, {vertex.nz}"          )
        print( f"     └─UV     : {vertex.u}, {vertex.v}"                         )
        print()
        vertex = mesh.vertices[ face.y ]
        print( f"     ├─Vertex : {vertex.x}, {vertex.y}, {vertex.z}, {vertex.w}" )
        print( f"     ├─Normal : {vertex.nx}, {vertex.ny}, {vertex.nz}"          )
        print( f"     └─UV     : {vertex.u}, {vertex.v}"                         )
        print()
        vertex = mesh.vertices[ face.z ]
        print( f"     ├─Vertex : {vertex.x}, {vertex.y}, {vertex.z}, {vertex.w}" )
        print( f"     ├─Normal : {vertex.nx}, {vertex.ny}, {vertex.nz}"          )
        print( f"     └─UV     : {vertex.u}, {vertex.v}"                         )
        print() 
