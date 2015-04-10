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
BSON::BSON( void ) : m_bson( bson_new() )
{
	bson_init( raw() );
}

// ** BSON::BSON
BSON::BSON( bson_t* value ) : m_bson( value )
{

}

// ** BSON::object
BSON BSON::object( void )
{
    return BSON();
}

// ** BSON::set
void BSON::set( const char* key, bool value )
{
	bson_append_bool( raw(), key, strlen( key ), value );
}

// ** BSON::set
void BSON::set( const char* key, int value )
{
	bson_append_int32( raw(), key, strlen( key ), value );
}

// ** BSON::set
void BSON::set( const char* key, double value )
{
	bson_append_double( raw(), key, strlen( key ), value );
}

// ** BSON::set
void BSON::set( const char* key, const std::string& value )
{
	bson_append_utf8( raw(), key, strlen( key ), value.c_str(), value.length() );
}

// ** BSON::set
void BSON::set( const char* key, const char* value )
{
	bson_append_utf8( raw(), key, strlen( key ), value, strlen( value ) );
}

// ** BSON::set
void BSON::set( const char* key, const OID& value )
{
	bson_append_oid( raw(), key, strlen( key ), value.value() );
}

// ** BSON::setNull
void BSON::setNull( const char* key )
{
	bson_append_null( raw(), key, strlen( key ) );
}

// ** BSON::setArray
void BSON::setArray( const char* key, const BSON& value )
{
	bson_append_array( raw(), key, strlen( key ), value.raw() );
}

// ** BSON::setDocument
void BSON::setDocument( const char* key, const BSON& value )
{
	bson_append_document( raw(), key, strlen( key ), value.raw() );
}

// ** BSON::copy
bson_t* BSON::copy( void ) const
{
	return bson_copy( raw() );
}

// ** BSON::raw
bson_t* BSON::raw( void ) const
{
	return m_bson.get();
}

// ** BSON::iter
IterPtr BSON::iter( void ) const
{
	IterPtr result( new Iter( m_bson.get() ) );

	if( !result->raw() ) {
		return IterPtr();
	}

	return result;
}

// ** BSON::find
IterPtr BSON::find( const char* key )
{
	bson_iter_t  iter;
	bson_iter_t* field = new bson_iter_t;

	if( bson_iter_init( &iter, raw() ) && bson_iter_find_descendant( &iter, key, field ) ) {
		return IterPtr( new Iter( field ) );
    }

	delete field;
	return IterPtr();
}

// -------------------------------------- DocumentSelector ------------------------------------- //

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( const OID& value )
{
	assert( m_key != "" );
	set( m_key.c_str(), value );
	m_key = "";

	return *this;
}

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( bool value )
{
	assert( m_key != "" );
	set( m_key.c_str(), value );
	m_key = "";

	return *this;
}

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( const char* value )
{
	if( m_key == "" ) {
		m_key = value;
	} else {
		set( m_key.c_str(), value );
		m_key = "";
	}

	return *this;
}

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( const std::string& value )
{
	return *this << value.c_str();
}

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( const BSON* value )
{
	assert( m_key.c_str() != "" );
	setNull( m_key.c_str() );
	m_key = "";

	return *this;
}

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( const DocumentSelector& value )
{
	assert( m_key.c_str() != "" );
	setDocument( m_key.c_str(), BSON( value.copy() ) );
	m_key = "";

	return *this;
}

// ** DocumentSelector::operator <<
DocumentSelector& DocumentSelector::operator << ( const ArraySelector& value )
{
	assert( m_key.c_str() != "" );
	setArray( m_key.c_str(), BSON( value.copy() ) );
	m_key = "";

	return *this;
}

// ** DocumentSelector::byId
DocumentSelector DocumentSelector::byId( const OID& oid )
{
	return SELECTOR( "_id" << oid );
}

// ** DocumentSelector::byId
DocumentSelector DocumentSelector::byId( const std::string& oid )
{
	return SELECTOR( "_id" << OID( oid ) );
}

// ** DocumentSelector::exists
DocumentSelector DocumentSelector::exists( const char* field, bool value )
{
	return SELECTOR( field << DOCUMENT( "$exists" << value ) );
}

// -------------------------------------- ArraySelector ------------------------------------- //

// ** ArraySelector::ArraySelector
ArraySelector::ArraySelector( void ) : m_index( 0 )
{
}

// ** ArraySelector::operator <<
ArraySelector& ArraySelector::operator << ( const OID& value )
{
	set( key().c_str(), value );
	return *this;
}

