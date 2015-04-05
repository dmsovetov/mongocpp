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

#include "Mongo.h"

namespace mongo {

// ** Connection::Connection
Connection::Connection( const std::string& host, const std::string& db ) : m_db( db )
{
    mongoc_init();
    m_client = mongoc_client_new( host.c_str() );
}

Connection::~Connection( void )
{
    mongoc_client_destroy( m_client );
}

// ** Connection::collection
CollectionPtr Connection::collection( const std::string& name )
{
    return CollectionPtr( new Collection( mongoc_client_get_collection( m_client, m_db.c_str(), name.c_str() ) ) );
}

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
    mongoc_cursor_t* cursor = mongoc_collection_find( m_collection, MONGOC_QUERY_NONE, 0, 0, 0, query.value(), NULL, NULL );
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
    if( !mongoc_collection_update( m_collection, MONGOC_UPDATE_NONE, query.value(), value.value(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }
    
    return true;
}

// ** Collection::upsert
bool Collection::upsert( const BSON& query, const BSON& value )
{
    bson_error_t err;
    if( !mongoc_collection_update( m_collection, MONGOC_UPDATE_UPSERT, query.value(), value.value(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }

    return true;
}

// ** Collection::insert
bool Collection::insert( const BSON& value )
{
    bson_error_t err;
    if( !mongoc_collection_insert( m_collection, MONGOC_INSERT_NONE, value.value(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }
    
    return true;
}

// ** Collection::remove
bool Collection::remove( const BSON& query )
{
    bson_error_t err;
    if( !mongoc_collection_remove( m_collection, MONGOC_REMOVE_NONE, query.value(), NULL, &err ) ) {
        printf( "Error: %s\n", err.message );
        return false;
    }
    
    return true;
}

// ** Collection::count
int Collection::count( const BSON& query ) const
{
    bson_error_t err;
    int64_t result = mongoc_collection_count( m_collection, MONGOC_QUERY_NONE, query.value(), 0, 0, NULL, &err );

    return ( int )result;
}

// ** Collection::mergeWith
void Collection::mergeWith( const CollectionPtr& other, bool drop )
{
    CursorPtr   cursor = other->find();
    DocumentPtr document;

    while( (document = cursor->next()) ) {
        BSON value = BSON::fromBSON( document->value() );
        value.remove( "_id" );
        insert( value );
    }

    if( drop ) {
        other->drop();
    }
}

// ** Collection::ensureIndex
bool Collection::ensureIndex( const std::string& name, const mongo::BSON &keys, bool unique )
{
    mongoc_index_opt_t opt;
    mongoc_index_opt_init( &opt );
    opt.unique  = unique;
    opt.name    = name.c_str();

    bson_error_t err;
    if( !mongoc_collection_create_index( m_collection, keys.value(), &opt, &err ) ) {
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

// ** BulkOperation::BulkOperation
BulkOperation::BulkOperation( mongoc_bulk_operation_t* bulk ) : m_bulk( bulk )
{

}

BulkOperation::~BulkOperation( void )
{
    mongoc_bulk_operation_destroy( m_bulk );
}

// ** BulkOperation::insert
void BulkOperation::insert( const BSON& document )
{
    mongoc_bulk_operation_insert( m_bulk, document.value() );
}

// ** BulkOperation::execute
bool BulkOperation::execute( void )
{
    bson_error_t err;

    if( !mongoc_bulk_operation_execute( m_bulk, NULL, &err ) ) {
        printf( "BulkOperation::execute : %s\n", err.message );
        return false;
    }

    return true;
}

// ** Cursor::Cursor
Cursor::Cursor( mongoc_cursor_t* cursor ) : m_cursor( cursor )
{

}

Cursor::~Cursor( void )
{
    mongoc_cursor_destroy( m_cursor );
}

// ** Cursor::clone
CursorPtr Cursor::clone( void )
{
    return CursorPtr( new Cursor( mongoc_cursor_clone( m_cursor ) ) );
}

// ** Cursor::next
DocumentPtr Cursor::next( void )
{
    const bson_t* doc;
    if( !mongoc_cursor_next( m_cursor, &doc ) ) {
        return NULL;
    }

    return DocumentPtr( new Document( bson_copy( doc ) ) );
}

// ** Document::Document
Document::Document( bson_t* document ) : m_document( document )
{

}

Document::~Document( void )
{
    bson_destroy( m_document );
}

// ** Document::_id
OID Document::_id( void ) const
{
    return objectId( "_id" );
}

// ** Document::string
std::string Document::string( const char* key ) const
{
    bson_iter_t iter, field;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_UTF8 );
        return bson_iter_utf8( &field, NULL );
    }

    return "";
}

// ** Document::objectId
OID Document::objectId( const char* key ) const
{
    bson_iter_t iter, field;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_OID );
        const bson_oid_t* oid = bson_iter_oid( &field );
        return OID( oid ? *oid : bson_oid_t() );
    }

    return OID( bson_oid_t() );
}

// ** Document::number
double Document::number( const char* key ) const
{
    bson_iter_t iter, field;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_DOUBLE );
        return bson_iter_double( &field );
    }

    return 0.0;
}

// ** Document::integer
int Document::integer( const char* key ) const
{
    bson_iter_t iter, field;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_INT32 );
        return bson_iter_int32( &field );
    }

    return 0;
}


// ** Document::value
const bson_t* Document::value( void ) const
{
    return m_document;
}

// ** Document::array
DocumentPtr Document::array( const char* key ) const
{
    bson_iter_t iter, field;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_ARRAY );
        
        const uint8_t* data;
        uint32_t       length;
        bson_t         array;

        bson_iter_array( &field, &length, &data );
        bson_init_static( &array, data, length );

        return DocumentPtr( new Document( bson_copy( &array ) ) );
    }

    return NULL;
}

