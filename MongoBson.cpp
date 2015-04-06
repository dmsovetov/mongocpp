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

#include "MongoBson.h"

namespace mongo {

// ** BSON::BSON
BSON::BSON( bool isArray ) : m_isArray( isArray ), m_key( isArray ? "0" : "" )
{
	m_bson = BsonPtr( new bson_t );
	bson_init( m_bson.get() );
}

// ** BSON::BSON
BSON::BSON( const BsonPtr& value ) : m_isArray( false ), m_bson( value )
{

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
	return m_bson.get();
}

// ** BSON::remove
void BSON::remove( const std::string& key )
{
	assert( false );
//    bson_t result;

//    bson_init( &result );
//    bson_copy_to_excluding( m_bson.get(), &result, key.c_str(), NULL );
//    bson_destroy( m_bson.get() );

//    m_bson = result;
}

// ** BSON::toString
std::string BSON::toString( void ) const
{
    return bson_as_json( m_bson.get(), NULL );
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

    bson_append_int32( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value );
    nextKey();
    
    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( float value )
{
    assert( m_key != "" );

    bson_append_double( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value );
    nextKey();

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( double value )
{
	assert( m_key != "" );

	bson_append_double( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value );
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

    bson_append_null( m_bson.get(), m_key.c_str(), ( int )m_key.length() );
    nextKey();

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const char* value )
{
    if( m_key == "" ) {
        m_key = value;
    } else {
        bson_append_utf8( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value, ( int )strlen( value ) );
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
        bson_append_utf8( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value.c_str(), ( int )value.length() );
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
        bson_append_array( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value.m_bson.get() );
    } else {
        bson_append_document( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value.m_bson.get() );
    }

    nextKey();

    return *this;
}

// ** BSON::operator <<
BSON& BSON::operator << ( const OID& value )
{
    assert( m_key != "" );
    bson_append_oid( m_bson.get(), m_key.c_str(), ( int )m_key.length(), value.value() );
    nextKey();
    
    return *this;
}

} // namespace mongo