// ** ArraySelector::operator <<
ArraySelector& ArraySelector::operator << ( const char* value )
{
	set( key().c_str(), value );
	return *this;
}

// ** ArraySelector::operator <<
ArraySelector& ArraySelector::operator << ( const std::string& value )
{
	return *this << value.c_str();
}

// ** ArraySelector::operator <<
ArraySelector& ArraySelector::operator << ( const DocumentSelector& value )
{
	setDocument( key().c_str(), BSON( value.copy() ) );
	return *this;
}

// ** ArraySelector::operator <<
ArraySelector& ArraySelector::operator << ( const ArraySelector& value )
{
	setArray( key().c_str(), BSON( value.copy() ) );
	return *this;
}

// ** ArraySelector::key
std::string ArraySelector::key( void )
{
	char buffer[16];
	return itoa( m_index++, buffer, 10 );
}

// ---------------------------------------- Iter --------------------------------------- //

// ** Iter::Iter
Iter::Iter( const bson_t* bson )
{
	m_iter = BsonIteratorPtr( new bson_iter_t );
	
	if( !bson_iter_init( raw(), bson ) ) {
		m_iter = BsonIteratorPtr();
	}
	else if( !bson_iter_next( raw() ) ) {
		m_iter = BsonIteratorPtr();
	}
}

// ** Iter::Iter
Iter::Iter( bson_iter_t* iter )
{
	m_iter = BsonIteratorPtr( iter );
}

// ** Iter::raw
bson_iter_t* Iter::raw( void ) const
{
	return m_iter.get();
}

// ** Iter::type
ValueType Iter::type( void ) const
{
	switch( bson_iter_type( raw() ) ) {
	case BSON_TYPE_BOOL:		return BsonBoolean;
	case BSON_TYPE_INT32:		return BsonInt32;
	case BSON_TYPE_DOUBLE:		return BsonDouble;
	case BSON_TYPE_OID:			return BsonObjectId;
	case BSON_TYPE_UTF8:		return BsonString;
	case BSON_TYPE_ARRAY:		return BsonArray;
	case BSON_TYPE_DOCUMENT:	return BsonObject;
	}

	return BsonNull;
}

// ** Iter::key
const char* Iter::key( void ) const
{
	return bson_iter_key( raw() );
}

// ** Iter::next
bool Iter::next( void )
{
	return bson_iter_next( raw() );
}

// ** Iter::toBool
bool Iter::toBool( void ) const
{
	if( bson_iter_type( raw() ) == BSON_TYPE_BOOL ) {
		return bson_iter_bool( raw() );
	}

	return false;
}

// ** Iter::toInt
int Iter::toInt( void ) const
{
	if( bson_iter_type( raw() ) == BSON_TYPE_INT32 ) {
		return bson_iter_int32( raw() );
	}

	return 0;
}

// ** Iter::toDouble
double Iter::toDouble( void ) const
{
	if( bson_iter_type( raw() ) == BSON_TYPE_DOUBLE ) {
		return bson_iter_double( raw() );
	}

	return 0.0;
}

// ** Iter::toObjectId
OID Iter::toObjectId( void ) const
{
	if( bson_iter_type( raw() ) == BSON_TYPE_OID ) {
		return OID( *bson_iter_oid( raw() ) );
	}

	return OID( "000000000000000000000000" );
}


// ** Iter::toString
const char* Iter::toString( void ) const
{
	uint32_t length = 0;

	if( bson_iter_type( raw() ) == BSON_TYPE_UTF8 ) {
		return bson_iter_utf8( raw(), &length );
	}

	return "";
}

// ** Iter::toArray
BSON Iter::toArray( void ) const
{
	if( bson_iter_type( raw() ) == BSON_TYPE_ARRAY ) {
		const uint8_t* data;
        uint32_t       length;
        bson_iter_array( raw(), &length, &data );
		
		bson_t* array = bson_new();
        bson_init_static( array, data, length );

		return BSON( array );
	}

	return BSON( ( bson_t* )NULL );
}

// ** Iter::toObject
BSON Iter::toObject( void ) const
{
	if( bson_iter_type( raw() ) == BSON_TYPE_DOCUMENT ) {
		const uint8_t* data;
        uint32_t       length;
        bson_iter_document( raw(), &length, &data );
		
		bson_t* document = bson_new();
        bson_init_static( document, data, length );

		return BSON( document );
	}

	return BSON( ( bson_t* )NULL );
}

} // namespace mongo