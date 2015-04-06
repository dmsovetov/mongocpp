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

#include "MongoBson.h"
#include "Collection.h"

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
            result.push_back( ( float )bson_iter_double( &i ) );
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
	assert( oid.length() == 24 );
    bson_oid_init_from_string( &m_oid, oid.c_str() );
}

// ** OID::value
const bson_oid_t* OID::value( void ) const
{
    return &m_oid;
}

// ** OID::generate
OID OID::generate( void )
{
	bson_oid_t oid;
	bson_oid_init( &oid, NULL );
	return OID( oid );
}

// ** OID::bytes
const unsigned char* OID::bytes( void ) const
{
	return m_oid.bytes;
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

} // namespace mongo