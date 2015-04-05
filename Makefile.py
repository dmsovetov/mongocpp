mongocpp = StaticLibrary( 'mongocpp', sources = [ '*' ] )
mongocpp.linkExternal( Library( 'mongoc', True ), Library( 'bson', True ) )