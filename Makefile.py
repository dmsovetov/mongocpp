mongocpp = StaticLibrary( 'mongocpp', sources = [ '*' ], defines = [ 'MONGO_BUILD_LIBRARY' ] )
mongocpp.linkExternal( Library( 'mongoc', True ), Library( 'bson', True ) )