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

#ifndef __Mongocpp_Mongo_H__
#define __Mongocpp_Mongo_H__

#ifdef MONGO_BUILD_LIBRARY
	#include <mongoc.h>
	#include <bcon.h>
#else
	struct mongoc_client_t;
	struct mongoc_cursor_t;
	struct mongoc_collection_t;
	struct mongoc_bulk_operation_t;

	struct bson_t;
	struct bson_oid_t;
	struct bson_iter_t;
#endif

#include <set>
#include <vector>
#include <string>
#include <memory>

#define DOCUMENT( x )   (mongo::DocumentSelector() << x)
#define ARRAY( x )      (mongo::ArraySelector()	   << x)
#define SELECTOR( x )	 mongo::DocumentSelector() << x
#define NIL             (mongo::BSON*)NULL

namespace mongo {

    typedef std::shared_ptr<class Connection>       ConnectionPtr;
    typedef std::shared_ptr<class Cursor>           CursorPtr;
    typedef std::shared_ptr<class Collection>       CollectionPtr;
    typedef std::shared_ptr<class Document>         DocumentPtr;
    typedef std::shared_ptr<class BulkOperation>    BulkOperationPtr;
    typedef std::set<std::string>                   StringSet;
    typedef std::set<int>                           IntegerSet;
	typedef std::vector<int>						IntegerArray;
    typedef std::vector<float>                      FloatArray;
	typedef std::vector<std::string>				StringArray;

	class BSON;

	//! Converts an integer to a string.
	inline std::string toString( int value )
	{
		char buf[32];
		sprintf_s( buf, 32, "%d", value );
		return buf;
	}

    //! Class that wraps the MongoDB ObjectId type.
    class OID {
    public:

								//! Constructs OID instance from BSON object id.
                                OID( const bson_oid_t& oid );

								//! Construcst OID instance from ObjectId string.
                                OID( const std::string& oid );

		//! Compares two OID instances.
        bool                    operator == ( const OID& other ) const;

		//! Returns the BSON ObjectId value.
        bson_oid_t*				raw( void ) const;

		//! Returns a byte representation of an ObjectId.
		const unsigned char*	bytes( void ) const;

		//! Convers the ObjectId to a string.
        std::string             toString( void ) const;

		//! Generates a new ObjectId.
		static OID				generate( void );

    private:

		//! Internal BSON object id pointer type.
		typedef std::shared_ptr<bson_oid_t> BsonObjectIdPtr;

		//! Actual object id.
        BsonObjectIdPtr			m_oid;
    };

    // ** class Cursor
    class Cursor {
    friend class Collection;
    public:

                                ~Cursor( void );

        DocumentPtr             next( void );
        CursorPtr               clone( void );

    private:

                                Cursor( mongoc_cursor_t* cursor );

    private:

        mongoc_cursor_t*        m_cursor;
    };

    // ** class BulkOperation
    class BulkOperation {
    friend class Collection;
    public:

                                    ~BulkOperation( void );

        void                        insert( const BSON& document );
        bool                        execute( void );

    private:

                                    BulkOperation( mongoc_bulk_operation_t* bulk );

    private:

        mongoc_bulk_operation_t*    m_bulk;
    };

    // ** class Connection
    class Connection {
    public:

                                Connection( const std::string& host, const std::string& db );
                                ~Connection( void );

        CollectionPtr           collection( const std::string& name );

    private:

        std::string             m_db;
        mongoc_client_t*        m_client;
    };

    // ** class Document
    class Document {
    friend class Cursor;
    public:

                                ~Document( void );

        bson_t*					value( void ) const;
        OID                     _id( void ) const;
        OID                     objectId( const char* key ) const;
        std::string             string( const char* key ) const;
        double                  number( const char* key ) const;
        int                     integer( const char* key ) const;
        DocumentPtr             array( const char* key ) const;
		DocumentPtr				object( const char* key ) const;
        StringSet               keys( void ) const;
        IntegerSet              integerSet( const char* key ) const;
        FloatArray              numbers( const char* key ) const;
		StringArray				strings( const char* key ) const;

    private:

                                Document( bson_t* document );

    private:

        bson_t*                 m_document;
    };

}


#endif /*	!__Mongocpp_Mongo_H__	*/
