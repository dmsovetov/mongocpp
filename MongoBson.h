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

#ifndef __Mongocpp_BSON_H__
#define __Mongocpp_BSON_H__

#include "Mongo.h"

namespace mongo {

	//! BSON value type
	enum ValueType {
		BsonNull,
		BsonBoolean,
		BsonDouble,
		BsonInt32,
		BsonString,
		BsonObjectId,
		BsonObject,
		BsonArray,
	};

	//! BSON object iterator.
	class Iter {
	public:

								//! Constructs Iter instance.
								Iter( const bson_t* bson );

								//! Constructs Iter instance from BSON iterator.
								Iter( bson_iter_t* iter );

		//! Switches to a next value.
		bool					next( void );

		//! Returns raw iterator pointer.
		bson_iter_t*			raw( void ) const;

		//! Returns iterator value type.
		ValueType				type( void ) const;

		//! Returns current iterator key.
		const char*				key( void ) const;

		//! Returns boolean iterator value.
		bool					toBool( void ) const;

		//! Returns string iterator value.
		const char*				toString( void ) const;

		//! Returns integer iterator value.
		int						toInt( void ) const;

		//! Returns double iterator value.
		double					toDouble( void ) const;

		//! Returns ObjectId iterator value.
		OID						toObjectId( void ) const;

		//! Returns array iterator value.
		BSON					toArray( void ) const;

		//! Returns object iterator value.
		BSON					toObject( void ) const;

	private:

		//! BSON iterator pointer type.
		typedef std::shared_ptr<bson_iter_t> BsonIteratorPtr;

		//! Bson object iterator.
		BsonIteratorPtr			m_iter;
	};

	//! BSON iterator pointer type.
	typedef std::shared_ptr<Iter> IterPtr;

	//! The BSON object wrapper.
    class BSON {
    public:

		//! Returns an iterator.
		IterPtr					iter( void ) const;

		//! Finds a field with a specified key.
		IterPtr					find( const char* key );

		//! Return the raw BSON pointer.
		bson_t*					raw( void ) const;

		//! Copies the raw BSON value.
		bson_t*					copy( void ) const;

		//! Appends boolean value to BSON.
		void					set( const char* key, bool value );

		//! Appends integer value to BSON.
		void					set( const char* key, int value );

		//! Appends double value to BSON.
		void					set( const char* key, double value );

		//! Appends string value to BSON.
		void					set( const char* key, const char* value );
		void					set( const char* key, const std::string& value );

		//! Appends ObjectId value to BSON.
		void					set( const char* key, const OID& value );

		//! Appends null to BSON.
		void					setNull( const char* key );

		//! Appends document value to BSON.
		void					setDocument( const char* key, const BSON& value );

		//! Appends array value to BSON.
		void					setArray( const char* key, const BSON& value );

		//! Constructs a BSON object.
        static BSON             object( void );

    public:


								//! Constructs BSON instance with a specified type.
								BSON( void );

								//! Constructs BSON instante from internal MongoDB BSON.
								BSON( bson_t* bson );

    private:

		//! BSON pointer type.
		typedef std::shared_ptr<bson_t> BsonPtr;

		//! Internal BSON object.
		BsonPtr					m_bson;
    };

	class ArraySelector;

	//! A BSON document wrapper to simplify selector construction.
	class DocumentSelector : public BSON {
	public:

		//! Appends boolean value to selector.
		DocumentSelector&			operator << ( bool value );

		//! Appends ObjectId value to selector.
		DocumentSelector&			operator << ( const OID& value );

		//! Appends string value to selector.
		DocumentSelector&			operator << ( const char* value );
		DocumentSelector&			operator << ( const std::string& value );

		//! Appends null to selector.
		DocumentSelector&			operator << ( const BSON* value );

		//! Concatenates two selectors.
		DocumentSelector&			operator << ( const DocumentSelector& value );

		//! Appends arary value to selector.
		DocumentSelector&			operator << ( const ArraySelector& value );

		//! Constructs an ObjectId selector.
		static DocumentSelector		byId( const OID& oid );
		static DocumentSelector		byId( const std::string& oid );

		//! Constructs selector that checks for existance of a specified field.
		static DocumentSelector		exists( const char* field, bool value = true );

	protected:

		//! Active key.
		std::string					m_key;
	};

	//! A shortcut for DocumentSelector class.
	typedef DocumentSelector Selector;

	//! A BSON array wrapper to simplify selector construction.
	class ArraySelector : public BSON {
	public:

							//! Constructs ArraySelector instance.
							ArraySelector( void );

		//! Appends ObjectId value to selector.
		ArraySelector&		operator << ( const OID& value );

		//! Appends string value to selector.
		ArraySelector&		operator << ( const char* value );
		ArraySelector&		operator << ( const std::string& value );

		//! Appends document to an array selector.
		ArraySelector&		operator << ( const DocumentSelector& value );

		//! Appends array to an array selector.
		ArraySelector&		operator << ( const ArraySelector& value );

	private:

		//! Generates a key string.
		std::string			key( void );

	private:

		//! Current array index.
		int					m_index;
	};

} // namespace mongo

#endif	/*	!__Mongocpp_BSON_H__	*/