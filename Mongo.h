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

#include <mongoc.h>
#include <bcon.h>
#include <set>
#include <vector>
#include <string>

#define DOCUMENT( x )   (mongo::BSON::object() << x)
#define ARRAY( x )      (mongo::BSON::array()  << x)
#define QUERY( x )       mongo::BSON::object() << x
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

	//! Converts an integer to a string.
	inline std::string toString( int value )
	{
		char buf[32];
		sprintf( buf, "%d", value );
		return buf;
	}

    // ** class OID
    class OID {
    public:

                                OID( bson_oid_t oid );
                                OID( const std::string& oid );

        bool                    operator == ( const OID& other ) const;

        const bson_oid_t*       value( void ) const;
        std::string             toString( void ) const;

    private:

        bson_oid_t              m_oid;
    };

    // ** BSON
    class BSON {
    public:

        BSON&                   operator << ( int value );
        BSON&                   operator << ( float value );
        BSON&                   operator << ( const BSON* value );
        BSON&                   operator << ( const char* value );
        BSON&                   operator << ( const std::string& value );
        BSON&                   operator << ( const IntegerArray& value );
        BSON&                   operator << ( const FloatArray& value );
        BSON&                   operator << ( const OID& value );
        BSON&                   operator << ( const BSON& value );

        const bson_t*           value( void ) const;

        std::string             toString( void ) const;
        void                    remove( const std::string& key );

        static BSON             array( void );
        static BSON             object( void );
        static BSON             fromBSON( const bson_t* bson );

    private:

                                BSON( bool isArray = false );
                                BSON( const bson_t* value );

        void                    nextKey( void );

    private:

        bool                    m_isArray;
        std::string             m_key;
        bson_t                  m_bson;
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

    // ** class Collection
    class Collection {
    friend class Connection;
    public:

                                ~Collection( void );

        void                    drop( void );
        CursorPtr               find( const BSON& query = BSON::object() ) const;
        DocumentPtr             findOne( const BSON& query ) const;
        bool                    update( const BSON& query, const BSON& value );
        bool                    upsert( const BSON& query, const BSON& value );
        bool                    insert( const BSON& value );
        bool                    remove( const BSON& query );
        int                     count( const BSON& query = BSON::object() ) const;
        void                    mergeWith( const CollectionPtr& other, bool drop = false );

        bool                    ensureIndex( const std::string& name, const BSON& keys, bool unique = false );
        bool                    dropIndex( const std::string& name );

        BulkOperationPtr        createBulkOperation( void );

    private:

                                Collection( mongoc_collection_t* collection );

    private:

        mongoc_collection_t*    m_collection;
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

        const bson_t*           value( void ) const;
        OID                     _id( void ) const;
        OID                     objectId( const char* key ) const;
        std::string             string( const char* key ) const;
        double                  number( const char* key ) const;
        int                     integer( const char* key ) const;
        DocumentPtr             array( const char* key ) const;
        StringSet               keys( void ) const;
        IntegerSet              integerSet( const char* key ) const;
        FloatArray              numbers( const char* key ) const;

    private:

                                Document( bson_t* document );

    private:

        bson_t*                 m_document;
    };

}


#endif /*	!__Mongocpp_Mongo_H__	*/