// ** Document::keys
StringSet Document::keys( void ) const
{
    StringSet   result;
    bson_iter_t iter;

    bson_iter_init( &iter, m_document );

    while( bson_iter_next( &iter ) ) {
        result.insert( bson_iter_key( &iter ) );
    }

    return result;
}

// ** Document::integerSet
IntegerSet Document::integerSet( const char* key ) const
{
    bson_iter_t iter, field;
    IntegerSet  result;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_ARRAY );

        bson_iter_t i;
        bson_iter_recurse( &field, &i );

        while( bson_iter_next( &i ) ) {
            assert( bson_iter_type( &i ) == BSON_TYPE_INT32 );
            result.insert( bson_iter_int32( &i ) );
        }
    }
    
    return result;
}

// ** Document::numbers
FloatArray Document::numbers( const char* key ) const
{
    bson_iter_t iter, field;
    FloatArray  result;

    if( bson_iter_init( &iter, m_document ) && bson_iter_find_descendant( &iter, key, &field ) ) {
        assert( bson_iter_type( &field ) == BSON_TYPE_ARRAY );

        bson_iter_t i;
        bson_iter_recurse( &field, &i );

        while( bson_iter_next( &i ) ) {
            assert( bson_iter_type( &i ) == BSON_TYPE_DOUBLE );
            result.push_back( bson_iter_double( &i ) );
        }
    }
    
    return result;
}

// ** OID::OID
OID::OID( bson_oid_t oid ) : m_oid( oid )
{

}

// ** OID::OID
OID::OID( const std::string& oid )
{
    bson_oid_init_from_string( &m_oid, oid.c_str() );
}

// ** OID::value
const bson_oid_t* OID::value( void ) const
{
    return &m_oid;
}

// ** OID::toString
std::string OID::toString( void ) const
{
    char str[64];
    bson_oid_to_string( &m_oid, str );
    return str;
}

// ** OID::operator ==
bool OID::operator == ( const OID& other ) const
{
    return bson_oid_equal( &m_oid, &other.m_oid );
}

// ** BSON::BSON
BSON::BSON( bool isArray ) : m_isArray( isArray ), m_key( isArray ? "0" : "" )
{
    bson_init( &m_bson );
}

// ** BSON::BSON
BSON::BSON( const bson_t* value ) : m_isArray( false )
{
    bson_init( &m_bson );
    bson_copy_to( value, &m_bson );
}

// ** BSON::object
BSON BSON::object( void )
{
    return BSON( false );
}

// ** BSON::array
BSON BSON::array( void )
{
    return BSON( true );
}

// ** BSON::fromBSON
BSON BSON::fromBSON( const bson_t* bson )
{
    return BSON( bson );
}

// ** BSON::value
const bson_t* BSON::value( void ) const
{
    return &m_bson;
}

// ** BSON::remove
void BSON::remove( const std::string& key )
{
    bson_t result;

    bson_init( &result );
    bson_copy_to_excluding( &m_bson, &result, key.c_str(), NULL );
    bson_destroy( &m_bson );

    m_bson = result;
}

// ** BSON::toString
std::string BSON::toString( void ) const
{
    return bson_as_json( &m_bson, NULL );
}

// ** BSON::nextKey
void BSON::nextKey( void )
{
    m_key = m_isArray ? ::mongo::toString( atoi( m_key.c_str() ) + 1 ) : "";
}

// ** BSON::operator <<
BSON& BSON::operator << ( int value )
{
    assert( m_key != "" );

    bson_append_int32( &m_bson, m_key.c_str(), ( int )m_key.length(), value );
    nextKey();
    
    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( float value )
{
    assert( m_key != "" );

    bson_append_double( &m_bson, m_key.c_str(), ( int )m_key.length(), value );
    nextKey();

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( double value )
{
	assert( m_key != "" );

	bson_append_double( &m_bson, m_key.c_str(), ( int )m_key.length(), value );
	nextKey();

	return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const BSON* value )
{
    assert( m_key != "" );

    if( value ) {
        return *this << *value;
    }

    bson_append_null( &m_bson, m_key.c_str(), ( int )m_key.length() );
    nextKey();

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const char* value )
{
    if( m_key == "" ) {
        m_key = value;
    } else {
        bson_append_utf8( &m_bson, m_key.c_str(), ( int )m_key.length(), value, ( int )strlen( value ) );
        nextKey();
    }

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const std::string& value )
{
    if( m_key == "" ) {
        m_key = value;
    } else {
        bson_append_utf8( &m_bson, m_key.c_str(), ( int )m_key.length(), value.c_str(), ( int )value.length() );
        nextKey();
    }
    
    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const IntegerArray& value )
{
    BSON array = BSON::array();

    for( int i = 0, n = ( int )value.size(); i < n; i++ ) {
        array << value[i];
    }

    return *this << array;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const FloatArray& value )
{
    BSON array = BSON::array();

    for( int i = 0, n = ( int )value.size(); i < n; i++ ) {
        array << value[i];
    }
    
    return *this << array;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const BSON& value )
{
    assert( m_key != "" );

    if( value.m_isArray ) {
        bson_append_array( &m_bson, m_key.c_str(), ( int )m_key.length(), &value.m_bson );
    } else {
        bson_append_document( &m_bson, m_key.c_str(), ( int )m_key.length(), &value.m_bson );
    }

    nextKey();

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const OID& value )
{
    assert( m_key != "" );
    bson_append_oid( &m_bson, m_key.c_str(), ( int )m_key.length(), value.value() );
    nextKey();
    
    return *this;
}

} // namespace mongo