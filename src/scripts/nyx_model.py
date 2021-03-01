#/usr/bin/python3

class Face:
  def __init__( self, x, y, z ):
    self.x = x 
    self.y = y 
    self.z = z 

class Material:
  ambient    = 0.0
  diffuse    = 0.0
  specular   = 0.0
  reflective = 0.0
  shininess  = 0.0

  ambient_tex    = ""
  diffuse_tex    = ""
  specular_tex   = ""
  reflective_tex = ""
  shininess_tex  = ""
  

class UV:
  def __init__( self, u, v ):
    self.u = u
    self.v = v 

class Vertex:
  def __init__( self, x, y, z, w ):
    self.x = x 
    self.y = y 
    self.z = z 
    self.w = w

  def setNormal( self, normal ):
    self.normal_x = normal.x 
    self.normal_y = normal.y 
    self.normal_z = normal.z 
 
  def setUV( self, uv ):
    self.u = uv.u
    self.v = uv.v 

class Mesh:
  def __init__( self, name, vertices, faces, textures, materials ):
    self.name      = name
    self.vertices  = vertices 
    self.faces     = faces
    self.textures  = textures 
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
        print( f"     ├─Vertex : {vertex.x}, {vertex.y}, {vertex.z}, {vertex.w}"          )
        print( f"     ├─Normal : {vertex.normal_x}, {vertex.normal_y}, {vertex.normal_z}" )
        print( f"     └─UV     : {vertex.u}, {vertex.v}"                                  )
        print()
        vertex = mesh.vertices[ face.y ]
        print( f"     ├─Vertex : {vertex.x}, {vertex.y}, {vertex.z}, {vertex.w}"          )
        print( f"     ├─Normal : {vertex.normal_x}, {vertex.normal_y}, {vertex.normal_z}" )
        print( f"     └─UV     : {vertex.u}, {vertex.v}"                                  )
        print()
        vertex = mesh.vertices[ face.z ]
        print( f"     ├─Vertex : {vertex.x}, {vertex.y}, {vertex.z}, {vertex.w}"          )
        print( f"     ├─Normal : {vertex.normal_x}, {vertex.normal_y}, {vertex.normal_z}" )
        print( f"     └─UV     : {vertex.u}, {vertex.v}"                                  )
        print() 
