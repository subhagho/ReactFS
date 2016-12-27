/*!
 *  Auto-generated types for schema definition [source schema: test_schema]
 *
 *  Reference Schema Version : 0001.0000
 *  Generated by : subho
 *  Generated on : 2016-12-27 16:52:56.868
 */


#ifndef TEST_SCHEMA_H
#define TEST_SCHEMA_H
#include "types/includes/type_defs.h"
#include "types/includes/__base_type.h"

#include "test_type.h"
#include "test_ref_type.h"


using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;



namespace com {
	namespace wookler {
		namespace test {
			class test_schema  : public com::wookler::reactfs::core::types::__base_type
			{
			private:
				char * key = nullptr;

				long * testLong = nullptr;

				short * shortWithDefault = nullptr;

				test_ref_type * testTypeRef = nullptr;

				vector<test_type *> * testListRef = nullptr;



				void set_value_key ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "key" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								char * __var = static_cast<char *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( char ) );
								this->key = __var;

							} else {
								this->key = nullptr;
							}
						} else {
							this->key = nullptr;
						}
					} else {
						this->key = nullptr;
					}
				}

				void set_map_key ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->key ) ) {
						__data->insert ( {"key", this->key} );

					}
				}

				void set_value_testLong ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testLong" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								long * __var = static_cast<long *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( long ) );
								this->testLong = __var;

							} else {
								this->testLong = nullptr;
							}
						} else {
							this->testLong = nullptr;
						}
					} else {
						this->testLong = nullptr;
					}
				}

				void set_map_testLong ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testLong ) ) {
						__data->insert ( {"testLong", this->testLong} );

					}
				}

				void set_value_shortWithDefault ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "shortWithDefault" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								short * __var = static_cast<short *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( short ) );
								this->shortWithDefault = __var;

							} else {
								this->shortWithDefault = nullptr;
							}
						} else {
							this->shortWithDefault = nullptr;
						}
					} else {
						this->shortWithDefault = nullptr;
					}
				}

				void set_map_shortWithDefault ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->shortWithDefault ) ) {
						__data->insert ( {"shortWithDefault", this->shortWithDefault} );

					}
				}

				__struct_datatype__ *serialize_test_ref_type ( test_ref_type * __value )
				{
					CHECK_NOT_NULL ( __value );
					__struct_datatype__ *__data = __value->serialize();
					CHECK_NOT_NULL ( __data );
					return __data;
				}

				test_ref_type * deserialize_test_ref_type ( void *__input )
				{
					CHECK_NOT_NULL ( __input );
					__struct_datatype__ *__value = static_cast<__struct_datatype__ *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( __struct_datatype__ ) );
					test_ref_type * __data = new test_ref_type ( __value );
					CHECK_ALLOC ( __data, TYPE_NAME ( test_ref_type ) );
					return __data;
				}

				void set_value_testTypeRef ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testTypeRef" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								test_ref_type * __var = deserialize_test_ref_type ( __ptr );
								CHECK_NOT_NULL ( __var );
								this->testTypeRef = __var;

							} else {
								this->testTypeRef = nullptr;
							}
						} else {
							this->testTypeRef = nullptr;
						}
					} else {
						this->testTypeRef = nullptr;
					}
				}

				void set_map_testTypeRef ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testTypeRef ) ) {
						void *__ptr = serialize_test_ref_type ( this->testTypeRef );
						CHECK_NOT_NULL ( __ptr );
						__data->insert ( {"testTypeRef", __ptr} );

					}
				}

				__struct_datatype__ *serialize_test_type ( test_type * __value )
				{
					CHECK_NOT_NULL ( __value );
					__struct_datatype__ *__data = __value->serialize();
					CHECK_NOT_NULL ( __data );
					return __data;
				}

				test_type * deserialize_test_type ( void *__input )
				{
					CHECK_NOT_NULL ( __input );
					__struct_datatype__ *__value = static_cast<__struct_datatype__ *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( __struct_datatype__ ) );
					test_type * __data = new test_type ( __value );
					CHECK_ALLOC ( __data, TYPE_NAME ( test_type ) );
					return __data;
				}

				std::vector<__struct_datatype__ *> *serialize_list_vector_test_type___ ( std::vector<test_type *> *__value )
				{
					CHECK_NOT_EMPTY_P ( __value );
					std::vector<__struct_datatype__ *> *__list = new std::vector<__struct_datatype__ *>();
					CHECK_ALLOC ( __list, TYPE_NAME ( vector ) );
					for ( test_type * rec : * ( __value ) ) {
						CHECK_NOT_NULL ( rec );
						__struct_datatype__ *rv = this->serialize_test_type ( rec );
						CHECK_NOT_NULL ( rv );
						__list->push_back ( rv );
					}
					return __list;
				}

				std::vector<test_type *> *deserialize_list_vector_test_type___ ( void *__input )
				{
					CHECK_NOT_NULL ( __input );
					std::vector<__struct_datatype__ *> *__value = static_cast<std::vector<__struct_datatype__ *> *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( vector ) );

					std::vector<test_type *> *__list = new std::vector<test_type *>();
					CHECK_ALLOC ( __list, TYPE_NAME ( vector ) );
					for ( __struct_datatype__ *v : *__value ) {
						CHECK_NOT_NULL ( v );
						test_type * __tv = deserialize_test_type ( v );
						CHECK_NOT_NULL ( __tv );
						__list->push_back ( __tv );
					}
					return __list;
				}

				void set_value_testListRef ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testListRef" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								std::vector<test_type *> *__var = deserialize_list_vector_test_type___ ( __ptr );
								CHECK_NOT_NULL ( __var );
								this->testListRef = __var;

							} else {
								this->testListRef = nullptr;
							}
						} else {
							this->testListRef = nullptr;
						}
					} else {
						this->testListRef = nullptr;
					}
				}

				void set_map_testListRef ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testListRef ) ) {
						void *__ptr = serialize_list_vector_test_type___ ( this->testListRef );
						CHECK_NOT_NULL ( __ptr );
						__data->insert ( {"testListRef", __ptr} );

					}
				}


			public:
				void set_key ( char * key )
				{
					this->key = key;
				}

				char * get_key ( void )
				{
					return this->key;
				}

				void set_key ( string &key )
				{
					PRECONDITION ( this->__is_allocated == true );
					FREE_PTR ( this->key );
					if ( !IS_EMPTY ( key ) ) {
						uint32_t __size = key.length() + 1;
						this->key = ( char * ) malloc ( sizeof ( char ) * __size );
						CHECK_ALLOC ( this->key, TYPE_NAME ( char * ) );
						memset ( this->key, 0, __size );
						memcpy ( this->key, key.c_str(), ( __size - 1 ) );
					}
				}

				void set_testLong ( long * testLong )
				{
					this->testLong = testLong;
				}

				long * get_testLong ( void )
				{
					return this->testLong;
				}

				void set_testLong ( long &testLong )
				{
					PRECONDITION ( this->__is_allocated == true );
					FREE_PTR ( this->testLong );
					this->testLong = ( long * ) malloc ( sizeof ( long ) );
					CHECK_ALLOC ( this->testLong, TYPE_NAME ( long ) );
					* ( this->testLong ) = testLong;
				}

				void set_shortWithDefault ( short * shortWithDefault )
				{
					this->shortWithDefault = shortWithDefault;
				}

				short * get_shortWithDefault ( void )
				{
					return this->shortWithDefault;
				}

				void set_shortWithDefault ( short &shortWithDefault )
				{
					PRECONDITION ( this->__is_allocated == true );
					FREE_PTR ( this->shortWithDefault );
					this->shortWithDefault = ( short * ) malloc ( sizeof ( short ) );
					CHECK_ALLOC ( this->shortWithDefault, TYPE_NAME ( short ) );
					* ( this->shortWithDefault ) = shortWithDefault;
				}

				test_ref_type * get_testTypeRef ( void )
				{
					return this->testTypeRef;
				}

				void set_testTypeRef ( test_ref_type * testTypeRef )
				{
					this->testTypeRef = testTypeRef;
				}

				vector<test_type *> * get_testListRef ( void )
				{
					return this->testListRef;
				}

				void set_testListRef ( vector<test_type *> * testListRef )
				{
					this->testListRef = testListRef;
				}

				void add_to_testListRef ( test_type * testListRef )
				{
					PRECONDITION ( this->__is_allocated == true );
					CHECK_NOT_NULL ( testListRef );
					if ( IS_NULL ( this->testListRef ) ) {
						this->testListRef = new std::vector<test_type *>();
						CHECK_ALLOC ( this->testListRef, TYPE_NAME ( vector ) );
					}
					this->testListRef->push_back ( testListRef );
				}

				test_schema()
				{
					this->__is_allocated = true;
					this->key = nullptr;
					this->testLong = nullptr;
					this->shortWithDefault = nullptr;
					this->testTypeRef = nullptr;
					this->testListRef = nullptr;

				}

				test_schema ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					this->__is_allocated = false;
					this->key = nullptr;
					this->testLong = nullptr;
					this->shortWithDefault = nullptr;
					this->testTypeRef = nullptr;
					this->testListRef = nullptr;

					this->deserialize ( __data );
				}

				~test_schema()
				{
					if ( this->__is_allocated ) {
						FREE_PTR ( this->key );
					}

					if ( this->__is_allocated ) {
						FREE_PTR ( this->testLong );
					}

					if ( this->__is_allocated ) {
						FREE_PTR ( this->shortWithDefault );
					}

					CHECK_AND_FREE ( this->testTypeRef );

					FREE_TYPE_LIST ( this->testListRef );


				}

				void deserialize ( __struct_datatype__ *__data ) override
				{
					CHECK_NOT_NULL ( __data );
					this->set_value_key ( __data );

					this->set_value_testLong ( __data );

					this->set_value_shortWithDefault ( __data );

					this->set_value_testTypeRef ( __data );

					this->set_value_testListRef ( __data );


				}

				__struct_datatype__ *serialize() override
				{
					__struct_datatype__ *__data = new __struct_datatype__();
					CHECK_ALLOC ( __data, TYPE_NAME ( __struct_datatype__ ) );
					this->set_map_key ( __data );

					this->set_map_testLong ( __data );

					this->set_map_shortWithDefault ( __data );

					this->set_map_testTypeRef ( __data );

					this->set_map_testListRef ( __data );


					return __data;
				}


			};

		}

	}

}

#endif // TEST_SCHEMA_H

