#! /usr/bin/python3

import os
import sys 
import nyx_model     
import nyx_obj_reader
import nyx_model_writer

from pathlib import Path
from typing  import Union

class GlobalState:
  verbose   = False 
  recursive = False

global_state = GlobalState()

def printUsage():
  print( " Usage: nyx_model_converter <options> <path_to_models>                            " )
  print( "    Options:                                                                      " )
  print( "     -o/-O <param>                                                                " )
  print( "         The name to associate with the output .nmp file.                         " )
  print( "                                                                                  " )
  print( "     -v/-V /-verbose                                                              " )
  print( "         Whether to use verbose output when converting models.                    " )
  print( "                                                                                  " )
  print( "     -r/-R                                                                        " )
  print( "         Whether to recursively load files, assumes paths to directories.         " )
  print( "         Expects output flag to point to the directory to store converted models. " )

def convert( name ):
  if ( name.find( ".obj" ) != -1 ):
    return nyx_obj_reader.readOBJ( name )  
    
  elif ( name.find( ".dae" ) != -1 ):
    print( "Found Collada!" )

  elif ( name.find( ".fbx" ) != -1 ):
    print( "Found FBX!" )

def convertName( name ):
  path = Path( name )
  extensions = "".join(Path(path).suffixes)
  return str( Path( str( path ).replace( extensions, ".nmp" ) ) )

def getFileName( name ):
  return name.split( '/' )[ -1 ]

def main():
  models    = []
  outputs   = []
  path      = ""
  output    = ""
  verbose   = False
  recursive = False
  index     = 0
  
  if( len( sys.argv ) == 1 ):
    printUsage()
   
  while index < len( sys.argv ):
    arg = sys.argv[ index ]
    if ( arg == "-v" or arg == "-verbose" ):
      verbose = True
    elif ( arg == "-r" or arg == "-R" ):  
      recursive = True
    elif ( arg == "-o" or arg == "-O" ):  
      output = sys.argv[ index + 1 ]
      index += 1
    else:
      path = arg
    index += 1
  
  index = 0
  if( recursive == True and path ):
    if( output[ -1 ] != "/" and output[ -1 ] != "\\" ):
      output += "/"

    for root, subdirs, files in os.walk( path ):
      for filename in files:
        name  = os.path.join( root, filename )
        model = convert     ( name           )
        
        models.append( ( name, model ) )
        
        if( model ):
          outputs.append( output + getFileName( convertName( name ) ) )

  elif( path ):
    models.append( ( path, convert( path ) ) )
    if( output ):
      outputs.append( output )
    else:
      outputs.append( output.join( getFileName( convertName( path ) ) ) )
      
  for name, model in models:
    if( model ):
      if( verbose == True ):
        print( f"  Converted model at {name}" )
        nyx_model.printModel( model )
        print()
        print( f"Outputting: {outputs[ index ]}" )
      
      nyx_model_writer.write( model, outputs[ index ] )
      index += 1


if __name__ == '__main__': 
  main()