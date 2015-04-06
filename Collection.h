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

#ifndef __Mongo_Collection_H__
#define __Mongo_Collection_H__

#include "MongoBson.h"

namespace mongo {

	//! MongoDB collection accessor.
    class Collection {
    friend class Connection;
    public:

                                ~Collection( void );

		//! Drops the collection.
        void                    drop( void );

		//! Find a documents that match the query parameter.
		/*!
		\param query Document query.
		\return The resulting cursor instance.
		*/
        CursorPtr               find( const BSON& query = BSON::object() ) const;

		//! Find a single document that matches a query.
		/*!
		\param query Document query.
		\return Document instance that matches a query, otherwise NULL.
		*/
        DocumentPtr             findOne( const BSON& query ) const;

		//! Updates a documents that match an update criteria.
        bool                    update( const BSON& query, const BSON& value );

		//! Updates or creates a new record.
        bool                    upsert( const BSON& query, const BSON& value );

		//! Inserts a new document to a collection.
        bool                    insert( const BSON& value );

		//! Removes documents that match a query criteria.
        bool                    remove( const BSON& query );

		//! Returns the total number of documents that match a specified criteria.
        int                     count( const BSON& query = BSON::object() ) const;

		//! Merges two collections.
        void                    mergeWith( const CollectionPtr& other, bool drop = false );

		//! Creates a new collection index.
        bool                    ensureIndex( const std::string& name, const BSON& keys, bool unique = false );

		//! Removes a collection index.
        bool                    dropIndex( const std::string& name );

		//! Creates a bulk operation instance to work with this collection.
        BulkOperationPtr        createBulkOperation( void );

    private:

								//! Constructs a Collection instance.
                                Collection( mongoc_collection_t* collection );

    private:

		//! Actual collection pointer.
        mongoc_collection_t*    m_collection;
    };

} // namespace mongo

#endif	/*	!__Mongo_Collection_H__	*/
