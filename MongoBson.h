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

	//! The BSON object wrapper.
    class BSON {
    public:

		//! Appends an integer value.
        BSON&                   operator << ( int value );

		//! Appends a double value.
        BSON&                   operator << ( float value );

		//! Appends a double value.
		BSON&                   operator << ( double value );

		//! Appends a nested BSON.
        BSON&                   operator << ( const BSON* value );

		//! Appends a nested BSON
        BSON&                   operator << ( const BSON& value );

		//! Appends a string.
        BSON&                   operator << ( const char* value );

		//! Appends a string.
        BSON&                   operator << ( const std::string& value );

		//! Appends an array of integers.
        BSON&                   operator << ( const IntegerArray& value );

		//! Appends an array of doubles.
        BSON&                   operator << ( const FloatArray& value );

		//! Appends an ObjectId.
        BSON&                   operator << ( const OID& value );

		//! Return the internal BSON object.
        const bson_t*			value( void ) const;

		//! Converts the BSON object to a string.
        std::string             toString( void ) const;

		//! Removes the field from a BSON.
        void                    remove( const std::string& key );

		//! Constructs an array BSON object.
        static BSON             array( void );

		//! Constructs a BSON object.
        static BSON             object( void );

		//! Constructs a BSON object from internal MongoDB BSON. 
        static BSON             fromBSON( const bson_t* bson );

    public:

								//! Constructs a BSON instance.
                                BSON( bool isArray = false );

								//! Constructs a BSON instance from internal MongoDB BSON.
                                BSON( const bson_t* value );

		//! Generates a next BSON key.
        void                    nextKey( void );

    private:

		//! The flag indicating this is an array BSON.
        bool                    m_isArray;

		//! Current BSON write key.
        std::string             m_key;

		//! Internal BSON object.
        bson_t                  m_bson;
    };

} // namespace mongo

#endif	/*	!__Mongocpp_BSON_H__	*/