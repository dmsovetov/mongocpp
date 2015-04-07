/**************************************************************************
 
 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include "Collection.h"

namespace mongo {

// ** Collection::Collection
Collection::Collection( mongoc_collection_t* collection ) : m_collection( collection )
{

}

Collection::~Collection( void )
{
    mongoc_collection_destroy( m_collection );
}

// ** Collection::drop
void Collection::drop( void )
{
    bson_error_t err;
    if( !mongoc_collection_drop( m_collection, &err ) ) {
        printf( "Collection::drop : %s\n", err.message );
    }
}

// ** Collection::find
CursorPtr Collection::find( const BSON& query ) const
{
    mongoc_cursor_t* cursor = mongoc_collection_find( m_collection, MONGOC_QUERY_NONE, 0, 0, 0, query.raw(), NULL, NULL );
    return cursor ? CursorPtr( new Cursor( cursor ) ) : NULL;
}

// ** Collection::findOne
DocumentPtr Collection::findOne( const BSON& query ) const
{
    CursorPtr cursor = find( query );
    return cursor != NULL ? cursor->next() : NULL;
}

// ** Collection::update
bool Collection::update( const BSON& query, const BSON& value )
{
    bson_error_t err;
    if( !mongoc_collection_update( m_collection, MONGOC_UPDATE_NONE, query.raw(), value.raw(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }
    
    return true;
}

// ** Collection::upsert
bool Collection::upsert( const BSON& query, const BSON& value )
{
    bson_error_t err;
    if( !mongoc_collection_update( m_collection, MONGOC_UPDATE_UPSERT, query.raw(), value.raw(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }

    return true;
}

// ** Collection::insert
bool Collection::insert( const BSON& value )
{
    bson_error_t err;
    if( !mongoc_collection_insert( m_collection, MONGOC_INSERT_NONE, value.raw(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }
    
    return true;
}

// ** Collection::remove
bool Collection::remove( const BSON& query )
{
    bson_error_t err;
    if( !mongoc_collection_remove( m_collection, MONGOC_REMOVE_NONE, query.raw(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }
    
    return true;
}

// ** Collection::count
int Collection::count( const BSON& query ) const
{
    bson_error_t err;
    int64_t result = mongoc_collection_count( m_collection, MONGOC_QUERY_NONE, query.raw(), 0, 0, NULL, &err );

    return ( int )result;
}

// ** Collection::mergeWith
void Collection::mergeWith( const CollectionPtr& other, bool drop )
{
	assert( false );
//    CursorPtr   cursor = other->find();
//    DocumentPtr document;
//
//    while( (document = cursor->next()) ) {
//        BSON value = BSON::fromBSON( document->value() );
//        value.remove( "_id" );
//        insert( value );
//    }
//
//    if( drop ) {
//        other->drop();
//    }
}

// ** Collection::ensureIndex
bool Collection::ensureIndex( const std::string& name, const mongo::BSON &keys, bool unique )
{
    mongoc_index_opt_t opt;
    mongoc_index_opt_init( &opt );
    opt.unique  = unique;
    opt.name    = name.c_str();

    bson_error_t err;
    if( !mongoc_collection_create_index( m_collection, keys.raw(), &opt, &err ) ) {
        printf( "Collection::ensureIndex : %s\n", err.message );
        return false;
    }

    return true;
}

// ** Collection::dropIndex
bool Collection::dropIndex( const std::string& name )
{
    bson_error_t err;
    if( !mongoc_collection_drop_index( m_collection, name.c_str(), &err ) ) {
        printf( "Collection::dropIndex : %s\n", err.message );
        return false;
    }

    return true;
}

// ** Collection::createBulkOperation
BulkOperationPtr Collection::createBulkOperation( void )
{
    return BulkOperationPtr( new BulkOperation( mongoc_collection_create_bulk_operation( m_collection, false, NULL ) ) );
}

} // namespace mongo